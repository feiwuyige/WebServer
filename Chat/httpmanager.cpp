#include "httpmanager.h"



HttpManager::HttpManager()
{
    connect(this, &HttpManager::sig_http_finish, &HttpManager::slot_http_finish);
}

void HttpManager::PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod)
{
    QByteArray data = QJsonDocument(json).toJson();
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));
    //伪闭包
    auto self = shared_from_this();
    QNetworkReply* reply = _manager.post(request, data);
    //要保证在触发回调之前httpManager对象不会被删除，所以要生成一个self智能指针
    QObject::connect(reply, &QNetworkReply::finished, [self, reply, req_id, mod](){
        //处理错误情况
        if(reply->error() != QNetworkReply::NoError){
            qDebug() << reply->errorString();
            //发送信号通知完成
            emit self->sig_http_finish(req_id, "", ErrorCodes::ERR_NETWORK, mod);
            reply->deleteLater(); //qt负责回收
            return;
        }
        //无错误
        QString res = reply->readAll();
        emit self->sig_http_finish(req_id, res, ErrorCodes::SUCCESS, mod);
        reply->deleteLater(); //qt负责回收
        return;
    });
}

HttpManager::~HttpManager()
{

}

void HttpManager::slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod)
{
    if(mod == Modules::REGISTERMOD){
        //发送信号通知指定模块http响应结束
        emit sig_reg_mod_finish(id, res, err);
    }
}
