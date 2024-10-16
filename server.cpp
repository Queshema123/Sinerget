#include "server.h"
#include "converter.h"
#include <QDir>
#include <QHttpServerResponse>
#include <QtConcurrent>
#include <QFuture>

Server::Server(quint16 port, QObject *parent)
    : QObject{parent}
    , server{nullptr}
    , port{port}
    , subdomain{"/metrics"}
    , path_to_files{""}
    , responce{""}
    , model{nullptr}
{
    server = new QHttpServer(this);
    server->route(subdomain, [this]() {
        QHttpServerResponse resp(responce);
        return resp;
    });

    server->listen(QHostAddress::LocalHost, port);
    dir_watcher = new QFileSystemWatcher(this);
    dir_watcher->addPath(path_to_files);
    connect(dir_watcher, &QFileSystemWatcher::directoryChanged, this, &Server::prepareResponce);
    model = new QStandardItemModel(this);
}

void Server::setPathToFiles(const QString &path)
{
    dir_watcher->removePath(path_to_files);
    dir_watcher->addPath(path);
    path_to_files = path;
}

void Server::setData(const QList<Token>& tokens)
{
    model->setRowCount(tokens.size());
    model->setColumnCount(2);
    model->setHeaderData(0, Qt::Horizontal, "Название");
    model->setHeaderData(1, Qt::Horizontal, "Значение");

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
    auto convert = [](const QList<Token>& tokens) { return Converter::convertToPrometheus(tokens); };
    QtConcurrent::run( convert, tokens ).then([this](const QString& str) { responce = str; });
    emit this->responceData(model);
}

void Server::addResponce(const QString &path_to_file)
{
    setData( Converter::parseFile(path_to_file) );
    QFile::remove(path_to_file);
}

void Server::prepareResponce()
{
    QDir dir(path_to_files);
    dir.setSorting(QDir::Name);
    dir.setFilter(QDir::Readable | QDir::Files);
    QStringList files{dir.entryList()};

    if (files.size() == 0)
        return;

    QtConcurrent::run( addResponce, this, path_to_files + "/" + files[0] );
}
