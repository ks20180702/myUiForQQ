#include "userChangeDialog.h"
#include "./ui_userChangeDialog.h"

CUserChangeDialog::CUserChangeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CUserChangeDialog)
{
    ui->setupUi(this);
}

CUserChangeDialog::~CUserChangeDialog()
{
    delete ui;
}
