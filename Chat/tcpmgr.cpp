#include "tcpmgr.h"
#include <QAbstractSocket>
#include <QDebug>
#include <QJsonDocument>
#include "usermgr.h"
TcpMgr::~TcpMgr()
{

}

TcpMgr::TcpMgr() : _host(""), _port(0), _b_recv_pending(false), _message_id(0), _message_len(0)
{
    QObject::connect(&_socket, &QTcpSocket::connected,[&](){
        qDebug() << "connect to server!";
        emit sig_con_success(true);
    });

    //当有数据可以读的时候，发送这个信号
    QObject::connect(&_socket, &QTcpSocket::readyRead,[&](){
       //读取数据
        _buffer.append(_socket.readAll());
        QDataStream stream(&_buffer, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_5_0);
        //#define Q_FOREVER for(;;)
        forever{
            //解析头部，要处理粘包问题，所以每一个消息体头部有两个自己的 id 和两个自己的 length
            //通过这两个字段来判断是否完整收到消息
            if(!_b_recv_pending){
                if(_buffer.size() < static_cast<int> (sizeof(quint16) * 2)){
                    return; //连消息头都没有接受完毕，数据不够，直接返回
                }
                //预读取消息id和长度，但是不从缓冲区移除
                stream >> _message_id >> _message_len;
                //移除
                _buffer = _buffer.mid(sizeof(quint16) * 2);
                //输出
                qDebug() << "Message id " << _message_id << " , Message Length " << _message_len;
            }
            if(_buffer.size() < _message_len){
                _b_recv_pending = true;
                return; //没收全
            }
            _b_recv_pending = false;
            QByteArray messageBody = _buffer.mid(0, _message_len);
            qDebug() << "receive body msg is " << messageBody;
            _buffer = _buffer.mid(_message_len);
            handleMsg(ReqId(_message_id), _message_len, messageBody);
        }
    });

    //当出现网络波动等问题时，tcp会将错误抛给上层客户端
    //此处处理错误
    QObject::connect(&_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
                     [&](QAbstractSocket::SocketError socketError){
            Q_UNUSED(socketError)
            qDebug() << "Error:" << _socket.errorString();
    });

    //连接断开
    QObject::connect(&_socket, &QTcpSocket::disconnected,[&](){
        qDebug() << "Disconnected from server.";
    });
    //连接发送信号用来发送数据
    QObject::connect(this, &TcpMgr::sig_send_data, this, &TcpMgr::slot_send_data);
    //注册消息
    initHandlers();
}

void TcpMgr::initHandlers()
{
    _handlers.insert(ID_CHAT_LOGIN_RSP, [this](ReqId id, int len, QByteArray data){
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;
        // 将QByteArrat 转换为 QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        //检查转换是否成功
        if(jsonDoc.isNull()){
            qDebug() << "Failed to create QJsonDocument";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        if(!jsonObj.contains("error")){
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Login Failed, err is Json Parse Err" << err;
            emit sig_login_failed(err);
            return;
        }
        int err = jsonObj["error"].toInt();
        if(err != ErrorCodes::SUCCESS){
            qDebug() << "Login Failed, err is " << err;
            emit sig_login_failed(err);
            return;
        }
        UserMgr::GetInstance()->SetUid(jsonObj["uid"].toInt());
        UserMgr::GetInstance()->SetName(jsonObj["name"].toString());
        UserMgr::GetInstance()->SetToken(jsonObj["token"].toString());
        emit sig_swich_chatdlg();
    });
}

void TcpMgr::handleMsg(ReqId id, int len, QByteArray data)
{
    auto find_iter = _handlers.find(id);
    if(find_iter == _handlers.end()){
        qDebug() << "Not found id [" << id << "] to handle";
        return;
    }
    find_iter.value()(id, len, data);
}

void TcpMgr::slot_tcp_connect(ServerInfo si)
{
    qDebug() << "receive tcp connect signal";
    qDebug() << "connecting to server...";
    _host = si.Host;
    _port = static_cast<uint16_t>(si.Port.toUInt());
    _socket.connectToHost(si.Host, _port); //异步连接，此处不会阻塞
}
//发送完数据后调用的槽函数
void TcpMgr::slot_send_data(ReqId reqId, QString data)
{
    uint16_t id = reqId;
    QByteArray dataBytes = data.toUtf8();
    quint16 len = static_cast<quint16>(data.size());
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);
    out << id << len;
    block.append(dataBytes);
    _socket.write(block);
}
