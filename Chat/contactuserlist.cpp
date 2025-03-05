#include "contactuserlist.h"
#include "tcpmgr.h"
#include <QRandomGenerator>
#include <QScrollBar>
#include "userdata.h"
#include "usermgr.h"

ContactUserList::ContactUserList(QWidget *parent)
{
    Q_UNUSED(parent);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //安装事件过滤器
    this->viewport()->installEventFilter(this);

    //模拟从后端传输过来的数据，进行列表加载
    addContactUserList();

    connect(this, &QListWidget::itemClicked, this, &ContactUserList::slot_item_clicked);
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_add_auth_friend, this,
            &ContactUserList::slot_add_auth_friend);
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_auth_rsp, this, &ContactUserList::slot_auth_rsp);
}

void ContactUserList::ShowRedPoint(bool bshow)
{
    _add_friend_item->ShowRedPoint(bshow);
}

bool ContactUserList::eventFilter(QObject *watched, QEvent *event)
{
    // 检查事件是否是鼠标悬浮进入或离开
    if (watched == this->viewport()) {
        if (event->type() == QEvent::Enter) {
            // 鼠标悬浮，显示滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        } else if (event->type() == QEvent::Leave) {
            // 鼠标离开，隐藏滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }
    // 检查事件是否是鼠标滚轮事件
    if (watched == this->viewport() && event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        int numDegrees = wheelEvent->angleDelta().y() / 8;
        int numSteps = numDegrees / 15; // 计算滚动步数
        // 设置滚动幅度
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);
        // 检查是否滚动到底部
        QScrollBar *scrollBar = this->verticalScrollBar();
        int maxScrollValue = scrollBar->maximum();
        int currentValue = scrollBar->value();
        //int pageSize = 10; // 每页加载的联系人数量
        if (maxScrollValue - currentValue <= 0) {
            // 滚动到底部，加载新的联系人
            qDebug()<<"load more contact user";
            //发送信号通知聊天界面加载更多聊天内容
            emit sig_loading_contact_user();
        }
        return true; // 停止事件传递
    }
    return QListWidget::eventFilter(watched, event);
}

void ContactUserList::addContactUserList()
{
    auto * groupTip = new GroupTipItem();
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(groupTip->sizeHint());
    this->addItem(item);
    this->setItemWidget(item, groupTip);
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);

    //往自定义list中添加item
    _add_friend_item = new ConUserItem();
    _add_friend_item->setObjectName("new_friend_item");
    _add_friend_item->SetInfo(0,tr("新的朋友"),":/img/add_friend.png");
    _add_friend_item->SetItemType(ListItemType::APPLY_FRIEND_ITEM);
    QListWidgetItem *add_item = new QListWidgetItem;
    //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    add_item->setSizeHint(_add_friend_item->sizeHint());
    this->addItem(add_item);
    //先加一个原始的item，然后进行替换
    this->setItemWidget(add_item, _add_friend_item);


    //默认设置新的朋友申请条目被选中
    this->setCurrentItem(add_item);

    auto * groupCon = new GroupTipItem();
    groupCon->SetGroupTip(tr("联系人"));
    _groupitem = new QListWidgetItem;
    _groupitem->setSizeHint(groupCon->sizeHint());
    this->addItem(_groupitem);
    this->setItemWidget(_groupitem, groupCon);
    _groupitem->setFlags(_groupitem->flags() & ~Qt::ItemIsSelectable);

    //加载
    auto con_list = UserMgr::GetInstance()->GetConListPrePage();
    for(auto& con_ele : con_list){
        auto *con_user_wid = new ConUserItem();
        con_user_wid->SetInfo(con_ele->_uid, con_ele->_name, con_ele->_icon);
        QListWidgetItem *item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(con_user_wid->sizeHint());
        this->addItem(item);
        this->setItemWidget(item, con_user_wid);
    }
    UserMgr::GetInstance()->UpdateContactLoadedCount();
    // 创建QListWidgetItem，并设置自定义的widget
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
    // for(int i = 0; i < 13; i++){
    //     int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
    //     int str_i = randomValue % strs.size();
    //     int head_i = randomValue % heads.size();
    //     int name_i = randomValue % names.size();
    //     auto *con_user_wid = new ConUserItem();
    //     con_user_wid->SetInfo(0,names[name_i], heads[head_i]);
    //     QListWidgetItem *item = new QListWidgetItem;
    //     //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
    //     item->setSizeHint(con_user_wid->sizeHint());
    //     this->addItem(item);
    //     this->setItemWidget(item, con_user_wid);
    // }
}

void ContactUserList::slot_item_clicked(QListWidgetItem *item)
{
    QWidget *widget = this->itemWidget(item); // 获取自定义widget对象
    if(!widget){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }
    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase *customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }
    auto itemType = customItem->GetItemType();
    if(itemType == ListItemType::INVALID_ITEM
        || itemType == ListItemType::GROUP_TIP_ITEM){
        qDebug()<< "slot invalid item clicked ";
        return;
    }
    if(itemType == ListItemType::APPLY_FRIEND_ITEM){
        // 创建对话框，提示用户
        qDebug()<< "apply friend item clicked ";
        //跳转到好友申请界面
        emit sig_switch_apply_friend_page();
        return;
    }
    if(itemType == ListItemType::CONTACT_USER_ITEM){
        // 创建对话框，提示用户
        qDebug()<< "contact user item clicked ";
        //跳转到好友申请界面
        auto con_item = qobject_cast<ConUserItem*>(customItem);
        auto user_info = con_item->GetInfo();
        emit sig_switch_friend_info_page(user_info);
        return;
    }
}

void ContactUserList::slot_add_auth_friend(std::shared_ptr<AuthInfo> auth_info)
{
    qDebug() << "slot add auth friend";
    bool isFriend = UserMgr::GetInstance()->CheckFriendById(auth_info->_uid);
    if(isFriend){
        return;
    }
    UserMgr::GetInstance()->AddFriend(auth_info);

    auto *con_user_wid = new ConUserItem();
    con_user_wid->SetInfo(auth_info);
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(con_user_wid->sizeHint());
    //获取 groupitem 的索引
    int index = this->row(_groupitem);
    //在 groupitem 后面插入新项
    this->insertItem(index + 1, item);
    this->setItemWidget(item, con_user_wid);
}

void ContactUserList::slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp)
{
    qDebug() << "slot auth rsp called";
    bool isFriend = UserMgr::GetInstance()->CheckFriendById(auth_rsp->_uid);
    if(isFriend){
        return;
    }
    UserMgr::GetInstance()->AddFriend(auth_rsp);
    auto *con_user_wid = new ConUserItem();
    con_user_wid->SetInfo(auth_rsp->_uid, auth_rsp->_name, auth_rsp->_icon);
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(con_user_wid->sizeHint());
    //获取 groupitem 的索引
    int index = this->row(_groupitem);
    //在 groupitem 后面插入新项
    this->insertItem(index + 1, item);
    this->setItemWidget(item, con_user_wid);
}

