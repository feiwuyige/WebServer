#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "global.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public slots:
    void slot_forget_pwd();
public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();
signals:
    void switchRegister();
    void switchReset();
    void sig_connect_tcp(ServerInfo si);
private slots:
    void on_LoginBtn_clicked();
    void slot_login_mod_finish(ReqId id, QString res, ErrorCodes err);

private:
    void showTip(QString str, bool ok);
    void AddTipErr(TipErr te, QString tips);
    void DelTipErr(TipErr te);
    bool checkUserValid();
    bool checkPassValid();
    void initHttpHandlers();
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;
    QMap<TipErr, QString> _tip_errs;

    int _uid;
    QString _token;



    void initHead();
    bool checkUsrValid();
    bool checkPwdValid();
    void enableBtn(bool ok);
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
