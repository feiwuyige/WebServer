#include "ChatServiceImpl.h"
#include "UserMgr.h"
#include "CSession.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "RedisMgr.h"
#include "MysqlMgr.h"

ChatServiceImpl::ChatServiceImpl()
{
}

Status ChatServiceImpl::NotifyAddFriend(ServerContext* context, const AddFriendReq* request, AddFriendRsp* reply) 
{
	//查找用户是否在线
	auto touid = request->touid();
	auto session = UserMgr::GetInstance()->GetSession(touid);

	Defer defer([request, reply]() {
		reply->set_error(ErrorCodes::Success);
		reply->set_applyuid(request->applyuid());
		reply->set_touid(request->touid());
		});

	if (session == nullptr) {
		return Status::OK;
	}
	//在内存中通知对方
	Json::Value rtvalue;
	rtvalue["error"] = ErrorCodes::Success;
	rtvalue["applyuid"] = request->applyuid();
	rtvalue["name"] = request->name();
	rtvalue["desc"] = request->desc();
	rtvalue["icon"] = request->icon();
	rtvalue["sex"] = request->sex();
	rtvalue["nick"] = request->nick();

	std::string return_str = rtvalue.toStyledString();
	session->Send(return_str, ID_NOTIFY_ADD_FRIEND_REQ);
	return Status::OK;
}

Status ChatServiceImpl::NotifyAuthFriend(ServerContext* context, const AuthFriendReq* request, AuthFriendRsp* response)
{
	return Status();
}

Status ChatServiceImpl::NotifyTextChatMsg(::grpc::ServerContext* context, const TextChatMsgReq* request, TextChatMsgRsp* response)
{
	return Status();
}

bool ChatServiceImpl::GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo)
{
	return false;
}
