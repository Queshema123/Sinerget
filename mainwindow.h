#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "serverwidget.h"
#include "filterwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    ServerWidget* server_wgt;
    FilterWidget* filter_wgt;
public:
    MainWindow(QWidget *parent = nullptr);
    QWidget* createToolsWidgets();
    QMenuBar* createMenuBar();
    ~MainWindow();
public slots:
    void changeMenuBar(QMenuBar* menu);
    void changeTools(QWidget* tools_wgt);
signals:
    void filterData();
};
#endif // MAINWINDOW_H
