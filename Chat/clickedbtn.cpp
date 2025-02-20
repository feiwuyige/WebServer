#include "clickedbtn.h"
#include "global.h"

ClickedBtn::ClickedBtn(QWidget *parent) : QPushButton(parent)
{
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::NoFocus);
}

ClickedBtn::~ClickedBtn()
{

}

void ClickedBtn::SetState(QString normal, QString hover, QString press)
{
    _normal = normal;
    _hover = hover;
    _press = press;
    setProperty("state", normal);
    repolish(this);
    update();
}

void ClickedBtn::enterEvent(QEnterEvent *event)
{
    setProperty("state", _hover);
    repolish(this);
    update();
    QPushButton::enterEvent(event);
}

void ClickedBtn::leaveEvent(QEvent *event)
{
    setProperty("state", _normal);
    repolish(this);
    update();
    QPushButton::leaveEvent(event);
}

void ClickedBtn::mousePressEvent(QMouseEvent *e)
{
    setProperty("state", _press);
    repolish(this);
    update();
    QPushButton::mousePressEvent(e);
}

void ClickedBtn::mouseReleaseEvent(QMouseEvent *e)
{
    setProperty("state", _hover);
    repolish(this);
    update();
    QPushButton::mouseReleaseEvent(e);
}
