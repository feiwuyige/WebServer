#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpmgr.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    login_dlg = new LoginDialog(this);
    login_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(login_dlg);
    //创建和注册消息链接
    connect(login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwicthRegister);
    //连接忘记密码信号
    connect(login_dlg, &LoginDialog::switchReset, this, &MainWindow::SlotSwitchReset);
    //连接聊天界面信号
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_swich_chatdlg, this, &MainWindow::SlotSwtichChat);
    //emit TcpMgr::GetInstance().get()->sig_swich_chatdlg();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SlotSwicthRegister()
{
    register_dlg = new RegisterDialog(this);

    //将dialog窗口设置成无边框，从而可以实现嵌入的效果

    register_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    connect(register_dlg, &RegisterDialog::sigSwitchLogin, this, &MainWindow::SlotSwitchLogin);
    setCentralWidget(register_dlg);
    login_dlg->hide();
    register_dlg->show();
}

void MainWindow::SlotSwitchLogin()
{

    login_dlg = new LoginDialog(this);
    login_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(login_dlg);
    //创建和注册消息链接
    connect(login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwicthRegister);
    //连接忘记密码信号
    connect(login_dlg, &LoginDialog::switchReset, this, &MainWindow::SlotSwitchReset);
    register_dlg->hide();
    login_dlg->show();
}

void MainWindow::SlotSwitchReset()
{
    reset_dlg = new ResetDialog(this);
    reset_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(reset_dlg);
    login_dlg->hide();
    reset_dlg->show();
    //注册返回信号的槽函数
    connect(reset_dlg, &ResetDialog::switchLogin, this, &MainWindow::SlotSwtichLogin2);
}

void MainWindow::SlotSwtichLogin2()
{
    login_dlg = new LoginDialog(this);
    login_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(login_dlg);
    //创建和注册消息链接
    connect(login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwicthRegister);
    //连接忘记密码信号
    connect(login_dlg, &LoginDialog::switchReset, this, &MainWindow::SlotSwitchReset);
    reset_dlg->hide();
    login_dlg->show();
}

void MainWindow::SlotSwtichChat()
{
    _chat_dlg = new ChatDialog();
    _chat_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setCentralWidget(_chat_dlg);
    _chat_dlg->show();
    login_dlg->hide();
    this->setMinimumSize(QSize(1050, 800));
    this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}


