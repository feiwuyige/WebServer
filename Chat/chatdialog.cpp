#include "chatdialog.h"
#include "ui_chatdialog.h"
#include <QAction>
#include <QRandomGenerator>
#include "chatuserwid.h"
#include <QMovie>
#include <QLabel>
#include <QListWidgetItem>
#include <QTimer>

ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChatDialog), _state(ChatUIMode::ChatMode), _mode(ChatUIMode::ChatMode), _b_loading(false)
{
    ui->setupUi(this);
    ui->add_btn->SetState("normal", "hover", "press");
    ui->search_edit->SetMaxLength(30);

    QAction* searchAction = new QAction(ui->search_edit);
    searchAction->setIcon(QIcon(":/img/search.png"));
    ui->search_edit->addAction(searchAction, QLineEdit::LeadingPosition);
    ui->search_edit->setPlaceholderText(QStringLiteral("搜索"));
    //创建清除动作,透明的图标，初始时不显示
    QAction* clearAction = new QAction(ui->search_edit);
    clearAction->setIcon(QIcon(":/img/close_transparent.png"));
    ui->search_edit->addAction(clearAction, QLineEdit::TrailingPosition);
    connect(ui->search_edit, &QLineEdit::textChanged, [clearAction](const QString &text){
        if(!text.isEmpty()){
            clearAction->setIcon(QIcon(":/img/close_search.png"));
        }
        else{
            clearAction->setIcon(QIcon(":/img/close_transparent.png"));
        }
    });
    //连接清除动作
    connect(clearAction, &QAction::triggered, [this, clearAction](){
        ui->search_edit->clear();
        clearAction->setIcon(QIcon(":/img/close_transparent.png"));
        ui->search_edit->clearFocus();
        //清除按钮按下，则不显示搜索框
        ShowSearch(false);
    });
    ShowSearch(false);
    connect(ui->chat_user_list, &ChatUserList::sig_loading_chat_user, this, &ChatDialog::slot_loading_chat_user);
    addChatUserList();
}

ChatDialog::~ChatDialog()
{
    delete ui;
}

void ChatDialog::addChatUserList()
{
    std::vector<QString>  strs ={"hello world !",
                                 "nice to meet u",
                                 "New year，new life",
                                 "You have to love yourself",
                                 "My love is written in the wind ever since the whole world is you"};
    std::vector<QString> heads = {
        ":/img/head_1.jpg",
        ":/img/head_2.jpg",
        ":/img/head_3.jpg",
        ":/img/head_4.jpg",
        ":/img/head_5.jpg"
    };
    std::vector<QString> names = {
        "llfc",
        "zack",
        "golang",
        "cpp",
        "java",
        "nodejs",
        "python",
        "rust"
    };
    // 创建QListWidgetItem，并设置自定义的widget
    for(int i = 0; i < 13; i++){
        int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int str_i = randomValue%strs.size();
        int head_i = randomValue%heads.size();
        int name_i = randomValue%names.size();
        auto *chat_user_wid = new ChatUserWid();
        chat_user_wid->SetInfo(names[name_i], heads[head_i], strs[str_i]);
        QListWidgetItem *item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(chat_user_wid->sizeHint());
        ui->chat_user_list->addItem(item);
        ui->chat_user_list->setItemWidget(item, chat_user_wid);
    }
}

void ChatDialog::ShowSearch(bool bsearch)
{
    if(bsearch){
        ui->chat_user_list->hide();
        ui->con_user_list->hide();
        ui->search_list->show();
        _mode = ChatUIMode::SearchMode;
    }else if(_state == ChatUIMode::ChatMode){
        ui->chat_user_list->show();
        ui->con_user_list->hide();
        ui->search_list->hide();
        _mode = ChatUIMode::ChatMode;
    }else if(_state == ChatUIMode::ContactMode){
        ui->chat_user_list->hide();
        ui->search_list->hide();
        ui->con_user_list->show();
        _mode = ChatUIMode::ContactMode;
    }
}

void ChatDialog::slot_loading_chat_user()
{
    if(_b_loading){
        return;
    }
    _b_loading = true;
    //LoadingDlg * loadingDialog = new LoadingDlg(this);
    QLabel *loading_item = new QLabel(this);
    QMovie *movie = new QMovie(":/img/loading.gif");
    loading_item->setMovie(movie);
    loading_item->setFixedSize(250, 70);
    loading_item->setAlignment(Qt::AlignCenter);
    movie->setScaledSize(QSize(50, 50));

    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(QSize(250, 70));
    ui->chat_user_list->addItem(item);
    ui->chat_user_list->setItemWidget(item, loading_item);
    movie->start();


    QTimer::singleShot(1000, this, [this, item](){
        qDebug() << " add new data to list ...";
        addChatUserList();
        ui->chat_user_list->takeItem(ui->chat_user_list->row(item));
        ui->chat_user_list->update();
        _b_loading = false;
    });
}
