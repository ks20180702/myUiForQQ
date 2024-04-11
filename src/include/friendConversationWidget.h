#ifndef FRIENDCONVERSATIONWIDGET_H
#define FRIENDCONVERSATIONWIDGET_H

#include <QWidget>

#include "clientQQ.h"

namespace Ui {
class CFriendConversationWidget;
}

class CFriendConversationWidget : public QWidget
{
    Q_OBJECT
public slots:
    //点击确认是，发送消息过去
    void send_msg_in_conversation();

public:
    explicit CFriendConversationWidget(QWidget *parent = nullptr);
    ~CFriendConversationWidget();

    //设置客户端请求指针对象
    void set_main_client_ptr(std::shared_ptr<ClientQQ> mainClientPtr);

    //设置当前用户对象
    void set_current_user(CUser &currentUser);


    //设置好友的一些信息和是否在线情况
    void set_friend_label_info(CUser &friendUser,bool isOnline,std::map<int,std::vector<CMsg>> &msgsPartAccountMap);

    //当有新消息过来时，增加一下新的消息
    void show_new_msg(std::map<int,std::vector<CMsg>> &msgsPartAccountMap);
private:
    Ui::CFriendConversationWidget *ui;

    std::shared_ptr<ClientQQ> _mainClientPtr;

    //当前用户和当前的好友
    CUser _currentUser,_friendUser;
};

#endif // FRIENDCONVERSATIONWIDGET_H
