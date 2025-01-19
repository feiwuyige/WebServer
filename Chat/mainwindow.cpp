#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    login_dlg = new LoginDialog(this);
    setCentralWidget(login_dlg);

    //创建和注册消息链接
    connect(login_dlg, &LoginDialog::switchRegister, this, &MainWindow::SlotSwicthRegister);
    register_dlg = new RegisterDialog(this);

    //将dialog窗口设置成无边框，从而可以实现嵌入的效果
    login_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    register_dlg->setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    register_dlg->hide();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SlotSwicthRegister()
{
    setCentralWidget(register_dlg);
    login_dlg->hide();
    register_dlg->show();
}
