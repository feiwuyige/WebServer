#include "RedisMgr.h"
#include "ConfigMgr.h"

RedisMgr::RedisMgr()
{
	auto& gCfgMgr = ConfigMgr::Inst();
	auto host = gCfgMgr["Redis"]["Host"];
	auto port = gCfgMgr["Redis"]["Port"];
	auto pwd = gCfgMgr["Redis"]["Passwd"];
	con_pool_.reset(new RedisConPool(5, host.c_str(), atoi(port.c_str()), pwd.c_str()));
}
RedisMgr::~RedisMgr()
{
	Close();
}


//根据 key 值取出 value
bool RedisMgr::Get(const std::string& key, std::string& value)
{
	auto connect = con_pool_->getConnection();
	if (connect == nullptr) {
		return false;
	}
	auto reply = (redisReply*)redisCommand(connect, "GET %s", key.c_str());
	if (reply == NULL) {
		std::cout << "[GET " << key << "] failed" << std::endl;
		freeReplyObject(reply);
		return false;
	}
	if (reply->type != REDIS_REPLY_STRING) {
		std::cout << "[GET " << key << "] failed" << std::endl;
		freeReplyObject(reply);
		return false;
	}
	value = reply->str;
	freeReplyObject(reply);
	std::cout << "Succeed to execute command [ GET " << key << "]" << std::endl;
	con_pool_->returnConnection(connect);
	return true;
}
//插入
bool RedisMgr::Set(const std::string& key, const std::string& value)
{
	auto connect = con_pool_->getConnection();
	if (connect == nullptr) {
		return false;
	}
	auto reply = (redisReply*)redisCommand(connect, "SET %s %s", key.c_str(), value.c_str());
	if (NULL == reply) {
		std::cout << "Execute [ SET" << key << " " << value << "] failed." << std::endl;
		freeReplyObject(reply);
		return false;
	}
	if (!(reply->type == REDIS_REPLY_STATUS 
		&& (strcmp(reply->str, "OK") == 0 || strcmp(reply->str, "ok") == 0))) {
		std::cout << "Execute [ SET" << key << " " << value << "] failed." << std::endl;
		freeReplyObject(reply);
		return false;
	}
	freeReplyObject(reply);
	std::cout << "Succeed to execute command [ SET " << key << " " << value << "]" << std::endl;
	con_pool_->returnConnection(connect);
	return true;
}

//认证服务
bool RedisMgr::Auth(const std::string& password)
{
	auto connect = con_pool_->getConnection();
	if (connect == nullptr) {
		return false;
	}
	auto reply = (redisReply*)redisCommand(connect, "AUTH %s", password.c_str());
	if (reply->type == REDIS_REPLY_ERROR) {
		std::cout << "认证失败" << std::endl;
		//执行成功 释放redisCommand执行后返回的redisReply所占用的内存
		freeReplyObject(reply);
		con_pool_->returnConnection(connect);
		return false;
	}
	else {
		//执行成功 释放redisCommand执行后返回的redisReply所占用的内存
		freeReplyObject(reply);
		std::cout << "认证成功" << std::endl;
		con_pool_->returnConnection(connect);
		return true;
	}
}
//一个 key 对应一个队列，从左边和右边插入，从左边右边弹出
bool RedisMgr::LPush(const std::string& key, const std::string& value)
{
	auto connect = con_pool_->getConnection();
	if (connect == nullptr) {
		return false;
	}
	auto reply = (redisReply*)redisCommand(connect, "LPUSH %s %s", key.c_str(), value.c_str());
	if (NULL == reply)
	{
		std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		con_pool_->returnConnection(connect);
		return false;
	}
	if (reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
		std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		con_pool_->returnConnection(connect);
		return false;
	}
	std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] success ! " << std::endl;
	freeReplyObject(reply);
	con_pool_->returnConnection(connect);
	return true;
}

bool RedisMgr::LPop(const std::string& key, std::string& value)
{
	auto connect = con_pool_->getConnection();
	if (connect == nullptr) {
		return false;
	}
	auto reply = (redisReply*)redisCommand(connect, "LPOP %s ", key.c_str());
	if (reply == nullptr || reply->type == REDIS_REPLY_NIL) {
		std::cout << "Execut command [ LPOP " << key << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		con_pool_->returnConnection(connect);
		return false;
	}
	value = reply->str;
	std::cout << "Execut command [ LPOP " << key << " ] success ! " << std::endl;
	freeReplyObject(reply);
	con_pool_->returnConnection(connect);
	return true;
}

bool RedisMgr::RPush(const std::string& key, const std::string& value)
{
	auto connect = con_pool_->getConnection();
	if (connect == nullptr) {
		return false;
	}
	auto reply = (redisReply*)redisCommand(connect, "RPUSH %s %s", key.c_str(), value.c_str());
	if (NULL == reply)
	{
		std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		con_pool_->returnConnection(connect);
		return false;
	}
	if (reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
		std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		con_pool_->returnConnection(connect);
		return false;
	}
	std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] success ! " << std::endl;
	freeReplyObject(reply);
	con_pool_->returnConnection(connect);
	return true;
}

bool RedisMgr::RPop(const std::string& key, std::string& value)
{
	auto connect = con_pool_->getConnection();
	if (connect == nullptr) {
		return false;
	}
	auto reply = (redisReply*)redisCommand(connect, "RPOP %s ", key.c_str());
	if (reply == nullptr || reply->type == REDIS_REPLY_NIL) {
		std::cout << "Execut command [ RPOP " << key << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		con_pool_->returnConnection(connect);
		return false;
	}
	value = reply->str;
	std::cout << "Execut command [ RPOP " << key << " ] success ! " << std::endl;
	freeReplyObject(reply);
	con_pool_->returnConnection(connect);
	return true;
}
//可能有多级key(类似于域名服务器)
bool RedisMgr::HSet(const std::string& key, const std::string& hkey, const std::string& value)
{
	auto connect = con_pool_->getConnection();
	if (connect == nullptr) {
		return false;
	}
	auto reply = (redisReply*)redisCommand(connect, "HSET %s %s %s", key.c_str(), hkey.c_str(), value.c_str());
	if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
		std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		con_pool_->returnConnection(connect);
		return false;
	}
	std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << value << " ] success ! " << std::endl;
	freeReplyObject(reply);
	con_pool_->returnConnection(connect);
	return true;
}

bool RedisMgr::HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen)
{
	auto connect = con_pool_->getConnection();
	if (connect == nullptr) {
		return false;
	}
	const char* argv[4];
	size_t argvlen[4];
	argv[0] = "HSET";
	argvlen[0] = 4;
	argv[1] = key;
	argvlen[1] = strlen(key);
	argv[2] = hkey;
	argvlen[2] = strlen(hkey);
	argv[3] = hvalue;
	argvlen[3] = hvaluelen;
	auto reply = (redisReply*)redisCommandArgv(connect, 4, argv, argvlen);
	if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
		std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << hvalue << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		con_pool_->returnConnection(connect);
		return false;
	}
	std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << hvalue << " ] success ! " << std::endl;
	freeReplyObject(reply);
	con_pool_->returnConnection(connect);
	return true;
}

bool RedisMgr::HDel(const std::string& key, const std::string& field)
{
	auto connect = con_pool_->getConnection();
	if (connect == nullptr) return false;
	Defer defer([&connect, this]() {
		con_pool_->returnConnection(connect);
		});
	redisReply* reply = (redisReply*)redisCommand(connect, "HDEL %s %s", key.c_str(), field.c_str());
	if (reply == nullptr) {
		std::cout << "Hdel failed" << std::endl;
		return false;
	}
	bool success = false;
	if (reply->type == REDIS_REPLY_INTEGER) {
		success = reply->integer > 0;
	}
	freeReplyObject(reply);
	return success;
}

std::string RedisMgr::HGet(const std::string& key, const std::string& hkey)
{
	auto connect = con_pool_->getConnection();
	if (connect == nullptr) {
		return "";
	}
	const char* argv[3];
	size_t argvlen[3];
	argv[0] = "HGET";
	argvlen[0] = 4;
	argv[1] = key.c_str();
	argvlen[1] = key.length();
	argv[2] = hkey.c_str();
	argvlen[2] = hkey.length();
	auto reply = (redisReply*)redisCommandArgv(connect, 3, argv, argvlen);
	if (reply == nullptr || reply->type == REDIS_REPLY_NIL) {
		freeReplyObject(reply);
		std::cout << "Execut command [ HGet " << key << " " << hkey << "  ] failure ! " << std::endl;
		con_pool_->returnConnection(connect);
		return "";
	}
	std::string value = reply->str;
	freeReplyObject(reply);
	std::cout << "Execut command [ HGet " << key << " " << hkey << " ] success ! " << std::endl;
	con_pool_->returnConnection(connect);
	return value;
}

bool RedisMgr::Del(const std::string& key)
{
	auto connect = con_pool_->getConnection();
	if (connect == nullptr) {
		return false;
	}
	auto reply = (redisReply*)redisCommand(connect, "DEL %s", key.c_str());
	if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER) {
		std::cout << "Execut command [ Del " << key << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		con_pool_->returnConnection(connect);
		return false;
	}
	std::cout << "Execut command [ Del " << key << " ] success ! " << std::endl;
	freeReplyObject(reply);
	con_pool_->returnConnection(connect);
	return true;
}

bool RedisMgr::ExistsKey(const std::string& key)
{
	auto connect = con_pool_->getConnection();
	if (connect == nullptr) {
		return false;
	}
	auto reply = (redisReply*)redisCommand(connect, "exists %s", key.c_str());
	if (reply == nullptr || reply->type != REDIS_REPLY_INTEGER || reply->integer == 0) {
		std::cout << "Not Found [ Key " << key << " ]  ! " << std::endl;
		freeReplyObject(reply);
		con_pool_->returnConnection(connect);
		return false;
	}
	std::cout << " Found [ Key " << key << " ] exists ! " << std::endl;
	freeReplyObject(reply);
	con_pool_->returnConnection(connect);
	return true;
}

void RedisMgr::Close()
{
	//回收连接池
	con_pool_->Close();
}

RedisConPool::RedisConPool(size_t poolSize, const char* host, int port, const char* pwd) :
	pool_size_(poolSize), host_(host), port_(port), b_stop_(false)
{
	for (size_t i = 0; i < pool_size_; ++i) {
		redisContext* context = redisConnect(host, port);
		if (context == nullptr || context->err != 0) {
			if (context != nullptr) {
				redisFree(context);
			}
			continue;
		}
		redisReply* reply = (redisReply*)redisCommand(context, "AUTH %s", pwd);
		if (reply->type == REDIS_REPLY_ERROR) {
			std::cout << "认证失败" << std::endl;
			freeReplyObject(reply);
			redisFree(context);
			continue;
		}
		freeReplyObject(reply);
		std::cout << "认证成功" << std::endl;
		connections_.push(context);
	}
}

RedisConPool::~RedisConPool()
{
	//std::lock_guard 是一种较轻量级的锁机制。它在构造时加锁，在析构时解锁，无法显式地手动解锁。
	std::lock_guard<std::mutex> lock(mutex_);
	while (!connections_.empty()) {
		connections_.pop();
	}
}
//从池子里取出连接
redisContext* RedisConPool::getConnection()
{
	//比 lock_guard 更加灵活，可以手动加锁解锁
	std::unique_lock<std::mutex> lock(mutex_);
	//调用 wait 时，线程会释放锁，然后进入阻塞状态，直到被其他线程唤醒
	//被唤醒后，如果谓词函数返回 false，线程继续等待，直到返回 true
	cond_.wait(lock, [this]() {
		if (b_stop_) {
			return true;
		}
		return !connections_.empty();
		});
	if (b_stop_) {
		return nullptr;
	}
	redisContext* context = connections_.front();
	connections_.pop();
	return context;
}

void RedisConPool::returnConnection(redisContext* context)
{
	//池子是临界资源，要往池子里面放，就得加锁
	std::lock_guard<std::mutex> lock(mutex_);
	if (b_stop_) {
		return;
	}
	connections_.push(context);
	//通知一个正在处于等待的线程
	cond_.notify_one();
}

void RedisConPool::Close()
{
	b_stop_ = true;
	//通知所有等待的线程
	cond_.notify_all();
}
