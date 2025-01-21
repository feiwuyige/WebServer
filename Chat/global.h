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
 * @brief HttpManager类中的请求类型
 *
 * @author zyl
 * @date 2025/01/19
 */
enum ReqId{
    ID_GET_VARIFY_CODE = 1001, //获取验证码
    ID_REG_USER = 1002, //注册用户
};
/**
 * @brief HttpManager类中的模块类型
 *
 * @author zyl
 * @date 2025/01/19
 */
enum Modules{
    REGISTERMOD = 0, //注册模块
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

extern QString gate_url_prefix;
#endif // GLOBAL_H
