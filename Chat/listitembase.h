#ifndef LISTITEMBASE_H
#define LISTITEMBASE_H
#include <QWidget>
#include "global.h"
#include <QPainter>

class ListItemBase: public QWidget
{
    Q_OBJECT
protected:
    virtual void paintEvent(QPaintEvent *event) override;
public:
    explicit ListItemBase(QWidget *parent = nullptr);
    void SetItemType(ListItemType itemType);
    ListItemType GetItemType();
private:
    ListItemType _itemType;
public slots:
signals:
};

#endif // LISTITEMBASE_H
