#include "clickedlabel.h"
#include "QMouseEvent"

ClickedLabel::ClickedLabel(QWidget *parent) : QLabel(parent), _curstate(ClickLbState::Normal)
{
    this->setCursor(Qt::PointingHandCursor);
}

void ClickedLabel::enterEvent(QEnterEvent *event)
{
    if(_curstate == ClickLbState::Normal){
        setProperty("state", _normal_hover);
        repolish(this);
        update();
    }
    else{
        setProperty("state", _select_hover);
        repolish(this);
        update();
    }
    QLabel::enterEvent(event);
}

void ClickedLabel::leaveEvent(QEvent *event)
{
    if(_curstate == ClickLbState::Normal){
        setProperty("state", _normal);
        repolish(this);
        update();
    }
    else{
        setProperty("state", _select);
        repolish(this);
        update();
    }
    QLabel::leaveEvent(event);
}

void ClickedLabel::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::LeftButton){
        if(_curstate == ClickLbState::Normal){
            _curstate = ClickLbState::Select;
            setProperty("state", _select_hover);
            repolish(this);
            update();
        }
        else{
            _curstate = ClickLbState::Normal;
            setProperty("state", _normal_hover);
            repolish(this);
            update();
        }
        emit clicked();
    }
    QLabel::mousePressEvent(ev);
}

void ClickedLabel::SetState(QString normal, QString hover, QString press, QString select, QString select_hover, QString select_press)
{
    _normal = normal;
    _normal_hover = hover;
    _normal_press = press;

    _select = select;
    _select_hover = select_hover;
    _select_press = select_press;

    setProperty("state", normal);
    repolish(this);
}

ClickLbState ClickedLabel::GetCurState()
{
    return _curstate;
}
