#include "messagetextedit.h"
#include <QDebug>
#include <QMessageBox>


MessageTextEdit::MessageTextEdit(QWidget *parent)
    : QTextEdit(parent)
{

    //this->setStyleSheet("border: none;");
    this->setMaximumHeight(60);
    //connect(this,SIGNAL(textChanged()),this,SLOT(textEditChanged()));

}

MessageTextEdit::~MessageTextEdit()
{

}

QVector<MsgInfo> MessageTextEdit::getMsgList()
{
    mGetMsgList.clear();
    //获取纯文本内容
    QString doc = this->document()->toPlainText();
    QString text="";//存储文本信息
    int indexUrl = 0;
    int count = mMsgList.size();

    for(int index=0; index<doc.size(); index++)
    {
        //如果当前遍历的是特殊字符
        if(doc[index]==QChar::ObjectReplacementCharacter)
        {
            //如果text中有内容，先将这个内容插入到 mGetMsgList 中
            if(!text.isEmpty())
            {
                QPixmap pix;
                insertMsgList(mGetMsgList,"text", text, pix);
                text.clear();
            }
            //遍历mMsgList
            while(indexUrl<count)
            {
                MsgInfo msg =  mMsgList[indexUrl];
                //toHtml() 会返回文档的 HTML 表现形式，包含文档中的文本、格式、字体、颜色、图片、表格等信息。
                //大小写敏感，去判断 HTML 文档中有没有 msg.content
                if(this->document()->toHtml().contains(msg.content,Qt::CaseSensitive))
                {
                    indexUrl++;
                    mGetMsgList.append(msg);
                    break;
                }
                indexUrl++;
            }
        }
        //是文本内容，直接添加即可
        else
        {
            text.append(doc[index]);
        }
    }
    //如果 text 非空，则将其加入到 mGetMsgList 中
    //所以 mGetMsgList 中的每一个元素就是一个待发送条目，可能是纯文本，可能是图片等
    if(!text.isEmpty())
    {
        QPixmap pix;
        insertMsgList(mGetMsgList,"text",text,pix);
        text.clear();
    }
    mMsgList.clear();
    this->clear();
    return mGetMsgList;
}

//处理拖拽事件
void MessageTextEdit::dragEnterEvent(QDragEnterEvent *event)
{
    //拖拽的源地址是当前控件则忽略
    if(event->source()==this)
        event->ignore();
    else
        event->accept();
}

//处理释放事件，将一个东西拖拽到 MessageTextEdit 然后释放触发该函数
void MessageTextEdit::dropEvent(QDropEvent *event)
{
    //event->mimeDate: 返回要释放的数据
    insertFromMimeData(event->mimeData());
    event->accept();
}

void MessageTextEdit::keyPressEvent(QKeyEvent *e)
{
    if((e->key()==Qt::Key_Enter||e->key()==Qt::Key_Return)&& !(e->modifiers() & Qt::ShiftModifier))
    {
        emit send();
        return;
    }
    QTextEdit::keyPressEvent(e);
}

void MessageTextEdit::insertFileFromUrl(const QStringList &urls)
{
    if(urls.isEmpty())
        return;

    foreach (QString url, urls){
        if(isImage(url))
            insertImages(url);
        else
            insertTextFile(url);
    }
}

//在 MessageTextEdit 中插入图片
void MessageTextEdit::insertImages(const QString &url)
{
    QImage image(url);
    //按比例缩放图片
    if(image.width()>120||image.height()>80)
    {
        if(image.width()>image.height())
        {
            image =  image.scaledToWidth(120,Qt::SmoothTransformation);
        }
        else
            image = image.scaledToHeight(80,Qt::SmoothTransformation);
    }
    //获取当前的光标位置，插入图片
    QTextCursor cursor = this->textCursor();
    // QTextDocument *document = this->document();
    // document->addResource(QTextDocument::ImageResource, QUrl(url), QVariant(image));
    cursor.insertImage(image,url);

    insertMsgList(mMsgList,"image",url,QPixmap::fromImage(image));
}

void MessageTextEdit::insertTextFile(const QString &url)
{
    QFileInfo fileInfo(url);
    if(fileInfo.isDir())
    {
        QMessageBox::information(this,"提示","只允许拖拽单个文件!");
        return;
    }

    if(fileInfo.size()>100*1024*1024)
    {
        QMessageBox::information(this,"提示","发送的文件大小不能大于100M");
        return;
    }

    QPixmap pix = getFileIconPixmap(url);
    QTextCursor cursor = this->textCursor();
    cursor.insertImage(pix.toImage(),url);
    insertMsgList(mMsgList,"file",url,pix);
}

bool MessageTextEdit::canInsertFromMimeData(const QMimeData *source) const
{
    return QTextEdit::canInsertFromMimeData(source);
}

//从 QMimeData 对象中提取数据，并根据不同的数据类型（如图片、文本文件等）将其插入到控件中。
void MessageTextEdit::insertFromMimeData(const QMimeData *source)
{
    QStringList urls = getUrl(source->text());

    if(urls.isEmpty())
        return;
    //如果是url，那就证明用户拖入了一个文件或者图片，分别进行处理
    foreach (QString url, urls)
    {
        if(isImage(url))
            insertImages(url);
        else
            insertTextFile(url);
    }
}

//判断一个 url 是否是一个图片
bool MessageTextEdit::isImage(QString url)
{
    QString imageFormat = "bmp,jpg,png,tif,gif,pcx,tga,exif,fpx,svg,psd,cdr,pcd,dxf,ufo,eps,ai,raw,wmf,webp";
    QStringList imageFormatList = imageFormat.split(",");
    QFileInfo fileInfo(url);
    //返回扩展名，即最后一个 . 后面的内容
    QString suffix = fileInfo.suffix();
    if(imageFormatList.contains(suffix,Qt::CaseInsensitive)){
        return true;
    }
    return false;
}

void MessageTextEdit::insertMsgList(QVector<MsgInfo> &list, QString flag, QString text, QPixmap pix)
{
    MsgInfo msg;
    msg.msgFlag=flag;
    msg.content = text;
    msg.pixmap = pix;
    list.append(msg);
}

//提取文本中的 url ，然后返回
QStringList MessageTextEdit::getUrl(QString text)
{

    QStringList urls;
    if(text.isEmpty()) return urls;

    //将文本按照换行符进行分隔，返回一个string列表
    QStringList list = text.split("\n");
    //遍历列表， url 是当前的 string
    foreach (QString url, list) {
        if(!url.isEmpty()){
            //处理url, 按照 "///" 分隔，file:///home/user/images/image1.png
            QStringList str = url.split("///");
            if(str.size()>=2)
                urls.append(str.at(1));
        }
    }
    return urls;
}

//得到文件图标
QPixmap MessageTextEdit::getFileIconPixmap(const QString &url)
{
    QFileIconProvider provder;
    QFileInfo fileinfo(url);
    QIcon icon = provder.icon(fileinfo);

    QString strFileSize = getFileSize(fileinfo.size());
    //qDebug() << "FileSize=" << fileinfo.size();

    QFont font(QString("宋体"),10,QFont::Normal,false);
    QFontMetrics fontMetrics(font);
    QSize textSize = fontMetrics.size(Qt::TextSingleLine, fileinfo.fileName());

    QSize FileSize = fontMetrics.size(Qt::TextSingleLine, strFileSize);
    int maxWidth = textSize.width() > FileSize.width() ? textSize.width() :FileSize.width();
    QPixmap pix(50 + maxWidth + 10, 50);
    pix.fill();

    QPainter painter;
    // painter.setRenderHint(QPainter::Antialiasing, true);
    //painter.setFont(font);
    painter.begin(&pix);
    // 文件图标
    QRect rect(0, 0, 50, 50);
    painter.drawPixmap(rect, icon.pixmap(40,40));
    painter.setPen(Qt::black);
    // 文件名称
    QRect rectText(50+10, 3, textSize.width(), textSize.height());
    painter.drawText(rectText, fileinfo.fileName());
    // 文件大小
    QRect rectFile(50+10, textSize.height()+5, FileSize.width(), FileSize.height());
    painter.drawText(rectFile, strFileSize);
    painter.end();
    return pix;
}

//获取文件大小
QString MessageTextEdit::getFileSize(qint64 size)
{
    QString Unit;
    double num;
    if(size < 1024){
        num = size;
        Unit = "B";
    }
    else if(size < 1024 * 1224){
        num = size / 1024.0;
        Unit = "KB";
    }
    else if(size <  1024 * 1024 * 1024){
        num = size / 1024.0 / 1024.0;
        Unit = "MB";
    }
    else{
        num = size / 1024.0 / 1024.0/ 1024.0;
        Unit = "GB";
    }
    return QString::number(num,'f',2) + " " + Unit;
}

void MessageTextEdit::textEditChanged()
{
    //qDebug() << "text changed!" << endl;
}
