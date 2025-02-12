#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "global.h"
#include "httpmanager.h"


RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog), _countdown(5)
{
    ui->setupUi(this);
    ui->PassEdit->setEchoMode(QLineEdit::Password);
    ui->YesEdit->setEchoMode(QLineEdit::Password);
    ui->ErrorLabel->setProperty("state", "normal");
    repolish(ui->ErrorLabel);

    connect(HttpManager::GetInstance().get(), &HttpManager::sig_reg_mod_finish, this,
            &RegisterDialog::slot_reg_mod_finish);
    initHttpHandlers();

    ui->ErrorLabel->clear();
    ui->pass_visible->setCursor(Qt::PointingHandCursor);
    ui->confirm_visible->setCursor(Qt::PointingHandCursor);
    ui->pass_visible->SetState("unvisible", "unvisible_hover", "", "visible", "visible_hover", "");
    ui->confirm_visible->SetState("unvisible", "unvisible_hover", "", "visible", "visible_hover", "");

    connect(ui->pass_visible, &ClickedLabel::clicked, this, [this]() {
        auto state = ui->pass_visible->GetCurState();
        if(state == ClickLbState::Normal){
            ui->PassEdit->setEchoMode(QLineEdit::Password);
        }else{
            ui->PassEdit->setEchoMode(QLineEdit::Normal);
        }
        qDebug() << "Label was clicked!";
    });
    connect(ui->confirm_visible, &ClickedLabel::clicked, this, [this]() {
        auto state = ui->confirm_visible->GetCurState();
        if(state == ClickLbState::Normal){
            ui->YesEdit->setEchoMode(QLineEdit::Password);
        }else{
            ui->YesEdit->setEchoMode(QLineEdit::Normal);
        }
        qDebug() << "Label was clicked!";
    });
    connect(ui->UserEdit, &QLineEdit::editingFinished, this, [this](){
        checkUserValid();
    });
    connect(ui->EmailEdit, &QLineEdit::editingFinished, this, [this](){
        checkEmailValid();
    });
    connect(ui->PassEdit, &QLineEdit::editingFinished, this, [this](){
        checkPassValid();
    });
    connect(ui->TestEdit, &QLineEdit::editingFinished, this, [this](){
        checkTestValid();
    });

    //创建定时器，连接槽和信号
    _countdown_timer = new QTimer(this);
    connect(_countdown_timer, &QTimer::timeout,[this](){
        if(_countdown == 0){
            _countdown_timer->stop();
            emit sigSwitchLogin();
            return;
        }
        _countdown--;
        auto str = QString("注册成功， %1 s后返回登录").arg(_countdown);
        ui->tiplb->setText(str);
    });
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::on_TestBtn_clicked()
{
    //判断邮箱是否符合条件
    auto email = ui->EmailEdit->text();
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch();
    if(match){
        //发送http验证码
        QJsonObject json_obj;
        json_obj["email"] = email;
        HttpManager::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/get_varifycode"),
                                                json_obj, ReqId::ID_GET_VARIFY_CODE, Modules::REGISTERMOD);

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
        ui->ErrorLabel->setProperty("state", "err");
    }
    ui->ErrorLabel->setText(str);

    repolish(ui->ErrorLabel);
}

void RegisterDialog::AddTipErr(TipErr te, QString tips)
{
    _tip_errs[te] = tips;
    showTip(tips, false);
}

void RegisterDialog::DelTipErr(TipErr te)
{
    _tip_errs.remove(te);
    if(_tip_errs.empty()){
        ui->ErrorLabel->clear();
        return;
    }
    showTip(_tip_errs.first(), false);
}

bool RegisterDialog::checkUserValid()
{
    if(ui->UserEdit->text() == ""){
        AddTipErr(TipErr::TIP_USER_ERR, tr("用户名不能为空"));
        return false;
    }
    DelTipErr(TipErr::TIP_USER_ERR);
    return true;
}

bool RegisterDialog::checkEmailValid()
{
    //判断邮箱是否符合条件
    auto email = ui->EmailEdit->text();
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch();
    if(!match){
        AddTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱地址不正确"));
        return false;
    }
    DelTipErr(TipErr::TIP_EMAIL_ERR);
    return true;
}

bool RegisterDialog::checkPassValid()
{
    auto pass = ui->PassEdit->text();
    auto confirm = ui->YesEdit->text();
    if(pass.length() < 6 || pass.length() > 15){
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码长度应该为6-15"));
        return false;
    }
    // 创建一个正则表达式对象，按照上述密码要求
    // 这个正则表达式解释：
    // ^[a-zA-Z0-9!@#$%^&*]{6,15}$ 密码长度至少6，可以是字母、数字和特定的特殊字符
    QRegularExpression regExp("^[a-zA-Z0-9!@#$%^&*]{6,15}$");
    bool match = regExp.match(pass).hasMatch();
    if(!match){
        //提示字符非法
        AddTipErr(TipErr::TIP_PWD_ERR, tr("不能包含非法字符"));
        return false;;
    }
    DelTipErr(TipErr::TIP_PWD_ERR);
    if(pass != confirm){
        AddTipErr(TipErr::TIP_CONFIRM_ERR, tr("密码与确认密码不匹配"));
        return false;
    }
    DelTipErr(TipErr::TIP_CONFIRM_ERR);
    return true;
}

bool RegisterDialog::checkTestValid()
{
    auto pass = ui->TestEdit->text();
    if(pass.isEmpty()){
        AddTipErr(TipErr::TIP_VARIFY_ERR, tr("验证码不能为空"));
        return false;
    }
    DelTipErr(TipErr::TIP_VARIFY_ERR);
    return true;
}

void RegisterDialog::ChangeTipPage()
{
    _countdown_timer->stop();
    ui->stackedWidget->setCurrentWidget((ui->page_2));
    _countdown_timer->start(1000);
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
    //注册注册用户回报的逻辑
    _handlers.insert(ReqId::ID_REG_USER, [this](const QJsonObject& jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"), false);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("注册成功"), true);
        qDebug() << "email is " << email;
        ChangeTipPage();
    });
}




void RegisterDialog::on_RegisterBtn_clicked()
{
    bool valid = checkUserValid();
    if(!valid){
        return;
    }
    valid = checkEmailValid();
    if(!valid){
        return;
    }
    valid = checkPassValid();
    if(!valid){
        return;
    }
    valid = checkTestValid();
    if(!valid){
        return;
    }
    //发送http请求注册用户
    QJsonObject json_obj;
    json_obj["user"] = ui->UserEdit->text();
    json_obj["email"] = ui->EmailEdit->text();
    json_obj["passwd"] = xorString(ui->PassEdit->text());
    json_obj["confirm"] = xorString(ui->YesEdit->text());
    json_obj["varifycode"] = ui->TestEdit->text();
    HttpManager::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/user_register"), json_obj,
                                            ReqId::ID_REG_USER,Modules::REGISTERMOD);

}


void RegisterDialog::on_returnbtn_clicked()
{
    _countdown_timer->stop();
    emit sigSwitchLogin();
}


void RegisterDialog::on_CancelRegisterBtn_clicked()
{
    _countdown_timer->stop();
    emit sigSwitchLogin();
}

