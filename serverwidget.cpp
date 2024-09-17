#include "serverwidget.h"
#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include "converter.h"

ServerWidget::ServerWidget(quint16 port, QWidget *parent)
    : QWidget{parent}
    , server{nullptr}
    , port{port}
    , txt_editor{new QTextEdit}
{
    server = new Server(port, this);
    timer = new QTimer(this);
    timer->setInterval(60000);

    QVBoxLayout *main_layout = new QVBoxLayout(this);

    main_layout->setDirection(QBoxLayout::Direction::TopToBottom);
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

    txt_editor->setParent(this);
    txt_editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    main_layout->addWidget(txt_editor, 1);

    connect(timer,          &QTimer::timeout,                 this, &ServerWidget::updateViewData);
    connect(update_box,     &QCheckBox::clicked,              this, &ServerWidget::enableAutoUpdate);
    connect(this,           &ServerWidget::changePathToFiles, this, &ServerWidget::setPath);
    connect(update_txt_btn, &QPushButton::clicked,            this, &ServerWidget::updateViewData);
}

void ServerWidget::updateViewData()
{
    last_responce = server->getResponce();
    txt_editor->setText(last_responce);
    emit responceData(server->getData());
}

void ServerWidget::setData(const QVector<Token> &tokens)
{
    if (!tokens.size()) {
        txt_editor->setText(last_responce);
        return;
    }

    txt_editor->setText(Converter::convertToPrometheus(tokens));
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
