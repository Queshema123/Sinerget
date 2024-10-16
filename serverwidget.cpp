#include "serverwidget.h"
#include "converter.h"
#include "info.h"

#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QTreeView>
#include <QMessageBox>


ServerWidget::ServerWidget(quint16 port, QWidget *parent)
    : QWidget{parent}
    , server{nullptr}
    , port{port}
    , proxy_model{nullptr}
{
    server = new Server(port, this);
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    setupModelAndView(main_layout);
    connect(this,           &ServerWidget::changePathToFiles, this, &ServerWidget::setPath);
    connect(server,         &Server::responceData,            this, [this](QAbstractItemModel*){ emit modelData(proxy_model); });
}

void ServerWidget::setupModelAndView(QLayout* main_layout)
{
    proxy_model = new CustomProxyModel(this);
    proxy_model->setSourceModel( server->getData() );
    view = new QTreeView();
    view->setModel( proxy_model );
    main_layout->addWidget( view );
    view->show();
}

void ServerWidget::setData(const QList<Info>& info)
{
    proxy_model->setFilterConditions(info);
}

void ServerWidget::setPath()
{
    QString file_path = QFileDialog::getExistingDirectory(this, "Укажите путь к файлам");
    server->setPathToFiles(QFileInfo(file_path).absoluteFilePath());
}

void ServerWidget::changeDataView()
{
    // Добавить окно с выбором представления данных
    // Добавить смену view
    // Добавить окошко с надписью "Здесь выбор отображения"
    QMessageBox::information(this, "Смена отображения", "");
}

void ServerWidget::selectRow(QModelIndex row)
{
    if( row.isValid() )
    {
        view->scrollTo(row);
        view->selectionModel()->select(row, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    }
}
