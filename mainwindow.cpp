#include "mainwindow.h"
#include <QAction>
#include <QHBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QStatusBar>
#include <QtConcurrent>
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

QWidget* MainWindow::createToolsWidgets()
{
    QWidget *tools_wgt = new QWidget;
    tools_wgt->setObjectName("ToolsWidget");
    QHBoxLayout *tools_layout = new QHBoxLayout;
    tools_wgt->setLayout(tools_layout);

    return tools_wgt;
}

QMenuBar* MainWindow::createMenuBar()
{
    QMenuBar *menu_bar = new QMenuBar;
    QMenu *data_menu = new QMenu("Данные");

    QAction *path_to_files_act = new QAction("Сменить путь к файлам");
    QAction *filter_act = new QAction("Фильтр");
    QAction* search_act = new QAction("Поиск");

    connect(path_to_files_act, &QAction::triggered,         server_wgt,   &ServerWidget::changePathToFiles);
    connect(filter_act,        &QAction::triggered,         filter_wgt,   [this]()
    {
        filter_wgt->setData( server_wgt->getModel() );
        filter_wgt->show();
    } );
    connect(search_act,        &QAction::triggered,         searched_wgt, [this]()
    {
        searched_wgt->setData( server_wgt->getModel() );
        searched_wgt->show();
    } );

    connect(filter_wgt,        &FilterWidget::info,         server_wgt,   &ServerWidget::setData);
    connect(server_wgt,        &ServerWidget::modelData,    filter_wgt,   [this](QAbstractItemModel* model) {
        if(filter_wgt->isVisible())
            filter_wgt->setData(model);
    } );
    // Search
    connect(filter_wgt,     &FilterWidget::info,                 searched_wgt, &SearchWidget::changeInfo);
    connect(server_wgt,     &ServerWidget::modelData,            searched_wgt, [this](QAbstractItemModel* model) {
        if(searched_wgt->isVisible())
            searched_wgt->setData(model);
    });
    connect(searched_wgt,   &SearchWidget::selectIndex,          server_wgt,   &ServerWidget::selectRow);

    data_menu->addAction(filter_act);
    data_menu->addAction(search_act);
    data_menu->addAction(path_to_files_act);
    menu_bar->addMenu(data_menu);

    QMenu *view_menu = new QMenu("Отображение");
    QAction *view_act = new QAction("Сменить");
    QMenu *theme_menu = new QMenu("Темы");
    QAction* black_theme = new QAction("Темная");
    QAction* white_theme = new QAction("Светлая");
    connect(black_theme, &QAction::triggered, this, [this]()
    {
        emit styleToWidget( getThemeStyleSheet(Theme::Black) );
    });
    connect(white_theme, &QAction::triggered, this, [this]()
    {
        emit styleToWidget( getThemeStyleSheet(Theme::White) );
    });

    theme_menu->addAction(black_theme);
    theme_menu->addAction(white_theme);

    view_menu->addAction(view_act);
    view_menu->addMenu(theme_menu);

    connect(view_act,          &QAction::triggered, server_wgt, &ServerWidget::changeDataView);

    menu_bar->addMenu(view_menu);

    QAction *exit_act = new QAction("Выход");
    connect(exit_act,          &QAction::triggered, this,       &MainWindow::close);
    menu_bar->addAction(exit_act);

    return menu_bar;
}

QString MainWindow::getThemeStyleSheet(Theme theme)
{
    QFile style_sheet_file;
    switch (theme) {
    case Theme::Black:
        style_sheet_file.setFileName("://Styles/Combinear.qss");
        break;
    case Theme::White:
        style_sheet_file.setFileName("://Styles/Integrid.qss");
        break;
    default:
        break;
    }
    style_sheet_file.open(QFile::ReadOnly);
    return QLatin1String(style_sheet_file.readAll());
}

QStatusBar* MainWindow::createStatusBar()
{
    QStatusBar* status_bar = new QStatusBar;
    connect(this, &MainWindow::status, status_bar, [status_bar](const QString& msg) { status_bar->showMessage(msg); });
    connect(server_wgt,   &ServerWidget::status, this, &MainWindow::status);
    connect(filter_wgt,   &FilterWidget::status, this, &MainWindow::status);
    connect(searched_wgt, &SearchWidget::status, this, &MainWindow::status);
    return status_bar;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , server_wgt{nullptr}
    , filter_wgt{nullptr}
    , searched_wgt{nullptr}
{
    QWidget *main_wgt = new QWidget;
    main_wgt->setObjectName("MainWidget");
    QVBoxLayout *main_layout = new QVBoxLayout(main_wgt);
    setCentralWidget(main_wgt);

    server_wgt = new ServerWidget(8081, this);
    filter_wgt = new FilterWidget(this);
    searched_wgt = new SearchWidget(this);

    changeMenuBar(createMenuBar());
    changeTools(createToolsWidgets());
    this->setStatusBar( createStatusBar() );

    main_layout->addWidget(server_wgt);
    this->resize(800, 800);
}

MainWindow::~MainWindow() {}
