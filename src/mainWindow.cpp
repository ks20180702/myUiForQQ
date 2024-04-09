#include "mainWindow.h"
#include "./ui_mainWindow.h"
#include "loginDialog.h"
#include <iostream>


#include "cmdBase.h"
#include "loginCmd.h"
#include "userChangeCmd.h"

#include "qt_clip.h"

#include <QMessageBox>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QVariant>
#include <QPushButton>
#include <qDebug>


CMainWindow::CMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,_myConversation(nullptr)
    ,_mainClientPtr(nullptr)
{
    //这里三个注册我没弄懂，再信号和槽的部分，我好友没有传递非注册对象啊
    qRegisterMetaType<QList<QPersistentModelIndex>>("QList<QPersistentModelIndex>");
    qRegisterMetaType<QVector<int>>("QVector<int>");
    qRegisterMetaType<QAbstractItemModel::LayoutChangeHint>("QAbstractItemModel::LayoutChangeHint");

    if(recv_thread_init()==-1){return;}

    ui->setupUi(this);

    //初始化登录界面，并进行验证
    CLoginDialog loginDig(nullptr);
    loginDig.setWindowTitle(_Q_U("请输入账号密码"));
    loginDig.set_main_client_ptr(_mainClientPtr);
    loginDig.exec();
    std::shared_ptr<CLoginCmd> loginCmdPtr=loginDig.get_login_cmd_ptr();
//    loginCmdPtr->show_do_command_info();
    _currentUser=loginCmdPtr->get_login_user();
    _friendLists=loginCmdPtr->get_friend_lists();
    _msgsPartAccountMap=loginCmdPtr->get_msg_part_account_map();

    //启动心跳发送线程
//    _mainClientPtr->heart_thread_init(_currentUser);

    //启动指令处理线程(将接收到的指令数据更新到对应对象中)
    deal_cmd_thread_init();


    //设置初始的好友列表和好友申请列表
    user_friends_init();
    connect(ui->BtnUserFriendInfoView,&QPushButton::clicked,this,[&](){
        change_list_view_model(FRIEND_INFO);
    });
    connect(ui->BtnFriendRequestView,&QPushButton::clicked,this,[&](){
        change_list_view_model(FRIEND_REQUEST);
    });
    ui->TreeViewFriendList->setHeaderHidden(true);
    ui->TreeViewFriendList->setModel(_treeViewModelLists[0]);


    //初始化对话框
    _myConversation=new CFriendConversationWidget(nullptr);
    _myConversation->set_current_user(_currentUser);
    _myConversation->set_main_client_ptr(_mainClientPtr);
}

int CMainWindow::recv_thread_init()
{
    //初始化客户端，开启接收线程，开始等待接收服务器的数据
    _mainClientPtr=std::make_shared<ClientQQ>();
    if(_mainClientPtr->client_init(SERVER_ADDR)==-1)
    {
        QMessageBox::critical(this,_Q("客户端错误"),_Q("连接服务器失败"));
        return -1;
    }

    std::cout << _mainClientPtr->get_error() << std::endl;

    std::cout<<"recv thread is runing"<<std::endl;
    return 0;
}

void CMainWindow::deal_cmd_thread_init()
{
    std::thread dealCmdThread(&CMainWindow::thread_deal_recv_cmd,this);
    dealCmdThread.detach();
}
void CMainWindow::thread_deal_recv_cmd()
{
    std::list<std::shared_ptr<CmdBase>> &cmdPtrLists=_mainClientPtr->get_cmd_ptr_lists();
    std::list<std::shared_ptr<CmdBase>>::iterator itNow;
    std::vector<std::list<std::shared_ptr<CmdBase>>::iterator> delIteratorLists;
    std::vector<std::list<std::shared_ptr<CmdBase>>::iterator>::iterator itDel;
    std::shared_ptr<CHeartRequestCmd> heartCmdPtr;
    std::shared_ptr<CHeartMsgCmd> heartMsgCmdPtr;

    //对cmdPtrLists的互斥锁
    std::unique_lock<std::mutex> cmdPtrLock(mtx);
    cmdPtrLock.unlock();

    //5秒处理一次
    //只处理heartRequestCmd和dataMsgCmd
    //用户信息修改，需要等待是否修改成功，所以放在这里需要等比较久
    while(1)
    {
        Sleep(2000);

        cmdPtrLock.lock();
        for( itNow=cmdPtrLists.begin();itNow!=cmdPtrLists.end();itNow++)
        {
            if((*itNow)->_childCmdType==CmdBase::HEART_CMD)
            {
                delIteratorLists.push_back(itNow);

                if((*itNow)->_childDoCommandReturn==false)
                {
                    qDebug()<<"[E]  heart cmd return a error, pleace check";
                    continue;
                }
                qDebug()<<"[I] get a new heart cmd";
                heartCmdPtr =std::dynamic_pointer_cast<CHeartRequestCmd>(*(itNow));

                //防止遍历的时候，突然修改好友列表
                std::unique_lock<std::mutex> friendListsLock(mtxFriendLists);
                _friendLists=heartCmdPtr->get_friend_lists();
                _requestUserLists=heartCmdPtr->get_friendship_request_lists();
//                _msgsPartAccountMap=heartCmdPtr->get_msg_part_account_map();
                friendListsLock.unlock();

                user_friends_init();
            }
            if((*itNow)->_childCmdType==CmdBase::HEART_MSG_CMD)
            {
                delIteratorLists.push_back(itNow);

                if((*itNow)->_childDoCommandReturn==false)
                {
                    qDebug()<<"[E]  heart msg cmd return a error, pleace check";
                    continue;
                }
                heartMsgCmdPtr =std::dynamic_pointer_cast<CHeartMsgCmd>(*(itNow));
                std::unique_lock<std::mutex> friendListsLock(mtxMsgLists);
                _msgsPartAccountMap=heartMsgCmdPtr->get_msg_part_account_map();
                friendListsLock.unlock();
                qDebug()<<"[I]  reset _msgsPartAccountMap is over";
            }

        }
        for(itDel=delIteratorLists.begin();itDel!=delIteratorLists.end();itDel++)
        {
            cmdPtrLists.erase(*itDel);
        }
        //清空需删除数据
        delIteratorLists.clear();
        itDel=delIteratorLists.begin();

        cmdPtrLock.unlock();
        //当好友列表和好友申请有变化是，更新treeView
        Sleep(3000);
    }
}

int CMainWindow::user_friends_init()
{
    QStandardItemModel *modelFriendInfo;
    QStringListModel *modelFriendRequest;
    QStandardItem *friendRowLvOne;

    if(_treeViewModelLists.size()==0)
    {
        modelFriendInfo= new QStandardItemModel(ui->TreeViewFriendList);
        modelFriendRequest=new QStringListModel(ui->TreeViewFriendList);
        _treeViewModelLists.push_back((QAbstractItemModel*)modelFriendInfo);
        _treeViewModelLists.push_back((QAbstractItemModel*)modelFriendRequest);
        //暂定只有一个分组，后面看情况增加
        friendRowLvOne=new QStandardItem(_Q_U("好友"));
        modelFriendInfo->appendRow(friendRowLvOne);
        friendRowLvOne->setData(1);

        connect(ui->TreeViewFriendList, SIGNAL(clicked(QModelIndex)),
                this,SLOT(get_friend_info(QModelIndex)));
    }
    else if(_treeViewModelLists.size()==2)
    {
        modelFriendInfo=(QStandardItemModel*)_treeViewModelLists[0];
        modelFriendRequest=(QStringListModel*)_treeViewModelLists[1];

        //删除所有子类数据和内存，重新赋值
        friendRowLvOne=modelFriendInfo->item(0);
        qDebug()<<friendRowLvOne->text();
        friendRowLvOne->removeRows(0,friendRowLvOne->rowCount());
        for(int i=0;i<friendRowLvOne->rowCount();i++)
        {
            QStandardItem* tempDelItem=friendRowLvOne->child(i);
            if(tempDelItem)
            {
                delete tempDelItem;
            }
        }
    }

    //加锁
    std::unique_lock<std::mutex> friendListsLock(mtxFriendLists);
    for(int i=0;i<_friendLists.size();i++)
    {
        QStandardItem *friendRow=new QStandardItem(_Q(_friendLists[i].get_name()));
        friendRowLvOne->appendRow(friendRow);
        //好友从100开始加，家人从200开始，类推(也可从1000开始)，分组暂时不想弄
        friendRow->setData(100+i);
    }

    //好友申请模式
    //可以考虑使用QAbstractListModel，自定义一个userListModel等
    QStringList theStrList; //保存初始 StringList
    for(int i=0;i<_requestUserLists.size();i++)
    {
        theStrList<<_Q(_requestUserLists[i].get_name());
    }
    modelFriendRequest->setStringList(theStrList); //为模型设置StringList，会导入StringList的内容
    friendListsLock.unlock();

    return 0;
}

void CMainWindow::change_list_view_model(MODEL_TYPE modelNum)
{
    ui->TreeViewFriendList->setModel(_treeViewModelLists[modelNum]);
}
void CMainWindow::get_friend_info(const QModelIndex &index)
{

    int irow=index.row();
    int levelNum=index.data(Qt::UserRole+1).toInt();
    //一级标题，点击，展开，没有收缩功能
    if(levelNum<100)
    {
        ui->TreeViewFriendList->expand(index);
        return;
    }

    CUser friendUser=_friendLists[irow];
    _myConversation->set_friend_label_info(friendUser,true);
    _myConversation->show();
}
CMainWindow::~CMainWindow()
{
    delete ui;
    delete  _myConversation;
}

