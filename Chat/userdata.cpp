#include "userdata.h"
#include <QJsonObject>

SearchInfo::SearchInfo(int uid, QString name, QString nick, QString desc, QString icon, int sex):
    _uid(uid),_name(name), _nick(nick), _desc(desc),_icon(icon), _sex(sex){

}

AddFriendApply::AddFriendApply(int from_uid, QString name, QString desc, QString icon, QString nick, int sex)
    : _from_uid(from_uid), _name(name), _desc(desc), _icon(icon), _nick(nick), _sex(sex)
{

}

ApplyInfo::ApplyInfo(int uid, QString name, QString desc, QString icon, QString nick, int sex, int status)
    : _uid(uid), _name(name), _desc(desc), _icon(icon), _nick(nick), _sex(sex), _status(status)
{

}

ApplyInfo::ApplyInfo(std::shared_ptr<AddFriendApply> addinfo)
    : _uid(addinfo->_from_uid), _name(addinfo->_name), _desc(addinfo->_desc),
    _icon(addinfo->_icon), _nick(addinfo->_nick), _sex(addinfo->_sex),_status(0)
{

}

void ApplyInfo::SetIcon(QString head)
{
    _icon = head;
}

AuthInfo::AuthInfo(int uid, QString name, QString nick, QString icon, int sex):
    _uid(uid), _name(name), _nick(nick), _icon(icon), _sex(sex)
{

}

AuthRsp::AuthRsp(int peer_uid, QString peer_name, QString peer_nick, QString peer_icon, int peer_sex) :
    _uid(peer_uid), _name(peer_name), _nick(peer_nick), _icon(peer_icon), _sex(peer_sex)
{

}

UserInfo::UserInfo(int uid, QString name, QString nick, QString icon, int sex,  QString last_msg):
    _uid(uid), _name(name), _nick(nick), _icon(icon), _sex(sex), _last_msg(last_msg)
{

}

UserInfo::UserInfo(std::shared_ptr<AuthInfo> auth):
    _uid(auth->_uid), _name(auth->_name), _nick(auth->_nick),
    _icon(auth->_icon), _sex(auth->_sex), _last_msg("")
{

}

UserInfo::UserInfo(int uid, QString name, QString icon) :
    _uid(uid), _name(name), _nick(name),_icon(icon), _sex(0), _last_msg("")
{

}

UserInfo::UserInfo(std::shared_ptr<AuthRsp> auth):
    _uid(auth->_uid), _name(auth->_name), _nick(auth->_nick),
    _icon(auth->_icon), _sex(auth->_sex),_last_msg("")
{

}

UserInfo::UserInfo(std::shared_ptr<SearchInfo> si) :
    _uid(si->_uid), _name(si->_name), _nick(si->_nick),
    _icon(si->_icon), _sex(si->_sex), _last_msg("")
{

}

UserInfo::UserInfo(std::shared_ptr<FriendInfo> fi):
    _uid(fi->_uid), _name(fi->_name), _nick(fi->_nick),
    _icon(fi->_icon), _sex(fi->_sex), _last_msg(fi->_last_msg)
{
    _chat_msgs = fi->_chat_msgs;
}

FriendInfo::FriendInfo(int uid, QString name, QString nick, QString icon, int sex, QString desc, QString back, QString last_msg)
    : _uid(uid), _name(name), _nick(nick), _icon(icon), _sex(sex), _desc(desc), _back(back), _last_msg(last_msg)
{

}

FriendInfo::FriendInfo(std::shared_ptr<AuthInfo> auth_info)
    : _uid(auth_info->_uid), _name(auth_info->_name), _nick(auth_info->_nick), _icon(auth_info->_icon), _sex(auth_info->_sex)
{

}

FriendInfo::FriendInfo(std::shared_ptr<AuthRsp> auth_rsp)
    : _uid(auth_rsp->_uid), _name(auth_rsp->_name), _nick(auth_rsp->_nick), _icon(auth_rsp->_icon), _sex(auth_rsp->_sex)
{

}

void FriendInfo::AppendChatMsgs(const std::vector<std::shared_ptr<TextChatData> > text_vec)
{
    for(const auto & text : text_vec){
        _chat_msgs.push_back(text);
    }
}

TextChatData::TextChatData(QString msg_id, QString msg_content, int from_uid, int to_uid):
    _msg_id(msg_id), _msg_content(msg_content), _from_uid(from_uid), _to_uid(to_uid)
{
}

TextChatMsg::TextChatMsg(int fromuid, int touid, QJsonArray array) :
    _from_uid(fromuid), _to_uid(touid)
{
    for(auto msg_data : array){
        auto msg_obj = msg_data.toObject();
        auto content = msg_obj["content"].toString();
        auto msgid = msg_obj["msgid"].toString();
        auto msg_ptr = std::make_shared<TextChatData>(msgid, content, fromuid, touid);
        _chat_msgs.push_back(msg_ptr);

    }
}
