#include "server.h"
#include <QDir>
#include <QFuture>
#include <QHttpServerResponse>
#include <QtConcurrent>
#include "converter.h"

Server::Server(quint16 port, QObject *parent)
    : QObject{parent}
    , server{nullptr}
    , port{port}
    , subdomain{"/metrics"}
    , last_parsed_file{""}
    , path_to_files{""}
    , file_separator{"/"}
    , responce{""}
{
    server = new QHttpServer(this);
    server->route(subdomain, [this]() {
        if (file_watcher.isFinished() && responces.size() > 0) {
            responce.push_back(responces.first());
            responce_data.append(all_data.first());

            responces.removeFirst();
            all_data.removeFirst();
        }

        QHttpServerResponse resp(responce);
        return resp;
    });

    server->listen(QHostAddress::LocalHost, port);
    dir_watcher = new QFileSystemWatcher(this);
    dir_watcher->addPath(path_to_files);
    connect(dir_watcher, &QFileSystemWatcher::directoryChanged, this, &Server::prepareResponce);
}

void Server::setPathToFiles(const QString &path)
{
    dir_watcher->removePath(path_to_files);
    dir_watcher->addPath(path);
    path_to_files = path;
}

void Server::addResponce(const QString &path_to_file)
{
    all_data.push_back(Converter::parseFile(path_to_file));
    responces.append(Converter::convertToPrometheus(all_data.last()));
    qDebug() << path_to_file;
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

    last_parsed_file = path_to_files + file_separator + files[0];
    file_watcher.setFuture(
        QtConcurrent::run(addResponce, this, last_parsed_file) );
}
