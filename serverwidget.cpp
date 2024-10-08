#include "serverwidget.h"
#include "converter.h"

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
{
    server = new Server(port, this);
    timer = new QTimer(this);
    timer->setInterval(60000);

    QVBoxLayout *main_layout = new QVBoxLayout(this);

    QPushButton *update_txt_btn = new QPushButton("Обновить");
    update_txt_btn->setObjectName("UpdateBtn");
    update_txt_btn->setMaximumWidth(100);
    QCheckBox* update_box = new QCheckBox;
    QLabel* lbl = new QLabel("Auto update");
    QHBoxLayout* update_layout = new QHBoxLayout;
    update_layout->setAlignment(Qt::AlignLeft);
    update_layout->addWidget(update_txt_btn);
    update_layout->addWidget(lbl);
    update_layout->addWidget(update_box);

    main_layout->addLayout(update_layout);

    setupModelAndView(main_layout);

    connect(timer,          &QTimer::timeout,                 this, &ServerWidget::updateViewData);
    connect(update_box,     &QCheckBox::clicked,              this, &ServerWidget::enableAutoUpdate);
    connect(this,           &ServerWidget::changePathToFiles, this, &ServerWidget::setPath);
    connect(update_txt_btn, &QPushButton::clicked,            this, &ServerWidget::updateViewData);
}

void ServerWidget::setupModelAndView(QLayout* main_layout)
{
    model = new QStandardItemModel(this);
    view = new QTreeView();
    view->setModel( model );
    main_layout->addWidget( view );
    view->show();
}

void ServerWidget::updateViewData()
{
    emit status("Обновление отображения...");
    last_responce = server->getResponce();
    setData( server->getData() );
    emit responceData(server->getData());
}

void ServerWidget::setData(const QVector<Token> &tokens)
{
    model->setRowCount(tokens.size());
    model->setColumnCount(2);
    model->setHeaderData(0, Qt::Horizontal, "Metric");
    model->setHeaderData(1, Qt::Horizontal, "Value");

    int row{0};
    foreach (const Token& token, tokens) {
        QStandardItem *root_left_item = new QStandardItem(token.getMetricName());
        QStandardItem *root_right_item = new QStandardItem(token.getMetricValue());

        model->setItem(row, 0, root_left_item);
        model->setItem(row, 1, root_right_item);

        for(auto [key,value] : token.getLabels().asKeyValueRange())
        {
            QStandardItem *left_item = new QStandardItem(key);
            QStandardItem *right_item = new QStandardItem(value);
            root_left_item->appendRow( {left_item, right_item} );
        }
        ++row;
    }
    emit status("Данные обновлены");
}

void ServerWidget::setPath()
{
    QString file_path = QFileDialog::getExistingDirectory(this, "Укажите путь к файлам");
    server->setPathToFiles(QFileInfo(file_path).absoluteFilePath());
}

void ServerWidget::enableAutoUpdate(bool enable)
{
    if(enable)
        timer->start();
    else
        timer->stop();
}

void ServerWidget::changeDataView()
{
    // Добавить окно с выбором представления данных
    // Добавить смену view
    // Добавить окошко с надписью "Здесь выбор отображения"
    QMessageBox::information(this, "Смена отображения", "");
}

void ServerWidget::selectRow(qsizetype row)
{
    QModelIndex idx{ model->index(row, 0) };
    if( idx.isValid() )
    {
        view->scrollTo(idx);
        view->selectionModel()->select(idx, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    }
}
