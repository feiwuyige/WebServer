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
				//�������������ӣ�����������������
				if (ec) {
					self->start();
					return;
				}
				//���������ӣ�����ʹ��HttpConnection������������
				new_con->Start();
				//��������
				self->start();
			}
			catch (std::exception& exp) {

			}
		});
}
