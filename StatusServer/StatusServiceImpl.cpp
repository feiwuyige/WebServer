#include "StatusServiceImpl.h"
#include "ConfigMgr.h"
#include "const.h"
#include "boost/uuid.hpp"
#include "RedisMgr.h"

std::string generate_unique_string() {
    // 创建UUID对象
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    // 将UUID转换为字符串
    std::string unique_string = to_string(uuid);
    return unique_string;
}
Status StatusServiceImpl::GetChatServer(ServerContext* context, const GetChatServerReq* request, GetChatServerRsp* reply)
{
    std::string prefix("chat status server has received :  ");
    
    const auto& server = getChatServer();
    reply->set_host(server.host);
    reply->set_port(server.port);
    reply->set_error(ErrorCodes::Success);
    reply->set_token(generate_unique_string());
    insertToken(request->uid(), reply->token());
    return Status::OK;
}
Status StatusServiceImpl::Login(ServerContext* context, const LoginReq* request, LoginRsp* reply)
{
    auto uid = request->uid();
    auto token = request->token();
    
    std::string uid_str = std::to_string(uid);
    std::string token_key = USERTOKENPEFIX + uid_str;
    std::string token_value = "";
    bool success = RedisMgr::GetInstance()->Get(token_key, token_value);
    if (!success) {
        reply->set_error(ErrorCodes::UidInvalid);
        return Status::OK;
    }
    if (token_value != token) {
        reply->set_error(ErrorCodes::TokenInvalid);
        return Status::OK;
    }
    reply->set_error(ErrorCodes::Success);
    reply->set_uid(uid);
    reply->set_token(token);
    return Status::OK;
}
void StatusServiceImpl::insertToken(int uid, std::string token)
{
    std::string uid_str = std::to_string(uid);
    std::string token_key = USERTOKENPEFIX + uid_str;
    RedisMgr::GetInstance()->Set(token_key, token);

}
ChatServer StatusServiceImpl::getChatServer()
{
    std::lock_guard<std::mutex> guard(_server_mtx);
    auto minServer = _servers.begin()->second;
    //找到连接数量最小的chatserver
    for (const auto& server : _servers) {
        if (server.second.con_count < minServer.con_count) {
            minServer = server.second;
        }
    }
    return minServer;
}
StatusServiceImpl::StatusServiceImpl()
{
    auto& cfg = ConfigMgr::Inst();
    ChatServer server;
    server.port = cfg["ChatServer1"]["Port"];
    server.host = cfg["ChatServer1"]["Host"];
    server.con_count = 0;
    server.name = cfg["ChatServer1"]["Name"];
    _servers[server.name] = server;
    server.port = cfg["ChatServer2"]["Port"];
    server.host = cfg["ChatServer2"]["Host"];
    server.con_count = 0;
    server.name = cfg["ChatServer2"]["Name"];
    _servers[server.name] = server;
}