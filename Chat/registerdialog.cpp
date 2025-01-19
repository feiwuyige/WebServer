#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "global.h"
#include "httpmanager.h"


RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    ui->PassEdit->setEchoMode(QLineEdit::Password);
    ui->YesEdit->setEchoMode(QLineEdit::Password);
    ui->ErrorLabel->setProperty("state", "normal");
    repolish(ui->ErrorLabel);
    connect(HttpManager::GetInstance().get(), &HttpManager::sig_reg_mod_finish, this,
            &RegisterDialog::slot_reg_mod_finish);
    initHttpHandlers();

}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::on_TestBtn_clicked()
{
    //判断邮箱是否符合条件
    auto email = ui->EmailEdit->text();
    //qDebug() << email;
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch();
    //qDebug() << match;
    if(match){
        //发送验证码

    }
    else{
        showTip(tr("邮箱地址不正确！！！"), false);
    }
}

void RegisterDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err)
{
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
    //转化为json对象
    jsonDoc.object();
    //根据不同的情况处理json对象
    _handlers[id](jsonDoc.object());
    return;
}

void RegisterDialog::showTip(QString str, bool ok)
{
    if(ok){
        ui->ErrorLabel->setProperty("state", "normal");
    }
    else{
        ui->ErrorLabel->setText(str);
    }
    ui->ErrorLabel->setProperty("state", "err");
    repolish(ui->ErrorLabel);
}

void RegisterDialog::initHttpHandlers()
{
    //注册获取验证码回报的逻辑
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](const QJsonObject& jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"), false);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("验证码已经发送至邮箱"), true);
        qDebug() << "email is " << email;
    });
}



