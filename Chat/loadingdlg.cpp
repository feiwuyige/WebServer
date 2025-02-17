#include "loadingdlg.h"
#include "ui_loadingdlg.h"
#include <QMovie>
#include <QLabel>
#include <QListWidgetItem>

LoadingDlg::LoadingDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoadingDlg)
{
    ui->setupUi(this);


    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(parent->size());
}

LoadingDlg::~LoadingDlg()
{
    delete ui;
}
