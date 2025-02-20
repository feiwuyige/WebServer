#ifndef GLOBAL_H
#define GLOBAL_H
#include <QWidget>
#include <functional>
#include <QStyle>
#include <QRegularExpression>
#include <mutex>
#include <memory>
#include <iostream>
#include <QByteArray>
#include <QNetworkReply>
#include <QJsonObject>
#include <QDir>
#include <QSettings>
/**
 * @brief 用来刷新qss
 * @author zyl
 * @date 2025/01/19
 */
extern std::function<void(QWidget*)> repolish;
/**
 * @brief 异或字符串实现加密
 *
 * @author zyl
 * @date 2025/02/10
 */
extern std::function<QString(QString)> xorString;
/**
 * @brief HttpManager类中的请求类型
 *
 * @author zyl
 * @date 2025/01/19
 */
enum ReqId{
    ID_GET_VARIFY_CODE = 1001, //获取验证码
    ID_REG_USER = 1002, //注册用户
    ID_RESET_PWD = 1003, //重置密码
    ID_LOGIN_USER = 1004, //用户登录
    ID_CHAT_LOGIN = 1005, //登录聊天服务器
    ID_CHAT_LOGIN_RSP = 1006, //登录聊天服务器回包
};
/**
 * @brief HttpManager类中的模块类型
 *
 * @author zyl
 * @date 2025/01/19
 */
enum Modules{
    REGISTERMOD = 0, //注册模块
    RESETMOD = 1,
    LOGINMOD = 2,
};
/**
 * @brief 错误模块
 *
 * @author zyl
 * @date 2025/01/19
 */
enum ErrorCodes{
    SUCCESS = 0, //成功
    ERR_JSON = 1, //JSON解析失败
    ERR_NETWORK = 2, //网络错误
};

enum TipErr{
    TIP_SUCCESS = 0,
    TIP_EMAIL_ERR = 1,
    TIP_PWD_ERR = 2,
    TIP_CONFIRM_ERR = 3,
    TIP_PWD_CONFIRM = 4,
    TIP_VARIFY_ERR = 5,
    TIP_USER_ERR = 6
};
enum ClickLbState{
    Normal = 0,
    Select = 1
};
struct ServerInfo{
    QString Host;
    QString Port;
    QString Token;
    int Uid;
};
enum ChatUIMode{
    SearchMode, //搜索模式
    ChatMode,  //聊天模式
    ContactMode, //联系模式
};
enum ListItemType{
    CHAT_USER_ITEM,
    CONTACT_USER_ITEM,
    SEARCH_USER_ITEM,
    ADD_USER_TIP_ITEM,
    INVALID_ITEM,
    GROUP_TIP_ITEM,
    APPLY_FRIEND_ITEM,
    LINE_ITEM
};
enum class ChatRole{
    Self,
    Other,
};
struct MsgInfo{
    QString msgFlag;
    QString content;
    QPixmap pixmap;
};

const int MIN_APPLY_LABEL_ED_LEN = 40;
const QString add_prefix = "添加标签: ";
const int tip_offset = 5;

extern QString gate_url_prefix;
#endif // GLOBAL_H
