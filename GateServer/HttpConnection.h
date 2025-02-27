#pragma once
#include "const.h"
#include "LogicSystem.h"
class HttpConnection : public std::enable_shared_from_this<HttpConnection>
{
public:
	friend class LogicSystem; //��LogicSystem������Ϊ��Ԫ�࣬����LogicSystem���еĺ����Ϳ��Է��ʸ����е�˽�г�Ա
	HttpConnection(boost::asio::io_context& ioc);
	void Start();
	tcp::socket& GetSocket() {
		return _socket;
	}
private:
	//ʵ�ֳ�ʱ
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

