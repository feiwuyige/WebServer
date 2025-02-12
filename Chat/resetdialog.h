#ifndef RESETDIALOG_H
#define RESETDIALOG_H

#include <QDialog>
#include "global.h"
#include "httpmanager.h"
namespace Ui {
class ResetDialog;
}

class ResetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResetDialog(QWidget *parent = nullptr);
    ~ResetDialog();


signals:
    void switchLogin();
public slots:
    void slot_reset_mod_finish(ReqId id, QString res, ErrorCodes err);
private slots:
    void on_CancelBtn_clicked();

    void on_ConfirmBtn_clicked();

    void on_VarifyBtn_clicked();

private:
    void showTip(QString str, bool ok);
    void AddTipErr(TipErr te, QString tips);
    void DelTipErr(TipErr te);
    bool checkUserValid();
    bool checkEmailValid();
    bool checkPassValid();
    bool checkTestValid();
private:
    QMap<TipErr, QString> _tip_errs;
    Ui::ResetDialog *ui;
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;
    void initHandlers();
};

#endif // RESETDIALOG_H
