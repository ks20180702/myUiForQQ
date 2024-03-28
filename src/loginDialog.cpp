#include "loginDialog.h"
#include "./ui_loginDialog.h"

#include "qt_clip.h"

#include <QDebug>
#include <QMessageBox>

CLoginDialog::CLoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    connect(ui->BtnOK,SIGNAL(clicked()),this,SLOT(user_verify()));
    connect(ui->BtnCancle,SIGNAL(clicked()),this,SLOT(close()));
}
void CLoginDialog::set_main_client_ptr(std::shared_ptr<ClientQQ> mainClientPtr)
{
    _mainClientPtr=mainClientPtr;
}

void CLoginDialog::user_verify()
{
    std::list<std::shared_ptr<CmdBase>> &cmdPtrLists=_mainClientPtr->get_cmd_ptr_lists();
    std::list<std::shared_ptr<CmdBase>>::iterator itNow;
    bool isWait=false,isSucceed=false;

    while(1)
    {
        if(isWait==false)
        {
            QString accountValue=ui->LineEAccountValue->text();
            QString pwdValue=ui->LineEPwdValue->text();

            //����¼�û����͵���������
            CUser loginUser(accountValue.toStdString(),pwdValue.toStdString());
            if(_mainClientPtr->send_login_cmd(loginUser)==-1)
            {
                std::cout << _mainClientPtr->get_error() << std::endl;
            }
            Sleep(1000);
            isWait=true;
            ui->BtnOK->setEnabled(false);
        }
        for( itNow=cmdPtrLists.begin();itNow!=cmdPtrLists.end();itNow++)
        {
            if((*itNow)->_childCmdType==CmdBase::LOGIN_CMD)
            {
                if((*itNow)->_childDoCommandReturn==false)
                {
                    QMessageBox::critical(this,_Q_U("����"),_Q_U("�˺��������"));
                    isWait=false;
                    ui->BtnOK->setEnabled(true);
                    break;
                }
                isSucceed=true;
                break;
            }
        }
        //����¼ָ�����ֵ��_loginCmdPtr����������ָ���clientδ����list��ɾ��
        if(isSucceed)
        {
            _loginCmdPtr =std::dynamic_pointer_cast<CLoginCmd>(*(itNow));
            cmdPtrLists.erase(itNow);
            close();
            return;
        }
        //���յ��˴�����Ϣ��Ҳ�˳�ȥ�������岻��
        if(isWait==false)
        {
            cmdPtrLists.erase(itNow);
            return;
        }
    }
}
std::shared_ptr<CLoginCmd>& CLoginDialog::get_login_cmd_ptr()
{
    return _loginCmdPtr;
}
CLoginDialog::~CLoginDialog()
{
    delete ui;
}