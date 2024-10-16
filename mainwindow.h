#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFuture>
#include "filterwidget.h"
#include "serverwidget.h"
#include "searchwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    ServerWidget *server_wgt;
    FilterWidget *filter_wgt;
    SearchWidget *searched_wgt;
    QFuture<void> filter_ready, search_ready;
public:
    MainWindow(QWidget *parent = nullptr);
    QWidget* createToolsWidgets();
    QMenuBar* createMenuBar();
    QStatusBar* createStatusBar();
    enum class Theme{ Black, White };
    static QString getThemeStyleSheet(Theme theme);
    ~MainWindow();
public slots:
    void changeMenuBar(QMenuBar *menu);
    void changeTools(QWidget *tools_wgt);
signals:
    void status(const QString& status);
    void styleToWidget(const QString& stylesheet);
};
#endif // MAINWINDOW_H
