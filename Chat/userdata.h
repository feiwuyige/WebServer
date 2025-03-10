#ifndef USERDATA_H
#define USERDATA_H
#include <QString>
#include <vector>
#include <memory>
#include <QJsonArray>

class SearchInfo{
public:
    SearchInfo(int uid, QString name, QString nick, QString desc, QString icon, int sex);
    int _uid;
    QString _name;
    QString _nick;
    QString _desc;
    QString _icon;
    int _sex;
};

class AddFriendApply{
public:
    AddFriendApply(int from_uid, QString name, QString desc, QString icon, QString nick, int sex);
    int _from_uid;
    QString _name;
    QString _desc;
    QString _icon;
    QString _nick;
    int _sex;
};

class ApplyInfo{
public:
    ApplyInfo(int uid, QString name, QString desc, QString icon, QString nick, int sex, int status);
    ApplyInfo(std::shared_ptr<AddFriendApply> addinfo);
    void SetIcon(QString head);
    int _uid;
    QString _name;
    QString _desc;
    QString _icon;
    QString _nick;
    int _sex;
    int _status;
};

struct AuthInfo{
    AuthInfo(int uid, QString name, QString nick, QString icon, int sex);
    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;
};

struct AuthRsp{
    AuthRsp(int peer_uid, QString peer_name, QString peer_nick, QString peer_icon, int peer_sex);
    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;
};
struct TextChatData;
struct FriendInfo{
    FriendInfo(int uid, QString name, QString nick, QString icon, int sex, QString desc, QString back, QString last_msg = "");
    FriendInfo(std::shared_ptr<AuthInfo> auth_info);
    FriendInfo(std::shared_ptr<AuthRsp> auth_rsp);
    void AppendChatMsgs(const std::vector<std::shared_ptr<TextChatData>> text_vec);
    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;
    QString _desc;
    QString _back;
    QString _last_msg;
    std::vector<std::shared_ptr<TextChatData> > _chat_msgs;
};

struct UserInfo{
    UserInfo(int uid, QString name, QString nick, QString icon, int sex, QString last_msg = "");
    UserInfo(std::shared_ptr<AuthInfo> auth);
    UserInfo(int uid, QString name, QString icon);
    UserInfo(std::shared_ptr<AuthRsp> auth);
    UserInfo(std::shared_ptr<SearchInfo> si);
    UserInfo(std::shared_ptr<FriendInfo> fi);
    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;
    QString _last_msg;
    std::vector<std::shared_ptr<TextChatData> > _chat_msgs;

};
struct TextChatData{
    TextChatData(QString msg_id, QString msg_content, int from_uid, int to_uid);
    QString _msg_id;
    QString _msg_content;
    int _from_uid;
    int _to_uid;

};
struct TextChatMsg{
    TextChatMsg(int fromuid, int touid, QJsonArray array);
    int _from_uid;
    int _to_uid;
    std::vector<std::shared_ptr<TextChatData>> _chat_msgs;
};

#endif // USERDATA_H
