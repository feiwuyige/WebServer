#include "logindialog.h"
#include "ui_logindialog.h"
#include <QPainter>
#include <QPainterPath>
#include "httpmanager.h"

void LoginDialog::slot_forget_pwd()
{
    emit switchReset();
}

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    initHead();
    ui->err_tip->setProperty("state", "normal");
    repolish(ui->err_tip);
    ui->err_tip->clear();
    ui->PassEdit->setEchoMode(QLineEdit::Password);
    ui->pass_visible->setCursor(Qt::PointingHandCursor);
    ui->pass_visible->SetState("unvisible", "unvisible_hover", "", "visible", "visible_hover", "");
    connect(ui->pass_visible, &ClickedLabel::clicked, this, [this]() {
        auto state = ui->pass_visible->GetCurState();
        if(state == ClickLbState::Normal){
            ui->PassEdit->setEchoMode(QLineEdit::Password);
        }else{
            ui->PassEdit->setEchoMode(QLineEdit::Normal);
        }
        //qDebug() << "Label was clicked!";
    });

    initHttpHandlers();
    //将按钮事件与信号连接起来
    connect(ui->RegisterBtn, &QPushButton::clicked, this, &LoginDialog::switchRegister);

    ui->ForgetLabel->SetState("normal", "hover", "", "selected", "selected_hover", "");

    connect(ui->ForgetLabel, &ClickedLabel::clicked, this, &LoginDialog::slot_forget_pwd);

    connect(HttpManager::GetInstance().get(), &HttpManager::sig_login_mod_finish, this, &LoginDialog::slot_login_mod_finish);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::initHead()
{
    //加载图片
    QPixmap originalPixmap(":/img/login.jpg");
    //设置图片自动缩放
    originalPixmap = originalPixmap.scaled(ui->head_label->size(),Qt::KeepAspectRatio, Qt::SmoothTransformation);
    //绘制圆角图片
    QPixmap roundedPixmap(originalPixmap.size());
    roundedPixmap.fill(Qt::transparent); //透明色填充
    QPainter painter(&roundedPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint((QPainter::SmoothPixmapTransform));
    //设置圆角
    QPainterPath path;
    path.addRoundedRect(0, 0, originalPixmap.width(), originalPixmap.height(), 10, 10);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, originalPixmap);
    ui->head_label->setPixmap(roundedPixmap);
}

bool LoginDialog::checkUsrValid()
{
    auto email = ui->EmailEdit->text();
    if(email.isEmpty()){
        AddTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱不能为空"));
        return false;
    }
    DelTipErr(TipErr::TIP_EMAIL_ERR);
    return true;
}

bool LoginDialog::checkPwdValid()
{
    //to do
    return true;
}

void LoginDialog::enableBtn(bool ok)
{
    ui->LoginBtn->setEnabled(ok);
    ui->RegisterBtn->setEnabled(ok);
    return;
}

void LoginDialog::on_LoginBtn_clicked()
{
    if(checkUsrValid() == false){
        return;
    }
    if(checkPwdValid() == false){
        return;
    }
    enableBtn(false);
    auto email = ui->EmailEdit->text();
    auto pwd = ui->PassEdit->text();
    //发送http登录请求
    QJsonObject json_obj;
    json_obj["email"] = email;
    json_obj["passwd"] = xorString(pwd);
    HttpManager::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/user_login"),
                                            json_obj, ReqId::ID_LOGIN_USER, Modules::LOGINMOD);
}

void LoginDialog::slot_login_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    enableBtn(true);
    if(err != ErrorCodes::SUCCESS){
        showTip(tr("网络请求错误"), false);
        return;
    }
    //解析json字符串，res转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8()); //一个json文件
    if(jsonDoc.isNull()){
        showTip(tr("Json解析失败"), false);
        return;
    }
    //json解析错误
    else if(!jsonDoc.isObject()){
        showTip(tr("Json解析失败"), false);
        return;
    }
    qDebug() << id << " hahah";
    //转化为json对象
    jsonDoc.object();
    //根据不同的情况处理json对象
    _handlers[id](jsonDoc.object());
    return;
}

void LoginDialog::AddTipErr(TipErr te, QString tips)
{
    _tip_errs[te] = tips;
    showTip(tips, false);
}

void LoginDialog::DelTipErr(TipErr te)
{
    _tip_errs.remove(te);
    if(_tip_errs.empty()){
        ui->err_tip->clear();
        return;
    }
    showTip(_tip_errs.first(), false);
}

void LoginDialog::initHttpHandlers()
{
    _handlers.insert(ReqId::ID_LOGIN_USER, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"), false);
            enableBtn(true);
            return;
        }
        auto email = jsonObj["email"].toString();
        //发送信号通知tcpMgr发送长连接
        ServerInfo si; //存放服务器返回的信息
        si.Uid = jsonObj["uid"].toInt();
        si.Host = jsonObj["host"].toString();
        si.Port = jsonObj["port"].toString();
        si.Token = jsonObj["token"].toString();

        //缓存服务器分配的uid和token
        _uid = si.Uid;
        _token = si.Token;
        //发送信号
        qDebug() << "email is " << email << "uid is " << si.Uid << "host is "
                 << si.Host << " Port is " << si.Port << " Token is " << si.Token;
        emit sig_connect_tcp(si);
    });
}
void LoginDialog::showTip(QString str, bool ok)
{
    if(ok){
        ui->err_tip->setProperty("state", "normal");
    }
    else{
        ui->err_tip->setProperty("state", "err");
    }
    ui->err_tip->setText(str);

    repolish(ui->err_tip);
}
