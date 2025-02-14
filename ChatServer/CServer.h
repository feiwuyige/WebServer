#pragma once
#include "const.h"
#include "CSession.h"
class CServer : public std::enable_shared_from_this<CServer>
{
public:
	CServer(boost::asio::io_context& ioc, short portnum);
	~CServer();
	void ClearSession(std::string);
private:
	void HandleAccept(std::shared_ptr<CSession> new_session, const boost::system::error_code& error);
	void StartAccept();
	boost::asio::io_context& _io_context;
	short _port;
	tcp::acceptor _acceptor;
	std::map<std::string, std::shared_ptr<CSession> > _sessions;
	std::mutex _mutex;
};

