#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include "global.h"
#include "statewidget.h"

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();
    void addChatUserList();
protected:
    bool eventFilter(QObject * watched, QEvent *event) override;
    void handleGlobalMouseEvent(QMouseEvent *event);
private:
    void ClearLabelState(StateWidget *lb);
    void AddLBGroup(StateWidget* lb);
    void ShowSearch(bool bsearch);
    Ui::ChatDialog *ui;
    ChatUIMode _state;
    ChatUIMode _mode;
    bool _b_loading;
    QList<StateWidget*> _lb_list;
public slots:
    void slot_loading_chat_user();
    void slot_side_chat();
    void slot_side_contact();
    void slot_text_changed(const QString& str);

};

#endif // CHATDIALOG_H
