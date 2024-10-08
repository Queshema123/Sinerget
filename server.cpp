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
    , path_to_files{"C:\\Users\\admin\\Documents\\QtProjects\\logs"}
    , file_separator{"//"}
    , parsed_files_count{0}
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
}

void Server::prepareResponce()
{
    QDir dir(path_to_files);
    dir.setSorting(QDir::Name);
    dir.setFilter(QDir::Readable | QDir::Files);
    QStringList files{dir.entryList()};

    if (files.size() < parsed_files_count)
    {
        --parsed_files_count;
        return;
    }

    parsed_files_count = files.size();
    qsizetype idx{files.indexOf(last_parsed_file)
                  + 1}; // Если не найдет вернет -1 + 1 = 0 - индекс первого файла
    last_parsed_file = files.at(idx);
    file_watcher.setFuture(
        QtConcurrent::run(addResponce, this, path_to_files + file_separator + last_parsed_file));
}
