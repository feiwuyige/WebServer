#pragma once
#include <grpcpp/grpcpp.h>
#include "const.h"
#include "message.grpc.pb.h"
#include "Singleton.h"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetVerifyReq;
using message::GetVerifyRsp;
using message::VerifyService;

class RPConPool {
public:
	RPConPool(std::size_t poolsize, std::string host, std::string port);
	~RPConPool();
	void Close();
	std::unique_ptr< VerifyService::Stub > getConnection();
	void returnConnection(std::unique_ptr<VerifyService::Stub> context);
private:
	std::atomic<bool> b_stop_;
	std::size_t pool_size_;
	std::string host_;
	std::string port_;
	std::queue<std::unique_ptr<VerifyService::Stub>> connections_;
	std::condition_variable cond_;
	std::mutex mutex_;
	
};
class VerifyClient : public Singleton<VerifyClient>
{
	friend class Singleton<VerifyClient>;
public:
	GetVerifyRsp GetVerifyCode(std::string email) {
		//构造一个请求
		ClientContext context;
		GetVerifyRsp reply;
		GetVerifyReq request;
		request.set_email(email);

		auto stub = pool_->getConnection();
		Status status = stub->GetVerifyCode(&context, request, &reply);
		if (status.ok()) {
			pool_->returnConnection(std::move(stub));
			return reply;
		}
		else {
			reply.set_error(ErrorCodes::RPCFailed);
			return reply;
		}
	}
private:
	VerifyClient();
	std::unique_ptr<RPConPool> pool_;
};

