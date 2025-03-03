#include "usermgr.h"




UserMgr::~UserMgr()
{

}

void UserMgr::SetName(QString name)
{
    _user_info->_name = name;
}

void UserMgr::SetUid(int uid)
{
    _user_info->_uid = uid;
}

void UserMgr::SetToken(QString token)
{
    _token = token;
}

QString UserMgr::GetName()
{
    return _user_info->_name;
}

int UserMgr::GetUid()
{
    return _user_info->_uid;
}

std::vector<std::shared_ptr<ApplyInfo> > UserMgr::GetApplyList()
{
    return _apply_list;
}

bool UserMgr::AlreadyApply(int uid)
{
    for(auto& apply : _apply_list){
        if(apply->_uid == uid){
            return true;
        }
    }
    return false;
}

void UserMgr::AddApplyList(std::shared_ptr<ApplyInfo> app)
{
    _apply_list.push_back(app);
}

void UserMgr::SetUserInfo(std::shared_ptr<UserInfo> user_info)
{
    _user_info = user_info;
}

void UserMgr::AppendApplyList(QJsonArray array)
{
    for(const QJsonValue &value : array){
        auto name = value["name"].toString();
        auto desc = value["desc"].toString();
        auto icon = value["icon"].toString();
        auto nick = value["nick"].toString();
        auto sex = value["sex"].toInt();
        auto uid = value["uid"].toInt();
        auto status = value["status"].toInt();
        auto info = std::make_shared<ApplyInfo>(uid, name, desc, icon, nick, sex, status);
        _apply_list.push_back(info);
    }
}

void UserMgr::AppendFriendList(QJsonArray array)
{
    for(const QJsonValue &value : array){
        auto name = value["name"].toString();
        auto desc = value["desc"].toString();
        auto icon = value["icon"].toString();
        auto nick = value["nick"].toString();
        auto sex = value["sex"].toInt();
        auto uid = value["uid"].toInt();
        auto back = value["back"].toString();
        QString last_msg = "";
        auto info = std::make_shared<FriendInfo>(uid, name, nick, icon, sex, desc, back, last_msg);
        _friend_list.push_back(info);
        _friend_map.insert(uid, info);
    }
}

bool UserMgr::CheckFriendById(int uid)
{
    auto iter = _friend_map.find(uid);
    if(iter == _friend_map.end()){
        return false;
    }
    return true;
}

void UserMgr::AddFriend(std::shared_ptr<AuthRsp> auth_rsp)
{
    auto friend_info = std::make_shared<FriendInfo>(auth_rsp);
    _friend_map[friend_info->_uid] = friend_info;
}

void UserMgr::AddFriend(std::shared_ptr<AuthInfo> auth_info)
{
    auto friend_info = std::make_shared<FriendInfo>(auth_info);
    _friend_map[friend_info->_uid] = friend_info;
}

std::shared_ptr<FriendInfo> UserMgr::GetFriendById(int uid)
{
    auto iter = _friend_map.find(uid);
    if(iter == _friend_map.end()){
        return nullptr;
    }
    return (*iter);
}

std::vector<std::shared_ptr<FriendInfo> > UserMgr::GetChatListPerPage()
{
    std::vector<std::shared_ptr<FriendInfo>> friend_list;
    size_t begin = _chat_loaded;
    size_t end = begin + CHAT_COUNT_PER_PAGE;
    if(begin >= _friend_list.size()){
        return friend_list;
    }
    if(end > _friend_list.size()){
        friend_list = std::vector<std::shared_ptr<FriendInfo>>(_friend_list.begin()+begin, _friend_list.end());
        return friend_list;
    }
    friend_list = std::vector<std::shared_ptr<FriendInfo>>(_friend_list.begin()+begin, _friend_list.begin()+end);
    return friend_list;
}

bool UserMgr::IsLoadChatFin()
{
    if(_chat_loaded >= _friend_list.size()){
        return true;
    }
    return false;
}

void UserMgr::UpdateChatLoadedCount()
{
    size_t begin = _chat_loaded;
    size_t end = begin + CHAT_COUNT_PER_PAGE;
    if(begin >= _friend_list.size()){
        return;
    }
    if(end > _friend_list.size()){
        _chat_loaded = _friend_list.size();
        return;
    }
    _chat_loaded = end;
    return;
}

std::vector<std::shared_ptr<FriendInfo> > UserMgr::GetConListPrePage()
{
    std::vector<std::shared_ptr<FriendInfo>> friend_list;
    size_t begin = _con_loaded;
    size_t end = begin + CHAT_COUNT_PER_PAGE;
    if(begin >= _friend_list.size()){
        return friend_list;
    }
    if(end > _friend_list.size()){
        friend_list = std::vector<std::shared_ptr<FriendInfo>>(_friend_list.begin()+begin, _friend_list.end());
        return friend_list;
    }
    friend_list = std::vector<std::shared_ptr<FriendInfo>>(_friend_list.begin()+begin, _friend_list.begin()+end);
    return friend_list;
}

void UserMgr::UpdateContactLoadedCount()
{
    size_t begin = _con_loaded;
    size_t end = begin + CHAT_COUNT_PER_PAGE;
    if(begin >= _friend_list.size()){
        return;
    }
    if(end > _friend_list.size()){
        _con_loaded = _friend_list.size();
        return;
    }
    _con_loaded = end;
    return;
}

bool UserMgr::IsLoadConFin()
{
    if(_con_loaded >= _friend_list.size()){
        return true;
    }
    return false;
}

std::shared_ptr<UserInfo> UserMgr::GetUserInfo()
{
    return _user_info;
}

void UserMgr::AppendFriendChatMsg(int friend_id, std::vector<std::shared_ptr<TextChatData> > msgs)
{
    auto find_iter = _friend_map.find(friend_id);
    if(find_iter == _friend_map.end()){
        return;
    }
    find_iter.value()->AppendChatMsgs(msgs);
}


UserMgr::UserMgr() : _user_info(nullptr), _chat_loaded(0), _con_loaded(0)
{

}
