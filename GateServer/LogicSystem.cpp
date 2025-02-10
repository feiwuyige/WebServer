#include "LogicSystem.h"
#include "HttpConnection.h"
#include "VerifyClient.h"
#include "RedisMgr.h"

LogicSystem::~LogicSystem()
{
}

bool LogicSystem::HandleGet(std::string path, std::shared_ptr<HttpConnection> con){
	if (_get_handlers.find(path) == _get_handlers.end()) {
		return false;
	}
	_get_handlers[path](con);
	return true;
	
}

void LogicSystem::RegGet(std::string url, HttpHandler handler){
	_get_handlers.insert(make_pair(url, handler));

}
void LogicSystem::RegPost(std::string url, HttpHandler handler) {
	_post_handlers.insert(make_pair(url, handler));

}
bool LogicSystem::HandlePost(std::string path, std::shared_ptr<HttpConnection> con)
{
	if (_post_handlers.find(path) == _post_handlers.end()) {
		return false;
	}
	_post_handlers[path](con);
	return true;
}
LogicSystem::LogicSystem(){
	RegGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {
		beast::ostream(connection->_response.body()) << "receive get_test req";
		int i = 0;
		for (auto& elem : connection->_get_params) {
			i++;
			beast::ostream(connection->_response.body()) << "param" << i << "key is " << elem.first;
			beast::ostream(connection->_response.body()) << "param" << i << "key is " << elem.second << std::endl;
		}
		});

	RegPost("/get_varifycode", [](std::shared_ptr<HttpConnection> connection) {
		//将body转化为str
		auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
		std::cout << "receive body is " << body_str << std::endl;
		connection->_response.set(http::field::content_type, "text/json");
		Json::Value root;
		Json::Reader reader;
		Json::Value src_root; //来源
		bool parse_success = reader.parse(body_str, src_root);
		if (!parse_success) {
			std::cout << "Failed to parse Json data! " << std::endl;
			root["error"] = ErrorCodes::Error_Json;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->_response.body()) << jsonstr;
			return true;
		}
		if (!src_root.isMember("email")) {
			std::cout << "Failed to parse Json data! " << std::endl;
			root["error"] = ErrorCodes::Error_Json;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->_response.body()) << jsonstr;
			return true;
		}
		auto email = src_root["email"].asString();
		GetVerifyRsp rsp = VerifyClient::GetInstance()->GetVerifyCode(email);
		std::cout << "email is " << email << std::endl;
		root["error"] = rsp.error();
		root["email"] = src_root["email"];
		std::string jsonstr = root.toStyledString();
		beast::ostream(connection->_response.body()) << jsonstr;
		return true;
		});

	RegPost("/user_register", [](std::shared_ptr<HttpConnection> connection) {
		//将body转化为str
		auto body_str = boost::beast::buffers_to_string(connection->_request.body().data());
		std::cout << "receive body is " << body_str << std::endl;
		connection->_response.set(http::field::content_type, "text/json");
		Json::Value root;
		Json::Reader reader;
		Json::Value src_root; //来源
		bool parse_success = reader.parse(body_str, src_root);
		if (!parse_success) {
			std::cout << "Failed to parse Json data! " << std::endl;
			root["error"] = ErrorCodes::Error_Json;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->_response.body()) << jsonstr;
			return true;
		}
		//查找 验证码 是否正确
		std::string varify_code;
		bool b_get_varify = RedisMgr::GetInstance()->Get(CODEPREFIX + src_root["email"].asString(),
			varify_code);
		if (!b_get_varify) {
			std::cout << "Can not get verify code" << std::endl;
			root["error"] = ErrorCodes::VerifyCodeError;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->_response.body()) << jsonstr;
			return true;
		}
		if (varify_code != src_root["varifycode"].asString()) {
			std::cout << " varify code error" << std::endl;
			root["error"] = ErrorCodes::VerifyCodeError;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->_response.body()) << jsonstr;
			return true;
		}
		//查找数据库判断用户是否存在
		root["error"] = 0;
		root["email"] = src_root["email"];
		root["user"] = src_root["user"].asString();
		root["passwd"] = src_root["passwd"].asString();
		root["confirm"] = src_root["confirm"].asString();
		root["varifycode"] = src_root["varifycode"].asString();
		std::string jsonstr = root.toStyledString();
		beast::ostream(connection->_response.body()) << jsonstr;
		return true;
		});
}
