#include "friendConversationWidget.h"
#include "./ui_friendConversationWidget.h"

#include <QDebug>

CFriendConversationWidget::CFriendConversationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFriendConversationWidget)
{
    ui->setupUi(this);
}

//void CFriendConversationWidget::closeEvent ( QCloseEvent * e )
//{
//    emit close_now();
//    qDebug()<<"[close] now ++";
////    e->ignore();
//}

CFriendConversationWidget::~CFriendConversationWidget()
{
    delete ui;
}
