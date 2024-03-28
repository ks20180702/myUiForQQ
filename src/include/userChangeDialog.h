#ifndef USERCHANGEDIALOG_H
#define USERCHANGEDIALOG_H

#include <QDialog>

namespace Ui {
class CUserChangeDialog;
}

class CUserChangeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CUserChangeDialog(QWidget *parent = nullptr);
    ~CUserChangeDialog();

private:
    Ui::CUserChangeDialog *ui;
};

#endif // USERCHANGEDIALOG_H
