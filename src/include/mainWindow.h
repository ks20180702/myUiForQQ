#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qDebug>

#include "clientQQ.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CMainWindow : public QMainWindow
{
    Q_OBJECT

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

private:
    Ui::MainWindow *ui;

    std::shared_ptr<ClientQQ> _mainClientPtr;


    //当前用户
    CUser _currentUser;
    //好友列表
    std::vector<CUser> _friendLists;
    //好友请求列表
    std::vector<CUser> _requestUserLists;
    //未读消息列表
    std::vector<CMsg> _notRecvMsgsLists;
};
#endif // MAINWINDOW_H
