#include "CServer.h"
#include "HttpConnection.h"

CServer::CServer(boost::asio::io_context& ioc, unsigned short& portnum) 
	: _ioc(ioc), _acceptor(ioc, tcp::endpoint(tcp::v4(), portnum)), _socket(ioc){

}

void CServer::start(){
	auto self = shared_from_this();
	_acceptor.async_accept(_socket, [self](beast::error_code ec) {
			try{
				//出错放弃这个链接，继续监听其他连接
				if (ec) {
					self->start();
					return;
				}
				//创建新连接，并且使用HttpConnection类管理这个连接
				std::make_shared<HttpConnection>(std::move(self->_socket))->Start();
				//继续监听
				self->start();
			}
			catch (std::exception& exp) {

			}
		});
}
