#include "mainWindow.h"
#include "friendConversationWidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CMainWindow w;
//    CFriendConversationWidget w;
    w.show();
    return a.exec();
}
