#ifndef CUSTOMIZEEDIT_H
#define CUSTOMIZEEDIT_H
#include <QLineEdit>
#include <QDebug>

class CustomizeEdit : public QLineEdit
{
    Q_OBJECT
public:
    CustomizeEdit(QWidget *parent = nullptr);
    void SetMaxLength(int maxLen);
protected:
    virtual void focusOutEvent(QFocusEvent *event) override;
private:
    void limitTextLength(QString text);
    int _max_length;
signals:
    void sig_focus_out();
};

#endif // CUSTOMIZEEDIT_H
