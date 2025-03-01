#include "userdata.h"

SearchInfo::SearchInfo(int uid, QString name, QString nick, QString desc, int sex):
    _uid(uid),_name(name), _nick(nick), _desc(desc), _sex(sex){

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
