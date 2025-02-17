#ifndef CLICKEDLABEL_H
#define CLICKEDLABEL_H

#include <QLabel>
#include "global.h"

class ClickedLabel : public QLabel
{
    Q_OBJECT
public:
    ClickedLabel(QWidget *parent=nullptr);
    virtual void enterEvent(QEnterEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void SetState(QString normal = "", QString hover= "" , QString press = "",
                  QString select = "", QString select_hover = "",QString select_press = "");
    ClickLbState GetCurState();
private:
    QString _normal;
    QString _normal_hover;
    QString _normal_press;

    QString _select;
    QString _select_hover;
    QString _select_press;

    ClickLbState _curstate;
signals:
    void clicked(void);
};

#endif // CLICKEDLABEL_H
