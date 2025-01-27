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

		Status status = _stub->GetVerifyCode(&context, request, &reply);
		if (status.ok()) {
			return reply;
		}
		else {
			reply.set_error(ErrorCodes::RPCFailed);
			return reply;
		}
	}
private:
	VerifyClient() {
		std::shared_ptr<Channel> channel = grpc::CreateChannel("0.0.0.0:50051",
			grpc::InsecureChannelCredentials()); 
		_stub = VerifyService::NewStub(channel);
	}
	std::unique_ptr<VerifyService::Stub> _stub;
};

