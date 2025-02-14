#ifndef TCPMGR_H
#define TCPMGR_H
#include <QTcpSocket>
#include "global.h"
#include "singleton.h"
#include <functional>
#include <QObject>

class TcpMgr : public QObject, public Singleton<TcpMgr>,
               public std::enable_shared_from_this<TcpMgr>
{
    Q_OBJECT
public:
    ~TcpMgr();
private:
    friend class Singleton<TcpMgr>;
    TcpMgr();
    void initHandlers();
    void handleMsg(ReqId id, int len, QByteArray data);
    QTcpSocket _socket;
    QString _host;
    uint16_t _port;
    QByteArray _buffer;
    bool _b_recv_pending; //tcp是面向字节流的，所以使用该变量来判断有没有收完
    quint16 _message_id;
    quint16 _message_len;
    QMap<ReqId, std::function<void(ReqId id, int len, QByteArray data) >> _handlers;
public slots:
    void slot_tcp_connect(ServerInfo);
    void slot_send_data(ReqId reqId, QString data);
signals:
    void sig_con_success(bool bsucess);
    void sig_send_data(ReqId reqId, QString data);
    void sig_swich_chatdlg();
    void sig_login_failed(int);
};

#endif // TCPMGR_H
