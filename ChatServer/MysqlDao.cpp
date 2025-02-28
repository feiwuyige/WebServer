#include "MysqlDao.h"
#include "ConfigMgr.h"

MySqlPool::MySqlPool(const std::string& url, const std::string& user, const std::string& pass, 
	const std::string& schema, int poolSize) : url_(url), user_(user), pass_(pass), 
	schema_(schema),poolSize_(poolSize), b_stop_(false)
{
	try {
		for (int i = 0;i < poolSize_; ++i) {
			sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
			auto con = driver->connect(url_, user_, pass_);
			con->setSchema(schema_);
			//获取当前时间
			auto currentTime = std::chrono::system_clock::now().time_since_epoch();
			long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
			pool_.push(std::make_unique<SqlConnection>(con, timestamp));
		}
		check_thread_ = std::thread([this]() {
			while (!b_stop_) {
				checkConnection();
				std::this_thread::sleep_for(std::chrono::seconds(60));
			}
			});
		check_thread_.detach();
		std::cout << "数据库连接成功" << std::endl;
	}
	catch (sql::SQLException& e) { 
		std::cout << "mysql pool init failed, error is " << e.what() << std::endl;
	}
}

void MySqlPool::checkConnection()
{
	std::lock_guard<std::mutex> guard(mutex_);
	int poolSize = pool_.size();
	auto currentTime = std::chrono::system_clock::now().time_since_epoch();
	long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(currentTime).count();
	for (int i = 0; i < poolSize; ++i) {
		auto con = std::move(pool_.front());
		pool_.pop();
		//每次循环结束，析构该变量时执行该函数，将连接放回池子
		Defer defer([this, &con]() {
			pool_.push(std::move(con));
			});
		if (timestamp - con->_last_oper_time < 5) {
			continue;
		}
		try {
			std::unique_ptr<sql::Statement> stmt(con->_con->createStatement());
			stmt->executeQuery("SELECT 1");
			con->_last_oper_time = timestamp;
			//std::cout << "execute timer alive query, cur is " << timestamp << std::endl;
		}
		catch (sql::SQLException& e) {
			std::cout << "Error keeping connection alive: " << e.what() << std::endl;
			sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
			auto newcon = driver->connect(url_, user_, pass_);
			newcon->setSchema(schema_);
			con->_con.reset(newcon);
			con->_last_oper_time = timestamp;
		}
	}
}

//获取连接
std::unique_ptr<SqlConnection> MySqlPool::getConnection()
{
	std::unique_lock<std::mutex> lock(mutex_);
	cond_.wait(lock, [this]() {
		if (b_stop_) {
			return true;
		}
		return !pool_.empty();
		});
	if (b_stop_) return nullptr;
	std::unique_ptr<SqlConnection> con(std::move(pool_.front()));
	pool_.pop();
	return con;
}
//归还连接
void MySqlPool::returnConnection(std::unique_ptr<SqlConnection> con)
{
	std::unique_lock<std::mutex> lock(mutex_);
	if (b_stop_) return;
	pool_.push(std::move(con));
	cond_.notify_one();
}

void MySqlPool::Close()
{
	b_stop_ = true;
	cond_.notify_all();
}

MySqlPool::~MySqlPool()
{
	std::unique_lock<std::mutex> lock(mutex_);
	while (!pool_.empty()) {
		pool_.pop();
	}
}

MysqlDao::MysqlDao()
{
	auto& cfg = ConfigMgr::Inst();
	const auto& host = cfg["Mysql"]["Host"];
	const auto& port = cfg["Mysql"]["Port"];
	const auto& pwd = cfg["Mysql"]["Passwd"];
	const auto& schema = cfg["Mysql"]["Schema"];
	const auto& user = cfg["Mysql"]["User"];
	pool_.reset(new MySqlPool(host + ":" + port, user, pwd, schema, 5));
}

MysqlDao::~MysqlDao()
{
	pool_->Close();
}

int MysqlDao::RegUser(const std::string& name, const std::string& email, const std::string& pwd)
{
	auto con = pool_->getConnection();
	try {
		if (con == nullptr) {
			return false;
		}
		//准备调用存储过程
		std::unique_ptr<sql::PreparedStatement> stmt(
			con->_con->prepareStatement("CALL reg_user(?,?,?,@result)"));
		stmt->setString(1, name);
		stmt->setString(2, email);
		stmt->setString(3, pwd);
		stmt->execute();
		std::cout << "获取连接成功" << std::endl;
		std::unique_ptr<sql::Statement> stmtResult(con->_con->createStatement());
		std::unique_ptr<sql::ResultSet> res(stmtResult->executeQuery("SELECT @result AS result"));
		if (res->next()) {
			int result = res->getInt("result");
			std::cout << "Result: " << result << std::endl;
			pool_->returnConnection(std::move(con));
			return result;
		}
		pool_->returnConnection(std::move(con));
		return -1;
	}
	catch (sql::SQLException& e) {
		pool_->returnConnection(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return -1;
	}
	return 0;
}

bool MysqlDao::CheckEmail(const std::string& name, const std::string& email)
{
	//取出连接
	auto con = pool_->getConnection();
	try {
		if (con == nullptr) {
			return false;
		}
		//准备查询语句
		std::unique_ptr<sql::PreparedStatement> pstmt(
			con->_con->prepareStatement("SELECT email FROM user WHERE name = ?"));
		//绑定参数
		pstmt->setString(1, name);
		//执行查询
		std::unique_ptr <sql::ResultSet> res(pstmt->executeQuery());
		//遍历结果集
		while (res->next()) {
			std::cout << "Check Email: " << res->getString("email") << std::endl;
			if (email != res->getString("email")) {
				pool_->returnConnection(std::move(con));
				return false;
			}
			pool_->returnConnection(std::move(con));
			return true;
		}
		return true;
	}
	catch (sql::SQLException& e) {
		pool_->returnConnection(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

bool MysqlDao::UpdatePwd(const std::string& name, const std::string& newpwd)
{
	auto con = pool_->getConnection();
	try {
		if (con == nullptr) {
			pool_->returnConnection(std::move(con));
			return false;
		}
		// 准备查询语句
		std::unique_ptr<sql::PreparedStatement> pstmt(
			con->_con->prepareStatement("UPDATE user SET pwd = ? WHERE name = ?"));
		// 绑定参数
		pstmt->setString(2, name);
		pstmt->setString(1, newpwd);
		// 执行更新
		int updateCount = pstmt->executeUpdate();
		std::cout << "Updated rows: " << updateCount << std::endl;
		pool_->returnConnection(std::move(con));
		return true;
	}
	catch (sql::SQLException& e) {
		pool_->returnConnection(std::move(con));
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

bool MysqlDao::CheckPwd(const std::string& name, const std::string& pwd, UserInfo& userInfo)
{
	auto con = pool_->getConnection();
	if (con == nullptr) {
		return false;
	}
	Defer defer([this, &con]() {
		pool_->returnConnection(std::move(con));
		});
	try {
		// 准备SQL语句
		std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement("SELECT * FROM user WHERE email = ?"));
		pstmt->setString(1, name); // 将username替换为你要查询的用户名
		// 执行查询
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		std::string origin_pwd = "";
		// 遍历结果集
		while (res->next()) {
			origin_pwd = res->getString("pwd");
			// 输出查询到的密码
			std::cout << "Password: " << origin_pwd << std::endl;
			break;
		}
		if (pwd != origin_pwd) {
			return false;
		}
		userInfo.name = name;
		userInfo.email = res->getString("email");
		userInfo.uid = res->getInt("uid");
		userInfo.pwd = origin_pwd;
		return true;
	}
	catch (sql::SQLException& e) {
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

bool MysqlDao::AddFriendApply(const int& from, const int& to)
{
	auto con = pool_->getConnection();
	if (con == nullptr) {
		return false;
	}

	Defer defer([this, &con]() {
		pool_->returnConnection(std::move(con));
		});
	try {
		std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement(
			"INSERT INTO friend_apply (from_uid, to_uid) values (? , ? )"
		"ON DUPLICATE KEY UPDATE from_uid = from_uid, to_uid = to_uid"));
		pstmt->setInt(1, from);
		pstmt->setInt(2, to);
		//执行更新
		int rowAffected = pstmt->executeUpdate();
		if (rowAffected < 0) {
			return false;
		}
		return true;
	}
	catch (sql::SQLException& e) {
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
	return true;
}

std::shared_ptr<UserInfo> MysqlDao::GetUser(int uid)
{
	auto con = pool_->getConnection();
	if (con == nullptr) {
		return nullptr;
	}

	Defer defer([this, &con]() {
		pool_->returnConnection(std::move(con));
		});

	try {
		std::cout << "准备查询" << std::endl;
		// 准备SQL语句
		std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement("SELECT * FROM user WHERE uid = ?"));
		pstmt->setInt(1, uid); // 将uid替换为你要查询的uid
		// 执行查询
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		std::shared_ptr<UserInfo> user_ptr = nullptr;
		std::cout << "查询完毕" << std::endl;
		// 遍历结果集
		while (res->next()) {
			user_ptr.reset(new UserInfo);
			user_ptr->pwd = res->getString("pwd");
			user_ptr->email = res->getString("email");
			user_ptr->name = res->getString("name");
			user_ptr->nick = res->getString("nick");
			user_ptr->desc = res->getString("desc");
			user_ptr->sex = res->getInt("sex");
			user_ptr->icon = res->getString("icon");
			user_ptr->uid = uid;
			break;
		}
		return user_ptr;
	}
	catch (sql::SQLException& e) {
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return nullptr;
	}
}

std::shared_ptr<UserInfo> MysqlDao::GetUser(std::string name)
{
	auto con = pool_->getConnection();
	if (con == nullptr) {
		return nullptr;
	}

	Defer defer([this, &con]() {
		pool_->returnConnection(std::move(con));
		});

	try {
		// 准备SQL语句
		std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement("SELECT * FROM user WHERE name = ?"));
		pstmt->setString(1, name); // 将uid替换为你要查询的uid

		// 执行查询
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		std::shared_ptr<UserInfo> user_ptr = nullptr;
		// 遍历结果集
		while (res->next()) {
			user_ptr.reset(new UserInfo);
			user_ptr->pwd = res->getString("pwd");
			user_ptr->email = res->getString("email");
			user_ptr->name = res->getString("name");
			user_ptr->nick = res->getString("nick");
			user_ptr->desc = res->getString("desc");
			user_ptr->sex = res->getInt("sex");
			user_ptr->uid = res->getInt("uid");
			break;
		}
		return user_ptr;
	}
	catch (sql::SQLException& e) {
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return nullptr;
	}
}
