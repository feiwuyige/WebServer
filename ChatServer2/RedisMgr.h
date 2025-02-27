#pragma once
#include "const.h"
#include "hiredis/hiredis.h"
class RedisConPool;
class RedisMgr : public Singleton<RedisMgr>
{
	friend class Singleton<RedisMgr>;
public:
	~RedisMgr();
	bool Get(const std::string& key, std::string& value);
	bool Set(const std::string& key, const std::string& value);
	bool Auth(const std::string& password);
	bool LPush(const std::string& key, const std::string& value);
	bool LPop(const std::string& key, std::string& value);
	bool RPush(const std::string& key, const std::string& value);
	bool RPop(const std::string& key, std::string& value);
	bool HSet(const std::string& key, const std::string& hkey, const std::string& value);
	bool HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen);
	bool HDel(const std::string& key, const std::string& field);
	std::string HGet(const std::string& key, const std::string& hkey);
	bool Del(const std::string& key);
	bool ExistsKey(const std::string& key);
	void Close();
private:
	RedisMgr();
	
	std::unique_ptr<RedisConPool> con_pool_;
	/*redisContext* _connect;
	redisReply* _reply;*/
};

class RedisConPool {
public:
	RedisConPool(size_t poolSize, const char* host, int port, const char* pwd);
	~RedisConPool();
	redisContext* getConnection();
	void returnConnection(redisContext* context);
	void Close();
private:
	std::atomic<bool> b_stop_;
	size_t pool_size_;
	const char* host_;
	int port_;
	std::queue<redisContext*> connections_;
	std::mutex mutex_;
	std::condition_variable cond_; //条件变量，当队列为空时让其他线程挂起
};

