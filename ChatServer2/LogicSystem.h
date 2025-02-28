#pragma once
#include "Singleton.h"
#include <queue>
#include <thread>
#include "CSession.h"
#include <map>
#include <functional>
#include "const.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <unordered_map>
#include "RedisMgr.h"
#include "data.h"

typedef std::function<void(std::shared_ptr<CSession>, const short& msg_id, const std::string& msg_data)> FunCallBack;
class LogicSystem : public Singleton<LogicSystem>
{
	friend class Singleton<LogicSystem>;
public:
	~LogicSystem();
	void PostMsgToQue(std::shared_ptr<LogicNode> msg);
private:
	LogicSystem();
	void RegisterCallBacks();
	void DealMsg();
	void LoginHandler(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data);
	void SearchInfo(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data);
	void AddFriendApply(std::shared_ptr<CSession> session, const short& msg_id, const std::string& msg_data);
	bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);
	bool isPureDigit(std::string& str);
	void GetUserByUid(std::string uid_str, Json::Value& rtvalue);
	void GetUserByName(std::string name, Json::Value& rtvalue);
	std::thread _worker_thread;
	std::queue<std::shared_ptr<LogicNode> > _msg_que;
	std::mutex _mutex;
	std::condition_variable _consume;
	bool _b_stop;
	std::map<short, FunCallBack> _fun_callbacks;
};