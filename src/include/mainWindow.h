﻿#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractItemModel>

#include "friendConversationWidget.h"

#include "clientQQ.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum MODEL_TYPE
    {
        FRIEND_INFO,
        FRIEND_REQUEST
    };
public slots:
    void get_friend_info(const QModelIndex &index);

public:
    CMainWindow(QWidget *parent = nullptr);

    ~CMainWindow();

    //初始化客户端，并启动客户端中接收指令字节流的线程
    int recv_thread_init();

    //启动处理接收到的指令的线程(处理对实时性要求不高的,如心跳包的好友申请和新好友,好友未读消息))
    void deal_cmd_thread_init();
    //处理的具体执行
    void thread_deal_recv_cmd();

    /*
     * 初始化用户好友相关信息(好友信息，好友申请，好友未读信息)
     */
    int user_friends_init();

    void change_list_view_model(MODEL_TYPE modelNum);

private:
    Ui::MainWindow *ui;

    CFriendConversationWidget *_myConversation;

    std::shared_ptr<ClientQQ> _mainClientPtr;


    //当前用户
    CUser _currentUser;
    //好友列表
    //如果分多组的化(好友、家人)，应该外面再增加一个vector,这里暂时
    std::vector<CUser> _friendLists;
    //好友请求列表
    std::vector<CUser> _requestUserLists;
    //未读消息列表
    std::map<int,std::vector<CMsg>> _msgsPartAccountMap;

    std::vector<QAbstractItemModel*> _treeViewModelLists;
};
#endif // MAINWINDOW_H
