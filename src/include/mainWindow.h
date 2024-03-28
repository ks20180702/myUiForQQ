#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "clientQQ.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    CMainWindow(QWidget *parent = nullptr);

    ~CMainWindow();

    int recv_thread_init();

private:
    Ui::MainWindow *ui;

    std::shared_ptr<ClientQQ> _mainClientPtr;
};
#endif // MAINWINDOW_H
