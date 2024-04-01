#ifndef FRIENDCONVERSATIONWIDGET_H
#define FRIENDCONVERSATIONWIDGET_H

#include <QWidget>
#include <QCloseEvent>

namespace Ui {
class CFriendConversationWidget;
}

class CFriendConversationWidget : public QWidget
{
    Q_OBJECT

signals:
    void close_now();

public:
    explicit CFriendConversationWidget(QWidget *parent = nullptr);
    ~CFriendConversationWidget();

//    void closeEvent ( QCloseEvent * e );

private:
    Ui::CFriendConversationWidget *ui;
};

#endif // FRIENDCONVERSATIONWIDGET_H
