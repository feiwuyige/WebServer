#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::StatusService;
using message::LoginReq;
using message::LoginRsp;



struct ChatServer {
    std::string host;
    std::string port;
    int con_count;
    std::string name;
};
class StatusServiceImpl final : public StatusService::Service
{
public:
    StatusServiceImpl();
    Status GetChatServer(ServerContext* context, const GetChatServerReq* request,
        GetChatServerRsp* reply) override;
    Status Login(ServerContext* context, const LoginReq* request,
        LoginRsp* reply) override;
private:
    void insertToken(int uid, std::string token);
    ChatServer getChatServer();
    std::unordered_map<std::string, ChatServer> _servers;
    std::mutex _server_mtx; 
};

