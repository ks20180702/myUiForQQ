#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

#include "clientQQ.h"

namespace Ui {
class LoginDialog;
}

class CLoginDialog : public QDialog
{
    Q_OBJECT

public slots:
    void user_verify();

public:
    explicit CLoginDialog(QWidget *parent = nullptr);
    ~CLoginDialog();

    //设置客户端请求指针对象
    void set_main_client_ptr(std::shared_ptr<ClientQQ> mainClientPtr);

    std::shared_ptr<CLoginCmd>& get_login_cmd_ptr();

private:
    Ui::LoginDialog *ui;

    std::shared_ptr<ClientQQ> _mainClientPtr;

    std::shared_ptr<CLoginCmd> _loginCmdPtr;
};

#endif // LOGINDIALOG_H
