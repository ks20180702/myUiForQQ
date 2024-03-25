#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "Logindialog.h"
#include <iostream>


#include "cmdBase.h"
#include "loginCmd.h"
#include "userChangeCmd.h"


#include "clientQQ.h"
#include "qt_clip.h"



CMainWindow::CMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    run_client();
//    std::cout<<"main +++"<<std::endl;
}

void CMainWindow::run_client()
{
    std::cout<<"main +++"<<std::endl;

    ClientQQ myCli;
    //myCli.client_init("192.168.47.135");
    myCli.client_init("114.55.229.106");
    myCli.select_init() ;

    myCli.run();
    std::cout << myCli.get_error() << std::endl;

    std::cout<<"main over"<<std::endl;

}
CMainWindow::~CMainWindow()
{
    delete ui;
}

