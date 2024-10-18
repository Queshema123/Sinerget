#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "filterwidget.h"
#include "serverwidget.h"
#include "searchwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    ServerWidget *server_wgt;
    FilterWidget *filter_wgt;
    SearchWidget *searched_wgt;
public:
    MainWindow(QWidget *parent = nullptr);
    QWidget* createToolsWidgets();
    QMenuBar* createMenuBar();
    QStatusBar* createStatusBar();
    void fillThemeMenu(QMenu* menu);
    ~MainWindow();
public slots:
    void changeMenuBar(QMenuBar *menu);
    void changeTools(QWidget *tools_wgt);
signals:
    void themeStyleSheet(const QString& stylesheet);
    void status(const QString& status);
};
#endif // MAINWINDOW_H
