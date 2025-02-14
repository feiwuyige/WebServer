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

const std::string CODEPREFIX = "code_";
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
	VerifyCodeError = 1003,
	UserExist = 1004,
	EmailNotMatch = 1005,
	PasswdUpFailed = 1006,
	PasswdInvalid = 1007,
};

enum MSG_IDS {
	MSG_CHAT_LOGIN = 1005, //用户登陆
	MSG_CHAT_LOGIN_RSP = 1006,
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

