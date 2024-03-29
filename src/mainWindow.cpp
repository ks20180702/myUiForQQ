﻿#include "mainWindow.h"
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
    ,_mainClientPtr(nullptr)
{
    if(recv_thread_init()==-1){return;}

    ui->setupUi(this);

    //初始化登录界面，并进行验证
//    CLoginDialog loginDig(nullptr);
//    loginDig.setWindowTitle(_Q_U("请输入账号密码"));
//    loginDig.set_main_client_ptr(_mainClientPtr);
//    loginDig.exec();
//    std::shared_ptr<CLoginCmd> loginCmdPtr=loginDig.get_login_cmd_ptr();
//    loginCmdPtr->show_do_command_info();
//    _currentUser=loginCmdPtr->get_login_user();
//    _friendLists=loginCmdPtr->get_friend_lists();
//    _notRecvMsgsLists=loginCmdPtr->get_not_recv_msg_lists();

//    //启动心跳发送线程
//    _mainClientPtr->heart_thread_init(_currentUser);

//    //启动指令处理线程(将接收到的指令数据更新到对应对象中)
//    deal_cmd_thread_init();

    user_friends_init();

    connect(ui->BtnUserFriendInfoView,&QPushButton::clicked,this,[&](){
        change_list_view_model(FRIEND_INFO);
    });
    connect(ui->BtnFriendRequestView,&QPushButton::clicked,this,[&](){
        change_list_view_model(FRIEND_REQUEST);
    });
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
    //5秒处理一次
    //只处理heartRequestCmd和dataMsgCmd
    //用户信息修改，需要等待是否修改成功，所以放在这里需要等比较久
    while(1)
    {
        Sleep(1000);
        //这里需要上一个读锁
        for( itNow=cmdPtrLists.begin();itNow!=cmdPtrLists.end();itNow++)
        {
            if((*itNow)->_childCmdType==CmdBase::HEART_CMD)
            {
                if((*itNow)->_childDoCommandReturn==false)
                {
                    qDebug()<<"[E]  heart cmd return a error, pleace check";
                }
                qDebug()<<"[I] get a new heart cmd";
                heartCmdPtr =std::dynamic_pointer_cast<CHeartRequestCmd>(*(itNow));
                _friendLists=heartCmdPtr->get_friend_lists();
                _requestUserLists=heartCmdPtr->get_friendship_request_lists();
                delIteratorLists.push_back(itNow);
            }
//            else if((*itNow)->_childCmdType==CmdBase::DTAT_MSG_CMD)
//            {
//                if((*itNow)->_childDoCommandReturn==false)
//                {
//                    QMessageBox::critical(this,_Q_U("错误"),_Q_U("账号密码错误"));
//                }
//                break;
//            }
        }
        //这里加一个写锁，删除list中的数据
        for(itDel=delIteratorLists.begin();itDel!=delIteratorLists.end();itDel++)
        {
            cmdPtrLists.erase(*itDel);
        }
        //清空需删除数据
        delIteratorLists.clear();
        itDel=delIteratorLists.begin();
        Sleep(3000);
    }
}

int CMainWindow::user_friends_init()
{
    ui->TreeViewFriendList->setHeaderHidden(true);
    //好友列表信息模式
    QStandardItemModel *modelFriendInfo = new QStandardItemModel(ui->TreeViewFriendList);

    modelFriendInfo->setItem(0,0,new QStandardItem(_Q_U("好友")));
    modelFriendInfo->item(0,0)->setChild(0,0,new QStandardItem(_Q_U("好友1")));
    modelFriendInfo->item(0,0)->setChild(1,0,new QStandardItem(_Q_U("好友2")));
    modelFriendInfo->setItem(1,0,new QStandardItem(_Q_U("家人")));
    modelFriendInfo->item(1,0)->setChild(0,0,new QStandardItem(_Q_U("家人1")));

    //好友申请模式
    //可以考虑使用QAbstractListModel，自定义一个userListModel等
    QStringListModel *modelFriendRequest=new QStringListModel(ui->TreeViewFriendList);

    QStringList theStrList; //保存初始 StringList
    theStrList<<_Q_U("北京")<<_Q_U("上海")<<_Q_U("天津"); //初始化 StringList
    modelFriendRequest->setStringList(theStrList); //为模型设置StringList，会导入StringList的内容

//    ui->listView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
    _treeViewModelLists.push_back((QAbstractItemModel*)modelFriendInfo);
    _treeViewModelLists.push_back((QAbstractItemModel*)modelFriendRequest);

    ui->TreeViewFriendList->setModel(_treeViewModelLists[0]);
    return 0;
}

void CMainWindow::change_list_view_model(MODEL_TYPE modelNum)
{
    ui->TreeViewFriendList->setModel(_treeViewModelLists[modelNum]);
}
CMainWindow::~CMainWindow()
{
    delete ui;
}

