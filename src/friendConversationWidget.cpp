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
    ui->TextEShowMsg->append(myInputMsg);
    ui->TextEShowMsg->setAlignment(Qt::AlignRight);
    ui->TextEWriteMsg->clear();

    CMsg sendMsg(_currentUser.get_id(),_friendUser.get_id(),CurrentDate(),myInputMsg.toStdString());
    _mainClientPtr->send_data_msg_cmd(_friendUser,sendMsg,CDataMsgCmd::MSG_SEND);
}
void CFriendConversationWidget::set_friend_label_info(CUser &friendUser,bool isOnline,std::map<int,std::vector<CMsg>> &msgsPartAccountMap)
{
    _friendUser=friendUser;

    ui->LabelNameValue->setText(_Q(_friendUser.get_name()));
    ui->LabelAccountValue->setText(_Q(_friendUser.get_account()));
    ui->LabelAgeValue->setText(QString::number(_friendUser.get_age()));
    if(isOnline) {ui->LabelOnlineValue->setText(_Q_U("在线"));}
    else {ui->LabelOnlineValue->setText(_Q_U("离线"));}

    //加载未读消息，并将已读信息发送给服务器端
    ui->TextEShowMsg->clear();
    show_new_msg(msgsPartAccountMap);

}
void CFriendConversationWidget::show_new_msg(std::map<int,std::vector<CMsg>> &msgsPartAccountMap)
{
    //如果对话框是隐藏的，那不用更新最新的消息
    if(isHidden()) return;

    std::map<int,std::vector<CMsg>>::iterator itUserMsg;
    std::vector<CMsg> tempNewMsg;
    //加锁
    std::unique_lock<std::mutex> msgListsLock(mtxMsgLists);
    itUserMsg=msgsPartAccountMap.find(_friendUser.get_id());
    if(itUserMsg==msgsPartAccountMap.end()) {
        msgListsLock.unlock();
        return ;
    }
    else{
        tempNewMsg=itUserMsg->second;
    }
    msgListsLock.unlock();

    //显示改到左边，增加消息显示
    ui->TextEShowMsg->append(QString(""));
    ui->TextEShowMsg->setAlignment(Qt::AlignLeft);
    for(std::vector<CMsg>::iterator itMsg=tempNewMsg.begin();itMsg!=tempNewMsg.end();itMsg++)
    {
        ui->TextEShowMsg->append((*itMsg).get_content());
    }

    //发送消息已读给服务器端
    CMsg readMsgOver(_friendUser.get_id(),_currentUser.get_id(),"","");
    _mainClientPtr->send_data_msg_cmd(_friendUser,readMsgOver,CDataMsgCmd::MSG_CONFIRM);
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
