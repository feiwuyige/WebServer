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
			//��ȡ��ǰʱ��
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
		std::cout << "���ݿ����ӳɹ�" << std::endl;
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
		//ÿ��ѭ�������������ñ���ʱִ�иú����������ӷŻس���
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

//��ȡ����
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
//�黹����
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
		//׼�����ô洢����
		std::unique_ptr<sql::PreparedStatement> stmt(
			con->_con->prepareStatement("CALL reg_user(?,?,?,@result)"));
		stmt->setString(1, name);
		stmt->setString(2, email);
		stmt->setString(3, pwd);
		stmt->execute();
		std::cout << "��ȡ���ӳɹ�" << std::endl;
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
	//ȡ������
	auto con = pool_->getConnection();
	try {
		if (con == nullptr) {
			return false;
		}
		//׼����ѯ���
		std::unique_ptr<sql::PreparedStatement> pstmt(
			con->_con->prepareStatement("SELECT email FROM user WHERE name = ?"));
		//�󶨲���
		pstmt->setString(1, name);
		//ִ�в�ѯ
		std::unique_ptr <sql::ResultSet> res(pstmt->executeQuery());
		//���������
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
		// ׼����ѯ���
		std::unique_ptr<sql::PreparedStatement> pstmt(
			con->_con->prepareStatement("UPDATE user SET pwd = ? WHERE name = ?"));
		// �󶨲���
		pstmt->setString(2, name);
		pstmt->setString(1, newpwd);
		// ִ�и���
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
		// ׼��SQL���
		std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement("SELECT * FROM user WHERE email = ?"));
		pstmt->setString(1, name); // ��username�滻Ϊ��Ҫ��ѯ���û���
		// ִ�в�ѯ
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		std::string origin_pwd = "";
		// ���������
		while (res->next()) {
			origin_pwd = res->getString("pwd");
			// �����ѯ��������
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
		//ִ�и���
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

bool MysqlDao::GetApplyList(int touid, std::vector<std::shared_ptr<ApplyInfo>>& applyList, int begin, int limit)
{
	auto con = pool_->getConnection();
	if (con == nullptr) {
		return false;
	}

	Defer defer([this, &con]() {
		pool_->returnConnection(std::move(con));
		});

	try {
		std::cout << "׼����ѯ" << std::endl;
		// ׼��SQL���
		std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement("select apply.from_uid, apply.status, user.name, user.nick, user.sex from friend_apply as apply join user on apply.from_uid = user.uid where apply.to_uid = ? and apply.id > ? order by apply.id ASC LIMIT ?"));
		pstmt->setInt(1, touid);
		pstmt->setInt(2, begin);
		pstmt->setInt(3, limit);
		// ִ�в�ѯ
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		// ���������
		while (res->next()) {
			auto name = res->getString("name");
			auto uid = res->getInt("from_uid");
			auto status = res->getInt("status");
			auto nick = res->getString("nick");
			auto sex = res->getInt("sex");
			auto apply_ptr = std::make_shared<ApplyInfo>(uid, name, "", "", nick, sex, status);
			applyList.push_back(apply_ptr);
		}
		return true;
	}
	catch (sql::SQLException& e) {
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

bool MysqlDao::AuthFriendApply(int uid, int touid)
{
	auto con = pool_->getConnection();
	if (con == nullptr) {
		return false;
	}

	Defer defer([this, &con]() {
		pool_->returnConnection(std::move(con));
		});

	try {
		std::cout << "׼������, form " << uid << " to " << touid << std::endl;
		// ׼��SQL���
		std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement("UPDATE friend_apply SET status = 1 WHERE from_uid = ? AND to_uid = ?"));
		pstmt->setInt(1, touid);
		pstmt->setInt(2, uid);
		// ִ�в�ѯ
		int rowAffected = pstmt->executeUpdate();
		std::cout << "��ȷ" << std::endl;
		if (rowAffected < 0) {
			return false;
		}
		return true;
	}
	catch (sql::SQLException& e) {
		std::cerr << "SQLException: " << e.what();
		std::cerr << " MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

bool MysqlDao::AddFriend(int uid, int touid, std::string& back_name)
{
	auto con = pool_->getConnection();
	if (con == nullptr) {
		return false;
	}

	Defer defer([this, &con]() {
		pool_->returnConnection(std::move(con));
		});

	try {
		//��ʼ����
		con->_con->setAutoCommit(false);
		// ׼��SQL���
		std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement("INSERT IGNORE INTO friend (self_id, friend_id, back) VALUES (?, ?, ?)"));
		pstmt->setInt(1, uid);
		pstmt->setInt(2, touid);
		pstmt->setString(3, back_name);
		// ִ�и���
		int rowAffected = pstmt->executeUpdate();
		std::cout << "Insert 1 ִ�гɹ�" << std::endl;
		if (rowAffected < 0) {
			con->_con->rollback();
			return false;
		}
		//׼���ڶ���sql ���
		std::unique_ptr<sql::PreparedStatement> pstmt2(con->_con->prepareStatement("INSERT IGNORE INTO friend (self_id, friend_id, back) VALUES (?, ?, ?)"));
		pstmt2->setInt(1, touid);
		pstmt2->setInt(2, uid);
		pstmt2->setString(3, "");
		// ִ�и���
		int rowAffected2 = pstmt2->executeUpdate();
		if (rowAffected2 < 0) {
			con->_con->rollback();
			return false;
		}
		//�ύ����
		con->_con->commit();
		std::cout << "��Ӻ��Ѳ���ɹ�" << std::endl;
		return true;
	}
	catch (sql::SQLException& e) {
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
}

bool MysqlDao::GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>>& user_list)
{
	auto con = pool_->getConnection();
	if (con == nullptr) {
		return false;
	}

	Defer defer([this, &con]() {
		pool_->returnConnection(std::move(con));
		});

	try {
		// ׼��SQL���
		std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement("select * from friend where self_id = ?"));
		pstmt->setInt(1, self_id);
		// ִ�и���
		// ִ�в�ѯ
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		// ���������
		while (res->next()) {
			auto friend_id = res->getInt("friend_id");
			auto back = res->getString("back");
			//��һ�β�ѯ friend_id ��Ӧ����Ϣ
			auto user_info = GetUser(friend_id);
			if (user_info == nullptr) {
				continue;
			}
			user_info->back = user_info->name;
			user_list.push_back(user_info);
		}
		return true;
	}
	catch (sql::SQLException& e) {
		std::cerr << "SQLException: " << e.what();
		std::cerr << " (MySQL error code: " << e.getErrorCode();
		std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		return false;
	}
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
		std::cout << "׼����ѯ" << std::endl;
		// ׼��SQL���
		std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement("SELECT * FROM user WHERE uid = ?"));
		pstmt->setInt(1, uid); // ��uid�滻Ϊ��Ҫ��ѯ��uid
		// ִ�в�ѯ
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		std::shared_ptr<UserInfo> user_ptr = nullptr;
		std::cout << "��ѯ���" << std::endl;
		// ���������
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
		// ׼��SQL���
		std::unique_ptr<sql::PreparedStatement> pstmt(con->_con->prepareStatement("SELECT * FROM user WHERE name = ?"));
		pstmt->setString(1, name); // ��uid�滻Ϊ��Ҫ��ѯ��uid

		// ִ�в�ѯ
		std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
		std::shared_ptr<UserInfo> user_ptr = nullptr;
		// ���������
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
