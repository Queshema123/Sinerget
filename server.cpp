#include "server.h"
#include "converter.h"
#include <QDir>
#include <QFuture>
#include <QtConcurrent>

Server::Server(quint16 port, QObject* parent) : QObject{parent}, server{nullptr}, port{port}, subdomain{"/metrics"},
    last_parsed_file{""}, path_to_files{"C:\\Users\\admin\\Documents\\QtProjects\\logs"}, file_separator{"//"}, parsed_files_count{0}
{
    server = new QHttpServer(this);
    server->route(subdomain, [this]()
    {
        QHttpServerResponse resp("");
        if( file_watcher.isFinished() && responces.size() > 0 )
        {
            resp = QHttpServerResponse(responces.first());
            responces.removeFirst();
        }
        return resp;
    }
    );
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

QString Server::createResponce(const QString &path_to_file)
{
    return Converter::convertToPrometheus( path_to_file );
}

void Server::prepareResponce()
{ // Предусмотреть удаление файла, добавить переменную хранящую последнее количество файлов в папке
    QDir dir(path_to_files);
    dir.setSorting(QDir::Name);
    dir.setFilter(QDir::Readable | QDir::Files);
    QStringList files{ dir.entryList() };

    if(files.size() < parsed_files_count)
        return;

    parsed_files_count = files.size();
    qsizetype idx{ files.indexOf(last_parsed_file) + 1 }; // Если не найдет вернет -1 + 1 = 0 - индекс первого файла)
    last_parsed_file = files.at(idx);
    file_watcher.setFuture( QtConcurrent::run( createResponce, this, path_to_files + file_separator + last_parsed_file ) );
    QObject::connect(&file_watcher, &QFutureWatcher<QString>::finished, this, [this]()
    {
        QString str{ QString::fromUtf8(file_watcher.result().toUtf8() ) };
        responces.append( str );
    }
    );
}
