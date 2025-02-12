#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "logindialog.h"
#include "registerdialog.h"
#include "resetdialog.h"
/**
 * @file  mainwindow.h
 * @brief 主窗口
 *
 * @author zyl
 * @date 2025/01/17
 *
 * 详细描述
 */

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    LoginDialog *login_dlg;
    RegisterDialog *register_dlg;
    ResetDialog *reset_dlg;
public slots:
    void SlotSwicthRegister();
    void SlotSwitchLogin();
    void SlotSwitchReset();
    void SlotSwtichLogin2();

};
#endif // MAINWINDOW_H
