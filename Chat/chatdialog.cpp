#include "chatdialog.h"
#include "ui_chatdialog.h"
#include <QAction>
#include <QRandomGenerator>
#include "chatuserwid.h"
#include <QMovie>
#include <QLabel>
#include <QListWidgetItem>
#include <QTimer>
#include "tcpmgr.h"
#include "usermgr.h"

ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChatDialog), _state(ChatUIMode::ChatMode), _mode(ChatUIMode::ChatMode), _b_loading(false)
    , _cur_chat_uid(0), _last_widget(nullptr)
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
    connect(ui->con_user_list, &ContactUserList::sig_loading_contact_user, this, &ChatDialog::slot_loading_con_user);
    addChatUserList();

    QPixmap pixmap(":/img/head_1.jpg");
    QPixmap scaledPixmap = pixmap.scaled(ui->side_head_lb->size(), Qt::KeepAspectRatio);
    ui->side_head_lb->setPixmap(scaledPixmap);
    ui->side_head_lb->setScaledContents(true);
    //设置选中条目
    SetSelectChatItem();
    //更新聊天界面信息
    SetSelectChatPage();

    ui->side_chat_lb->setProperty("state", "normal");
    ui->side_chat_lb->SetState("normal", "hover", "pressed", "selected_normal", "selected_hover", "selected_pressed");
    ui->side_contact_lb->SetState("normal", "hover", "pressed", "selected_normal", "selected_hover", "selected_pressed");

    AddLBGroup(ui->side_chat_lb);
    AddLBGroup(ui->side_contact_lb);

    connect(ui->side_chat_lb, &StateWidget::clicked, this, &ChatDialog::slot_side_chat);
    connect(ui->side_contact_lb, &StateWidget::clicked, this, &ChatDialog::slot_side_contact);

    //连接搜索框输入变化
    connect(ui->search_edit, &QLineEdit::textChanged, this, &ChatDialog::slot_text_changed);

    //检测鼠标点击位置判断是否要清空搜索框
    this->installEventFilter(this); //安装事件过滤器
    ui->search_list->SetSearchEdit(ui->search_edit);
    //连接申请添加好友信号
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_friend_apply, this, &ChatDialog::slot_apply_friend);

    //连接添加好友成功信号
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_add_auth_friend, this,
            &ChatDialog::slot_add_auth_friend);
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_auth_rsp, this, &ChatDialog::slot_auth_rsp);
    //连接搜索信号
    connect(ui->search_list, &SearchList::sig_jump_chat_item, this, &ChatDialog::slot_jump_chat_item);
    connect(ui->friend_info_page, &FriendInfoPage::sig_jump_chat_item, this, &ChatDialog::slot_jump_chat_item_from_infopage);
    //添加点击联系人item发出的信号和用户信息展示槽函数
    connect(ui->con_user_list, &ContactUserList::sig_switch_friend_info_page, this, &ChatDialog::slot_friend_info_page);
    //连接联系人页面点击好友申请条目
    connect(ui->con_user_list, &ContactUserList::sig_switch_apply_friend_page,
            this, &ChatDialog::slot_switch_apply_friend_page);

    //设置中心部件为chatpage
    ui->stackedWidget->setCurrentWidget(ui->chat_page);
    //点击聊天item，聊天界面切换
    connect(ui->chat_user_list, &QListWidget::itemClicked, this, &ChatDialog::slot_item_clicked);

    connect(ui->chat_page, &ChatPage::sig_append_send_chat_msg, this, &ChatDialog::slot_append_send_chat_msg);

    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_text_chat_msg, this,&ChatDialog::slot_text_chat_msg);

}

ChatDialog::~ChatDialog()
{
    delete ui;
}

void ChatDialog::addChatUserList()
{
    auto friend_list = UserMgr::GetInstance()->GetChatListPerPage();
    if(friend_list.empty() == false){
        for(auto& friend_ele : friend_list){
            auto find_iter = _chat_items_added.find(friend_ele->_uid);
            if(find_iter == _chat_items_added.end()){
                continue;
            }
            auto *chat_user_wid = new ChatUserWid();
            auto user_info = std::make_shared<UserInfo>(friend_ele);
            chat_user_wid->SetInfo(user_info);
            QListWidgetItem *item = new QListWidgetItem;
            //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
            item->setSizeHint(chat_user_wid->sizeHint());
            ui->chat_user_list->addItem(item);
            ui->chat_user_list->setItemWidget(item, chat_user_wid);
            _chat_items_added.insert(friend_ele->_uid, item);
        }
    }
    // std::vector<QString>  strs ={"hello world !",
    //                              "nice to meet u",
    //                              "New year，new life",
    //                              "You have to love yourself",
    //                              "My love is written in the wind ever since the whole world is you"};
    // std::vector<QString> heads = {
    //     ":/img/head_1.jpg",
    //     ":/img/head_2.jpg",
    //     ":/img/head_3.jpg",
    //     ":/img/head_4.jpg",
    //     ":/img/head_5.jpg"
    // };
    // std::vector<QString> names = {
    //     "llfc",
    //     "zack",
    //     "golang",
    //     "cpp",
    //     "java",
    //     "nodejs",
    //     "python",
    //     "rust"
    // };
    // // 创建QListWidgetItem，并设置自定义的widget
    // for(int i = 0; i < 13; i++){
    //     int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
    //     int str_i = randomValue%strs.size();
    //     int head_i = randomValue%heads.size();
    //     int name_i = randomValue%names.size();
    //     auto *chat_user_wid = new ChatUserWid();
    //     auto user_info = std::make_shared<UserInfo>(0, names[name_i], names[name_i], heads[head_i],0, strs[str_i]);
    //     chat_user_wid->SetInfo(user_info);
    //     QListWidgetItem *item = new QListWidgetItem;
    //     //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    //     item->setSizeHint(chat_user_wid->sizeHint());
    //     ui->chat_user_list->addItem(item);
    //     ui->chat_user_list->setItemWidget(item, chat_user_wid);
    // }
}

void ChatDialog::loadMoreChatUser()
{
    auto friend_list = UserMgr::GetInstance()->GetChatListPerPage();
    if(friend_list.empty() == false){
        for(auto& friend_ele : friend_list){
            auto find_iter = _chat_items_added.find(friend_ele->_uid);
            if(find_iter == _chat_items_added.end()){
                continue;
            }
            auto *chat_user_wid = new ChatUserWid();
            auto user_info = std::make_shared<UserInfo>(friend_ele);
            chat_user_wid->SetInfo(user_info);
            QListWidgetItem *item = new QListWidgetItem;
            //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
            item->setSizeHint(chat_user_wid->sizeHint());
            ui->chat_user_list->addItem(item);
            ui->chat_user_list->setItemWidget(item, chat_user_wid);
            _chat_items_added.insert(friend_ele->_uid, item);
        }
    }
    //更新已加载条目
    UserMgr::GetInstance()->UpdateChatLoadedCount();
}

void ChatDialog::loadMorrConUser()
{
    auto friend_list = UserMgr::GetInstance()->GetConListPrePage();
    if(friend_list.empty() == false){
        for(auto& friend_ele : friend_list){
            auto *chat_user_wid = new ConUserItem();
            chat_user_wid->SetInfo(friend_ele->_uid, friend_ele->_name, friend_ele->_icon);
            QListWidgetItem *item = new QListWidgetItem;
            //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
            item->setSizeHint(chat_user_wid->sizeHint());
            ui->con_user_list->addItem(item);
            ui->con_user_list->setItemWidget(item, chat_user_wid);
        }
    }
    //更新已加载条目
    UserMgr::GetInstance()->UpdateContactLoadedCount();
}

//表示当前聊天的条目在哪里
void ChatDialog::SetSelectChatItem(int uid)
{
    if(ui->chat_user_list->count() <= 0){
        return;
    }
    if(uid == 0){
        ui->chat_user_list->setCurrentRow(0);
        QListWidgetItem* firstItem = ui->chat_user_list->item(0);
        if(!firstItem){
            return;
        }
        //转化为 widget
        QWidget* widget = ui->chat_user_list->itemWidget(firstItem);
        if(!widget){
            return;
        }
        auto con_item = qobject_cast<ChatUserWid*>(widget);
        if(!con_item){
            return;
        }
        _cur_chat_uid = con_item->GetUserInfo()->_uid;
        return;
    }
}

void ChatDialog::SetSelectChatPage(int uid)
{
    if(ui->chat_user_list->count() <= 0){
        return;
    }
    if(uid == 0){
        auto item = ui->chat_user_list->item(0);
        //转换为 widget
        QWidget* widget = ui->chat_user_list->itemWidget(item);
        if(!widget){
            return;
        }
        auto con_item = qobject_cast<ChatUserWid*>(widget);
        if(!con_item){
            return;
        }
        auto user_info = con_item->GetUserInfo();
        ui->chat_page->SetUserInfo(user_info);
        return;
    }
    auto find_iter = _chat_items_added.find(uid);
    if(find_iter == _chat_items_added.end()){
        return;
    }
    QWidget* widget = ui->chat_user_list->itemWidget(find_iter.value());
    if(!widget){
        return;
    }
    ListItemBase* customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem){
        return;
    }
    auto itemType = customItem->GetItemType();
    if(itemType == ListItemType::CHAT_USER_ITEM){
        auto con_item = qobject_cast<ChatUserWid*>(customItem);
        if(!con_item){
            return;
        }
        auto user_info = con_item->GetUserInfo();
        ui->chat_page->SetUserInfo(user_info);
        return;
    }
}

void ChatDialog::UpdateChatMsg(std::vector<std::shared_ptr<TextChatData> > msgdata)
{
    for(auto& msg : msgdata){
        if(msg->_from_uid != _cur_chat_uid){
            break;
        }
        ui->chat_page->AppendChatMsg(msg);
    }
}

bool ChatDialog::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress){
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        handleGlobalMouseEvent(mouseEvent);
    }
    return QDialog::eventFilter(watched, event);
}

void ChatDialog::handleGlobalMouseEvent(QMouseEvent *event)
{
    if(_mode != ChatUIMode::SearchMode){
        return;
    }
    //将鼠标点击位置转换为搜索列表坐标系中的位置
    QPoint posInSearchList = ui->search_list->mapFromGlobal(event->globalPosition()).toPoint();
    //判断点击位置是否在聊天列表的范围内
    if(!ui->search_list->rect().contains(posInSearchList)){
        ui->search_edit->clear();
        ShowSearch(false);
    }
}

void ChatDialog::ClearLabelState(StateWidget *lb)
{
    for(auto & ele : _lb_list){
        if(ele == lb){
            continue;
        }
        ele->ClearState();
    }
}

void ChatDialog::AddLBGroup(StateWidget *lb)
{
    _lb_list.push_back(lb);
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
    }else if(_state == ChatUIMode::SearchMode){
        ui->chat_user_list->show();
        ui->con_user_list->hide();
        ui->search_list->hide();
        _mode = ChatUIMode::ChatMode;
    }
}

void ChatDialog::slot_loading_chat_user()
{
    auto b_load = UserMgr::GetInstance()->IsLoadChatFin();
    if(b_load) return;
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
        loadMoreChatUser();
        ui->chat_user_list->takeItem(ui->chat_user_list->row(item));
        ui->chat_user_list->update();
        _b_loading = false;
    });
}

void ChatDialog::slot_loading_con_user()
{
    auto b_load = UserMgr::GetInstance()->IsLoadConFin();
    if(b_load) return;
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
    ui->con_user_list->addItem(item);
    ui->con_user_list->setItemWidget(item, loading_item);
    movie->start();


    QTimer::singleShot(1000, this, [this, item](){
        qDebug() << " add new data to list ...";
        loadMoreChatUser();
        ui->con_user_list->takeItem(ui->con_user_list->row(item));
        ui->con_user_list->update();
        _b_loading = false;
    });
}

void ChatDialog::slot_side_chat()
{
    qDebug() << "receive side chat clicked";
    ClearLabelState(ui->side_chat_lb);
    ui->stackedWidget->setCurrentWidget(ui->chat_page);
    _state = ChatUIMode::ChatMode;
    ShowSearch(false);
}

void ChatDialog::slot_side_contact()
{
    qDebug() << "receive side contact clicked";
    ClearLabelState(ui->side_contact_lb);
    ui->stackedWidget->setCurrentWidget(ui->friend_apply_page);
    _state = ChatUIMode::ContactMode;
    ShowSearch(false);
}

void ChatDialog::slot_text_changed(const QString &str)
{
    if(!str.isEmpty()){
        ShowSearch(true);
    }
}

void ChatDialog::slot_apply_friend(std::shared_ptr<AddFriendApply> apply)
{
    qDebug() << "receive apply friend slot";
    //判断是否已经申请了
    bool b_already = UserMgr::GetInstance()->AlreadyApply(apply->_from_uid);
    if(b_already){
        return;
    }
    qDebug() << "未申请";
    UserMgr::GetInstance()->AddApplyList(std::make_shared<ApplyInfo>(apply));
    ui->side_contact_lb->ShowRedPoint(true);
    ui->con_user_list->ShowRedPoint(true);
    ui->friend_apply_page->AddNewApply(apply);
}

void ChatDialog::slot_add_auth_friend(std::shared_ptr<AuthInfo> auth_info)
{
    qDebug() << "slot add auth friend for chatdialog";
    bool isFriend = UserMgr::GetInstance()->CheckFriendById(auth_info->_uid);
    if(isFriend){
        return;
    }
    // 在 groupitem 之后插入新项
    // std::vector<QString>  strs ={"hello world !",
    //                              "nice to meet u",
    //                              "New year，new life",
    //                              "You have to love yourself",
    //                              "My love is written in the wind ever since the whole world is you"};
    // std::vector<QString> heads = {
    //     ":/img/head_1.jpg",
    //     ":/img/head_2.jpg",
    //     ":/img/head_3.jpg",
    //     ":/img/head_4.jpg",
    //     ":/img/head_5.jpg"
    // };
    // int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
    // int str_i = randomValue % strs.size();
    // int head_i = randomValue % heads.size();

    auto *chat_user_wid = new ChatUserWid();
    auto user_info = std::make_shared<UserInfo>(auth_info);
    chat_user_wid->SetInfo(user_info);
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);
    _chat_items_added.insert(auth_info->_uid, item);

}



void ChatDialog::slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp)
{
    qDebug() << "slot add auth friend for chatdialog";
    bool isFriend = UserMgr::GetInstance()->CheckFriendById(auth_rsp->_uid);
    if(isFriend){
        return;
    }
    // 在 groupitem 之后插入新项
    // std::vector<QString>  strs ={"hello world !",
    //                              "nice to meet u",
    //                              "New year，new life",
    //                              "You have to love yourself",
    //                              "My love is written in the wind ever since the whole world is you"};
    // std::vector<QString> heads = {
    //     ":/img/head_1.jpg",
    //     ":/img/head_2.jpg",
    //     ":/img/head_3.jpg",
    //     ":/img/head_4.jpg",
    //     ":/img/head_5.jpg"
    // };
    // int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
    // int str_i = randomValue % strs.size();
    // int head_i = randomValue % heads.size();

    auto *chat_user_wid = new ChatUserWid();
    auto user_info = std::make_shared<UserInfo>(auth_rsp);
    chat_user_wid->SetInfo(user_info);
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);
    _chat_items_added.insert(auth_rsp->_uid, item);
}

void ChatDialog::slot_jump_chat_item(std::shared_ptr<SearchInfo> si)
{
    qDebug() << "slog jump chat called";
    auto find_iter = _chat_items_added.find(si->_uid);
    if(find_iter != _chat_items_added.end()){
        qDebug() << "jump to chat item, uid is " << si->_uid;
        ui->chat_user_list->scrollToItem(find_iter.value());
        ui->side_chat_lb->SetSelected(true);
        SetSelectChatItem(si->_uid);
        SetSelectChatPage(si->_uid);
        slot_side_chat();
        return;
    }
    //如果没找到，则创建新的插入 listwidget
    auto *chat_user_wid = new ChatUserWid();
    auto user_info = std::make_shared<UserInfo>(si);
    chat_user_wid->SetInfo(user_info);
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);
    _chat_items_added.insert(si->_uid, item);

    ui->side_chat_lb->SetSelected(true);
    SetSelectChatItem(si->_uid);
    SetSelectChatPage(si->_uid);
    slot_side_chat();
    return;
}

void ChatDialog::slot_jump_chat_item_from_infopage(std::shared_ptr<UserInfo> si)
{
    qDebug() << "slog jump chat called";
    auto find_iter = _chat_items_added.find(si->_uid);
    if(find_iter != _chat_items_added.end()){
        qDebug() << "jump to chat item, uid is " << si->_uid;
        ui->chat_user_list->scrollToItem(find_iter.value());
        ui->side_chat_lb->SetSelected(true);
        SetSelectChatItem(si->_uid);
        SetSelectChatPage(si->_uid);
        slot_side_chat();
        return;
    }
    //如果没找到，则创建新的插入 listwidget
    auto *chat_user_wid = new ChatUserWid();
    auto user_info = si;
    chat_user_wid->SetInfo(user_info);
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(chat_user_wid->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);
    _chat_items_added.insert(si->_uid, item);

    ui->side_chat_lb->SetSelected(true);
    SetSelectChatItem(si->_uid);
    SetSelectChatPage(si->_uid);
    slot_side_chat();
    return;
}

void ChatDialog::slot_friend_info_page(std::shared_ptr<UserInfo> user_info)
{
    qDebug() << "receive switch friend into page sig";
    _last_widget = ui->friend_info_page;
    ui->stackedWidget->setCurrentWidget(ui->friend_info_page);
    ui->friend_info_page->SetInfo(user_info);
}

void ChatDialog::slot_switch_apply_friend_page()
{
    _last_widget = ui->friend_apply_page;
    ui->stackedWidget->setCurrentWidget(ui->friend_apply_page);
}

void ChatDialog::slot_item_clicked(QListWidgetItem *item)
{
    QWidget *widget = ui->chat_user_list->itemWidget(item);
    if(!widget){
        return;
    }
    ListItemBase *customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem){
        return;
    }
    auto itemType = customItem->GetItemType();
    if(itemType == ListItemType::INVALID_ITEM || itemType == ListItemType::GROUP_TIP_ITEM){
        return;
    }
    if(itemType == ListItemType::CHAT_USER_ITEM){
        auto chat_wid = qobject_cast<ChatUserWid*>(customItem);
        auto user_info = chat_wid->GetUserInfo();
        ui->chat_page->SetUserInfo(user_info);
        _cur_chat_uid = user_info->_uid;
        return;
    }
}

void ChatDialog::slot_append_send_chat_msg(std::shared_ptr<TextChatData> msgdata)
{
    if(_cur_chat_uid == 0){
        return;
    }
    auto find_iter = _chat_items_added.find(_cur_chat_uid);
    if(find_iter == _chat_items_added.end()){
        return;
    }
    QWidget *widget = ui->chat_user_list->itemWidget(find_iter.value());
    if(!widget){
        return;
    }
    ListItemBase* customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem){
        return;
    }
    auto itemType = customItem->GetItemType();
    if(itemType == ListItemType::CHAT_USER_ITEM){
        auto con_item = qobject_cast<ChatUserWid*>(customItem);
        if(!con_item){
            return;
        }
        auto user_info = con_item->GetUserInfo();
        user_info->_chat_msgs.push_back(msgdata);
        std::vector<std::shared_ptr<TextChatData>> msg_vec;
        msg_vec.push_back(msgdata);
        UserMgr::GetInstance()->AppendFriendChatMsg(_cur_chat_uid, msg_vec);
        return;
    }
}

void ChatDialog::slot_text_chat_msg(std::shared_ptr<TextChatMsg> msg)
{
    auto find_iter = _chat_items_added.find(msg->_from_uid);
    if(find_iter != _chat_items_added.end()){
        QWidget* widget = ui->chat_user_list->itemWidget(find_iter.value());
        auto chat_wid = qobject_cast<ChatUserWid*>(widget);
        if(!chat_wid){
            return;
        }
        chat_wid->updateLastMsg(msg->_chat_msgs);
        //更新当前聊天页面
        UpdateChatMsg(msg->_chat_msgs);
        UserMgr::GetInstance()->AppendFriendChatMsg(msg->_from_uid, msg->_chat_msgs);
    }
    //没找到
    auto* chat_user_wid = new ChatUserWid();
    auto fi_ptr = UserMgr::GetInstance()->GetFriendById(msg->_from_uid);
    chat_user_wid->SetInfo(std::make_shared<UserInfo>(fi_ptr));
    QListWidgetItem *item = new QListWidgetItem();
    item->setSizeHint(chat_user_wid->sizeHint());
    chat_user_wid->updateLastMsg(msg->_chat_msgs);
    UserMgr::GetInstance()->AppendFriendChatMsg(msg->_from_uid, msg->_chat_msgs);
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chat_user_wid);
    _chat_items_added.insert(msg->_from_uid, item);
}
