#include "CServer.h"
#include "AsioIOServicePool.h"
#include <iostream>
#include "UserMgr.h"

CServer::CServer(boost::asio::io_context& ioc, short portnum) 
	: _io_context(ioc), _port(portnum), _acceptor(ioc, tcp::endpoint(tcp::v4(), portnum)){
	std::cout << "Server start success, listen on port :" << _port << std::endl;
	StartAccept();
}

CServer::~CServer()
{
	std::cout << "Server destruct listen on port: " << _port << std::endl;
}

void CServer::ClearSession(std::string session_id)
{
	if (_sessions.find(session_id) != _sessions.end()) {
		UserMgr::GetInstance()->RmvUserSession(_sessions[session_id]->GetUserId());
	}
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_sessions.erase(session_id);
	}
	
}

void CServer::HandleAccept(std::shared_ptr<CSession> new_session, const boost::system::error_code& error)
{
	if (!error) {
		new_session->Start();
		std::lock_guard<std::mutex> lock(_mutex);
		_sessions.insert(std::make_pair(new_session->GetSessionId(), new_session));
		std::cout << "session accept success" << std::endl;
	}
	else {
		std::cout << "session accept failed, error is " << error.what() << std::endl;
	}
	StartAccept();
}

void CServer::StartAccept()
{
	auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
	std::shared_ptr<CSession> new_session = std::make_shared<CSession>(io_context, this);
	_acceptor.async_accept(new_session->GetSocket(), std::bind(&CServer::HandleAccept, this, new_session,
		std::placeholders::_1));
}


