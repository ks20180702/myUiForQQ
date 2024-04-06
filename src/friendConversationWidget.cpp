#include "friendConversationWidget.h"
#include "./ui_friendConversationWidget.h"

#include <QDebug>
#include <QString>

#include "qt_clip.h"
#include "other_funcation.h"

CFriendConversationWidget::CFriendConversationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CFriendConversationWidget)
{
    ui->setupUi(this);

    ui->TextEShowMsg->setReadOnly(true);

    connect(ui->BtnSendMsg,SIGNAL(clicked()),this,SLOT(send_msg_in_conversation()));
}
void CFriendConversationWidget::send_msg_in_conversation()
{
    QString myInputMsg=ui->TextEWriteMsg->toPlainText();
    if(myInputMsg=="") {return ;};

    //发送的一下处理(显示在右边，清空输入的内容，发送)
    ui->TextEShowMsg->setAlignment(Qt::AlignRight);
    ui->TextEShowMsg->append(myInputMsg);
    ui->TextEWriteMsg->clear();

    CMsg sendMsg(_currentUser.get_id(),_friendUser.get_id(),CurrentDate(),myInputMsg.toStdString());
    _mainClientPtr->send_data_msg_cmd(_friendUser,sendMsg,CDataMsgCmd::MSG_SEND);
}
void CFriendConversationWidget::set_friend_label_info(CUser &friendUser,bool isOnline)
{
    _friendUser=friendUser;

    ui->LabelNameValue->setText(_Q(_friendUser.get_name()));
    ui->LabelAccountValue->setText(_Q(_friendUser.get_account()));
    ui->LabelAgeValue->setText(QString::number(_friendUser.get_age()));
    if(isOnline) {ui->LabelOnlineValue->setText(_Q_U("在线"));}
    else {ui->LabelOnlineValue->setText(_Q_U("离线"));}
}

void CFriendConversationWidget::set_current_user(CUser &currentUser)
{
    _currentUser=currentUser;
}
void CFriendConversationWidget::set_main_client_ptr(std::shared_ptr<ClientQQ> mainClientPtr)
{
    _mainClientPtr=mainClientPtr;
}
CFriendConversationWidget::~CFriendConversationWidget()
{
    delete ui;
}
