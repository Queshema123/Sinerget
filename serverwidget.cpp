#include "serverwidget.h"
#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include "converter.h"

ServerWidget::ServerWidget(quint16 port, QWidget *parent)
    : QWidget{parent}
    , server{nullptr}
    , port{port}
    , txt_editor{new QTextEdit}
{
    server = new Server(port, this);
    QVBoxLayout *main_layout = new QVBoxLayout(this);

    main_layout->setDirection(QBoxLayout::Direction::TopToBottom);
    QPushButton *update_txt_btn = new QPushButton("Обновить");
    update_txt_btn->setObjectName("UpdateBtn");
    update_txt_btn->setMaximumWidth(100);
    main_layout->addWidget(update_txt_btn);

    txt_editor->setParent(this);
    txt_editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    main_layout->addWidget(txt_editor, 1);

    connect(this, &ServerWidget::changePathToFiles, this, &ServerWidget::setPath);
    connect(update_txt_btn, &QPushButton::clicked, this, &ServerWidget::updateViewData);
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
