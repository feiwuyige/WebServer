#include "chatpage.h"
#include "ui_chatpage.h"
#include <QStyleOption>
#include "chatitembase.h"
#include <QPainter>
#include "messagetextedit.h"
#include "textbubble.h"
#include "picturebubble.h"
#include "usermgr.h"
#include <QUuid>
#include <QJsonDocument>
#include "tcpmgr.h"

ChatPage::ChatPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatPage)
{
    ui->setupUi(this);
    //设置按钮样式
    ui->receive_btn_2->SetState("normal","hover","press");
    ui->send_btn_2->SetState("normal","hover","press");
    //设置图标样式
    ui->emo_lb_2->SetState("normal","hover","press","normal","hover","press");
    ui->file_lb_2->SetState("normal","hover","press","normal","hover","press");
}

ChatPage::~ChatPage()
{
    delete ui;
}

void ChatPage::SetUserInfo(std::shared_ptr<UserInfo> user_info)
{
    _user_info = user_info;
    //设置ui
    ui->title_lb_2->setText(_user_info->_name);
    ui->chat_data_list->removeAllItem();
    for(auto &msg : user_info->_chat_msgs){
        AppendChatMsg(msg);
    }
}

void ChatPage::AppendChatMsg(std::shared_ptr<TextChatData> msg)
{
    auto self_info = UserMgr::GetInstance()->GetUserInfo();
    ChatRole role;

    if(msg->_from_uid == self_info->_uid){
        role = ChatRole::Self;
        ChatItemBase* pChatItem = new ChatItemBase(role);
        pChatItem->setUserName(self_info->_name);
        pChatItem->setUserIcon(QPixmap(self_info->_icon));
        QWidget* pBubble = nullptr;
        pBubble = new TextBubble(role, msg->_msg_content);
        pChatItem->setWidget(pBubble);
        ui->chat_data_list->appendChatItem(pChatItem);
    }
    else{
        role = ChatRole::Other;
        ChatItemBase* pChatItem = new ChatItemBase(role);
        auto friend_info = UserMgr::GetInstance()->GetFriendById(msg->_from_uid);
        if (friend_info == nullptr) {
            return;
        }
        pChatItem->setUserName(friend_info->_name);
        pChatItem->setUserIcon(QPixmap(friend_info->_icon));
        QWidget* pBubble = nullptr;
        pBubble = new TextBubble(role, msg->_msg_content);
        pChatItem->setWidget(pBubble);
        ui->chat_data_list->appendChatItem(pChatItem);
    }
}



void ChatPage::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ChatPage::on_send_btn_2_clicked()
{
    if(_user_info == nullptr){
        return;
    }
    auto user_info = UserMgr::GetInstance()->GetUserInfo();
    auto pTextEdit = ui->chatEdit;
    ChatRole role = ChatRole::Self;
    QString userName = user_info->_name;
    QString userIcon = user_info->_icon;
    const QVector<MsgInfo>& msgList = pTextEdit->getMsgList();
    QJsonObject textObj;
    QJsonArray textArray;
    int txt_size = 0;

    for(int i=0; i<msgList.size(); ++i)
    {
        //消息内容过长跳过
        if(msgList[i].content.length() > 1024){
            continue;
        }
        QString type = msgList[i].msgFlag;
        ChatItemBase *pChatItem = new ChatItemBase(role);
        pChatItem->setUserName(userName);
        pChatItem->setUserIcon(QPixmap(userIcon));
        QWidget *pBubble = nullptr;
        if(type == "text")
        {
            pBubble = new TextBubble(role, msgList[i].content);
            QUuid uuid = QUuid::createUuid();
            QString uuidString = uuid.toString();
            if(txt_size + msgList[i].content.length() > 1024){
                textObj["fromuid"] = user_info->_uid;
                textObj["touid"] = _user_info->_uid;
                textObj["text_array"] = textArray;
                QJsonDocument doc(textObj);
                QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
                txt_size = 0;
                textArray = QJsonArray();
                textObj = QJsonObject();
                emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_TEXT_CHAT_MSG_REQ, jsonData);
            }
            txt_size += msgList[i].content.length();
            QJsonObject obj;
            QByteArray utf8Message = msgList[i].content.toUtf8();
            obj["content"] = QString::fromUtf8(utf8Message);
            obj["msgid"] = uuidString;
            textArray.append(obj);
            auto txt_msg = std::make_shared<TextChatData> (uuidString, obj["content"].toString(), user_info->_uid,
                                                          _user_info->_uid);
            emit sig_append_send_chat_msg(txt_msg);
        }
        else if(type == "image")
        {
            pBubble = new PictureBubble(QPixmap(msgList[i].content) , role);
        }
        else if(type == "file")
        {
        }
        if(pBubble != nullptr)
        {
            pChatItem->setWidget(pBubble);
            ui->chat_data_list->appendChatItem(pChatItem);
        }
        //发送给服务器
        textObj["fromuid"] = user_info->_uid;
        textObj["touid"] = _user_info->_uid;
        textObj["text_array"] = textArray;
        QJsonDocument doc(textObj);
        QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
        txt_size = 0;
        textArray = QJsonArray();
        textObj = QJsonObject();
        emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_TEXT_CHAT_MSG_REQ, jsonData);
    }
}

