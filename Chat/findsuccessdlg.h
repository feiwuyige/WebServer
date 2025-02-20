#ifndef FINDSUCCESSDLG_H
#define FINDSUCCESSDLG_H

#include <QDialog>
#include "userdata.h"
#include <memory>
#include "applyfriend.h"

namespace Ui {
class FindSuccessDlg;
}

class FindSuccessDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FindSuccessDlg(QWidget *parent = nullptr);
    ~FindSuccessDlg();
    void SetSearchInfo(std::shared_ptr<SearchInfo> si);

private slots:
    void on_add_friend_btn_clicked();

private:
    Ui::FindSuccessDlg *ui;
    std::shared_ptr<SearchInfo> _si;
    QWidget * _parent;
    std::unique_ptr<ApplyFriend> _apply_friend;

};

#endif // FINDSUCCESSDLG_H
