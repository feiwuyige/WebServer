#include "CServer.h"
#include "HttpConnection.h"
#include "AsioIOServicePool.h"

CServer::CServer(boost::asio::io_context& ioc, unsigned short& portnum) 
	: _ioc(ioc), _acceptor(ioc, tcp::endpoint(tcp::v4(), portnum)){

}

void CServer::start(){
	auto self = shared_from_this();
	auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
	std::shared_ptr<HttpConnection> new_con = std::make_shared<HttpConnection>(io_context);
	_acceptor.async_accept(new_con->GetSocket(), [self, new_con](beast::error_code ec) {
			try{
				//出错放弃这个链接，继续监听其他连接
				if (ec) {
					self->start();
					return;
				}
				//创建新连接，并且使用HttpConnection类管理这个连接
				new_con->Start();
				//继续监听
				self->start();
			}
			catch (std::exception& exp) {

			}
		});
}
