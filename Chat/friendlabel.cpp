#include "friendlabel.h"
#include "ui_friendlabel.h"



FriendLabel::FriendLabel(QWidget *parent):QFrame(parent), ui(new Ui::FriendLabel)
{
    ui->setupUi(this);
    ui->close_lb->SetState("normal", "hover", "pressed", "selected_normal", "selected_hover", "selected_pressed");
    connect(ui->close_lb, &ClickedLabel::clicked, this, &FriendLabel::slot_close);
}

FriendLabel::~FriendLabel()
{
    delete ui;
}

void FriendLabel::SetText(QString text)
{
    _text = text;
    ui->tip_lb->setText(_text);
    ui->tip_lb->adjustSize();

    auto textWidth = ui->tip_lb->width();
    auto textHeight = ui->tip_lb->height();
    // QFontMetrics fontmetrics(ui->tip_lb->font());
    // auto textWidth = fontmetrics.horizontalAdvance(ui->tip_lb->text());
    // auto textHeight = fontmetrics.height();

    this->setFixedWidth(textWidth + ui->close_lb->width() + 10);
    this->setFixedHeight(textHeight + 2);
    _width = this->Width();
    _height = this->Height();
}

int FriendLabel::Width()
{
    return _width;
}

int FriendLabel::Height()
{
    return _height;
}

QString FriendLabel::Text()
{
    return _text;
}

void FriendLabel::slot_close()
{
    emit sig_close(_text);
}
