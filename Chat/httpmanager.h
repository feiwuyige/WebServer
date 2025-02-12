#ifndef HTTPMANAGER_H
#define HTTPMANAGER_H
/**
 * @file  httpmanager.h
 * @brief http管理者类
 *
 * @author zyl
 * @date 2025/01/19
 *
 * 是一个单例类
 */
#include "singleton.h"
#include <QString>
#include <QUrl>
#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>

class HttpManager : public QObject, public Singleton<HttpManager>, public std::enable_shared_from_this<HttpManager>
{
    Q_OBJECT
private:
    friend class Singleton<HttpManager>; //声明友元类，从而可以使得友元类可以调用构造函数
    HttpManager();
    QNetworkAccessManager _manager;

public:
    //后面两个参数是因为采用异步发送的方式，回调函数执行的时候需要知道是哪一个模块的哪一个请求
    void PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod);
    ~HttpManager();
private slots:
    void slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);
signals:
    //发送的信号中的参数最后会传送给槽函数
    void sig_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);
    void sig_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
    void sig_reset_mod_finish(ReqId id, QString res, ErrorCodes err);
    void sig_login_mod_finish(ReqId id, QString res, ErrorCodes err);
};

#endif // HTTPMANAGER_H
