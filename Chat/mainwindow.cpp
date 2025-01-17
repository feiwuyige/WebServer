#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    login_dlg = new LoginDialog();
    setCentralWidget(login_dlg);
    login_dlg->show();

}

MainWindow::~MainWindow()
{
    delete ui;
}
