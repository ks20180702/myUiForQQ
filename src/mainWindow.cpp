#include "mainWindow.h"
#include "./ui_mainWindow.h"
#include "loginDialog.h"
#include <iostream>


#include "cmdBase.h"
#include "loginCmd.h"
#include "userChangeCmd.h"

#include "qt_clip.h"

#include <QMessageBox>

CMainWindow::CMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,_mainClientPtr(nullptr)
{
    if(recv_thread_init()==-1){return;}

    ui->setupUi(this);

    //初始化登录界面，并进行验证
    CLoginDialog loginDig(nullptr);
    loginDig.setWindowTitle(_Q_U("请输入账号密码"));
    loginDig.set_main_client_ptr(_mainClientPtr);
    loginDig.exec();
    std::shared_ptr<CLoginCmd> loginCmdPtr=loginDig.get_login_cmd_ptr();
    loginCmdPtr->show_do_command_info();
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
CMainWindow::~CMainWindow()
{
    delete ui;
}

