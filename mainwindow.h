#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "server.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Server* server;
    quint16 port;
public:
    MainWindow(QWidget *parent = nullptr);    
    ~MainWindow();
};
#endif // MAINWINDOW_H
