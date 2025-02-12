#include "logindialog.h"
#include "ui_logindialog.h"

void LoginDialog::slot_forget_pwd()
{
    emit switchReset();
}

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
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


    //将按钮事件与信号连接起来
    connect(ui->RegisterBtn, &QPushButton::clicked, this, &LoginDialog::switchRegister);
    ui->ForgetLabel->SetState("normal", "hover", "", "selected", "selected_hover", "");

    connect(ui->ForgetLabel, &ClickedLabel::clicked, this, &LoginDialog::slot_forget_pwd);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}
