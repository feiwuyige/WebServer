#pragma once
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <memory>
#include <iostream>
#include <boost/asio.hpp>
#include <functional>
#include "Singleton.h"
#include <map>
#include <unordered_map>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>

const std::string USERIPPREFIX = "uip_";
const std::string CODEPREFIX = "code_";
const std::string USERTOKENPEFIX = "utoken_";
const std::string IPCOUNTPEFIX = "ipcount_";
const std::string USER_BASE_INFO = "ubaseinfo_";
const std::string LOGIN_COUNT = "logincount";
const std::string NAME_INFO = "nameinfo_";
const int MAX_LENGTH = 1024 * 2;
const int HEAD_TOTAL_LEN = 4;
const int HEAD_ID_LEN = 2;
const int HEAD_DATA_LEN = 2;
const int MAX_RECVQUE = 10000;
const int MAX_SENDQUE = 1000;

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

//定义错误
enum ErrorCodes {
	Success = 0,
	Error_Json = 1001,
	RPCFailed = 1002,
	VerifyExpired = 1003,
	VerifyCodeError = 1004,
	UserExist = 1005,
	PasswdErr = 1006,
	EmailNotMatch = 1007,
	PasswdUpFailed = 1008,
	PasswdInvalid = 1009,
	TokenInvalid = 1010,
	UidInvalid = 1011,
};

enum MSG_IDS {
	MSG_CHAT_LOGIN = 1005, //用户登陆
	MSG_CHAT_LOGIN_RSP = 1006,
	ID_SEARCH_USER_REQ = 1007, //用户搜索
	ID_SEARCH_USER_RSP = 1008,
	ID_ADD_FRIEND_REQ = 1009, //申请添加好友
	ID_ADD_FRIEND_RSP = 1010,
	ID_NOTIFY_ADD_FRIEND_REQ = 1011, //通知好友添加好友申请
	ID_AUTH_FRIEND_REQ = 1013, //认证好友请求
	ID_AUTH_FRIEND_RSP = 1014, //认证好友回复
	ID_NOTIFY_ADD_FRIEND_RSP = 1015, //通知用户认证好友申请
	ID_TEXT_CHAT_MSG_REQ = 1017,
	ID_TEXT_CHAT_MSG_RSP = 1018,
	ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019,
};

//定义 defer 类
class Defer {
public:
	Defer(std::function<void()> func) : func_(func){}
	~Defer() {
		func_();
	}
private:
	std::function<void()> func_;
};

