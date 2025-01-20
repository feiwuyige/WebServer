#include "CServer.h"
#include "HttpConnection.h"

CServer::CServer(boost::asio::io_context& ioc, unsigned short& portnum) 
	: _ioc(ioc), _acceptor(ioc, tcp::endpoint(tcp::v4(), portnum)), _socket(ioc){

}

void CServer::start(){
	auto self = shared_from_this();
	_acceptor.async_accept(_socket, [self](beast::error_code ec) {
			try{
				//�������������ӣ�����������������
				if (ec) {
					self->start();
					return;
				}
				//���������ӣ�����ʹ��HttpConnection������������
				std::make_shared<HttpConnection>(std::move(self->_socket))->Start();
				//��������
				self->start();
			}
			catch (std::exception& exp) {

			}
		});
}
