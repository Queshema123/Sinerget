#include "mainwindow.h"
#include <QAction>
#include <QHBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>
#include "converter.h"

void MainWindow::changeMenuBar(QMenuBar *menu)
{
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(this->centralWidget()->layout());
    layout->setMenuBar(menu);
}

void MainWindow::changeTools(QWidget *tools_wgt)
{
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(this->centralWidget()->layout());
    layout->insertWidget(0, tools_wgt, 0.5, Qt::AlignTop);
}

QWidget *MainWindow::createToolsWidgets()
{
    QWidget *tools_wgt = new QWidget;
    tools_wgt->setObjectName("ToolsWidget");
    QHBoxLayout *tools_layout = new QHBoxLayout;
    tools_wgt->setLayout(tools_layout);

    return tools_wgt;
}

QMenuBar *MainWindow::createMenuBar()
{
    QMenuBar *menu_bar = new QMenuBar;
    QMenu *data_menu = new QMenu("Data");

    QAction *path_to_files_act = new QAction("Change path");
    QAction *filter_act = new QAction("Filter data");
    QAction *view_act = new QAction("Change view");
    QAction *exit_act = new QAction("exit");

    connect(exit_act,          &QAction::triggered, this,       &MainWindow::close);
    connect(path_to_files_act, &QAction::triggered, server_wgt, &ServerWidget::changePathToFiles);
    connect(filter_act,        &QAction::triggered, filter_wgt, &FilterWidget::show);
    connect(view_act,          &QAction::triggered, server_wgt, &ServerWidget::changeDataView);

    connect(filter_wgt, &FilterWidget::filteredData, server_wgt, &ServerWidget::setData);
    connect(server_wgt, &ServerWidget::responceData, filter_wgt, &FilterWidget::setData);

    data_menu->addAction(filter_act);
    data_menu->addAction(path_to_files_act);
    data_menu->addAction(view_act);

    menu_bar->addMenu(data_menu);
    menu_bar->addAction(exit_act);

    return menu_bar;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , server_wgt{nullptr}
    , filter_wgt{nullptr}
{
    QWidget *main_wgt = new QWidget;
    main_wgt->setObjectName("MainWidget");
    QVBoxLayout *main_layout = new QVBoxLayout(main_wgt);
    setCentralWidget(main_wgt);

    server_wgt = new ServerWidget(8081, this);
    filter_wgt = new FilterWidget(this);

    changeMenuBar(createMenuBar());
    changeTools(createToolsWidgets());

    main_layout->addWidget(server_wgt);
}

MainWindow::~MainWindow() {}
