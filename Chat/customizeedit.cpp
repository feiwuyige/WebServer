#include "customizeedit.h"


CustomizeEdit::CustomizeEdit(QWidget *parent) : QLineEdit(parent), _max_length(0)
{
    connect(this, &QLineEdit::textChanged, this, &CustomizeEdit::limitTextLength);
}

void CustomizeEdit::SetMaxLength(int maxLen)
{
    _max_length = maxLen;
}

void CustomizeEdit::focusOutEvent(QFocusEvent *event)
{
    QLineEdit::focusOutEvent(event);
    emit sig_focus_out();
}

void CustomizeEdit::limitTextLength(QString text)
{
    if(_max_length <= 0){
        return;
    }
    QByteArray byteArray = text.toUtf8();
    if(byteArray.size() > _max_length){
        byteArray = byteArray.left(_max_length);
        this->setText(QString::fromUtf8(byteArray));
    }
}
