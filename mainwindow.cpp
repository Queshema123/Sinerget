#include "mainwindow.h"
#include <QAction>
#include <QHBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QStatusBar>
#include <QDir>

void MainWindow::fillThemeMenu(QMenu* menu)
{
    QDir dir{"://Styles/"};
    if(!dir.exists())
    {
        qWarning() << "Can't open dir with themes. Path is - " << dir.absolutePath();
        return;
    }
    dir.setFilter(QDir::Readable | QDir::Files);
    dir.setNameFilters( {"*.qss"} );
    QStringList files{ dir.entryList() };
    QFile file;
    foreach (const QString& file_name, files)
    {
        file.setFileName(dir.absoluteFilePath(file_name));
        if(!file.open(QFile::ReadOnly))
            continue;

        QString style_sheet{ QLatin1String(file.readAll()) };
        QAction* act = new QAction(file_name.mid(0, file_name.indexOf('.')));
        connect(act, &QAction::triggered, this, [this, style_sheet, file_name](){ emit themeStyleSheet(style_sheet); settings_values["theme"] = file_name; });
        menu->addAction(act);
        file.close();
    }
}

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

QMenu* MainWindow::createDataMenu()
{
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
    //connect(filter_wgt,     &FilterWidget::info,                 searched_wgt, &SearchWidget::changeInfo);
    connect(server_wgt,     &ServerWidget::modelData,            searched_wgt, [this](QAbstractItemModel* model) {
        if(searched_wgt->isVisible())
            searched_wgt->setData(model);
    });
    connect(searched_wgt,   &SearchWidget::selectIndex,          server_wgt,   &ServerWidget::selectRow);

    data_menu->addAction(filter_act);
    data_menu->addAction(search_act);
    data_menu->addAction(path_to_files_act);

    return data_menu;
}

QMenu* MainWindow::createViewMenu()
{
    QMenu *view_menu = new QMenu("Отображение");
    QAction *view_act = new QAction("Сменить");
    QMenu *theme_menu = new QMenu("Темы");
    fillThemeMenu(theme_menu);

    view_menu->addAction(view_act);
    view_menu->addMenu(theme_menu);

    connect(view_act,          &QAction::triggered, server_wgt, &ServerWidget::changeDataView);

    return view_menu;
}

QMenuBar* MainWindow::createMenuBar()
{
    QMenuBar *menu_bar = new QMenuBar;

    menu_bar->addMenu(createDataMenu());
    menu_bar->addMenu(createViewMenu());

    QAction *exit_act = new QAction("Выход");
    connect(exit_act,          &QAction::triggered, this,       &MainWindow::close);
    menu_bar->addAction(exit_act);

    return menu_bar;
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
    : QMainWindow(parent), server_wgt{nullptr}, filter_wgt{nullptr}, searched_wgt{nullptr},
    app_settings("app_settings.ini", QSettings::IniFormat)
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
}

void MainWindow::saveSettings()
{
    app_settings.beginGroup("MainWindow");
    app_settings.setValue("theme", settings_values["theme"]);
    app_settings.setValue("path_to_files", server_wgt->getPathToFiles());
    app_settings.setValue("size", this->size());
    app_settings.endGroup();
}

void MainWindow::setupSettings()
{
    app_settings.beginGroup("MainWindow");
    this->resize( app_settings.value("size").toSize() );
    server_wgt->setPathToFiles( app_settings.value("path_to_files").toString() );
    // Вынести ниже в отдельную функцию и использовать ее здесь и в fillThemeMenu
    settings_values["size"] = app_settings.value("size").toString();
    settings_values["path_to_files"] = app_settings.value("path_to_files").toString();
    settings_values["theme"] = app_settings.value("theme").toString();

    QFile file( "://Styles/" + app_settings.value("theme").toString() );
    if(!file.open(QFile::ReadOnly))
        return;
    QString style_sheet{ QLatin1String(file.readAll()) };
    emit themeStyleSheet(style_sheet);
    app_settings.endGroup();
}

MainWindow::~MainWindow()
{
    saveSettings();
}
