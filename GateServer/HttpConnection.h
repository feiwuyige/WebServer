#pragma once
#include "const.h"
#include "LogicSystem.h"
class HttpConnection : public std::enable_shared_from_this<HttpConnection>
{
public:
	friend class LogicSystem; //将LogicSystem类声明为友元类，所以LogicSystem类中的函数就可以访问该类中的私有成员
	HttpConnection(boost::asio::io_context& ioc);
	void Start();
	tcp::socket& GetSocket() {
		return _socket;
	}
private:
	//实现超时
	void CheckDeadline();
	void WriteResponse();
	void HandleReq();
	void PreParseGetParam();
	tcp::socket _socket;
	beast::flat_buffer _buffer{ 8192 };
	http::request<http::dynamic_body> _request;
	http::response<http::dynamic_body> _response;
	net::steady_timer _deadline{
		_socket.get_executor(),
		std::chrono::seconds(60)
	};
	std::unordered_map<std::string, std::string> _get_params;
	std::string _get_url;
};

