#ifndef SERVER_H
#define SERVER_H

#include <QtHttpServer/QHttpServer>
#include <QString>
#include <QThread>
#include <QVector>
#include <QFileSystemWatcher>
#include <QFutureWatcher>

class Server : public QObject
{
    Q_OBJECT

    QHttpServer* server;
    quint16 port;
    QString path_to_files, subdomain, last_parsed_file, file_separator;
    QStringList responces;
    QFileSystemWatcher* dir_watcher;
    QFutureWatcher<QString> file_watcher;
    int parsed_files_count;

public:
    Server(quint16 port, QObject* parent = nullptr);
    QString createResponce(const QString &path_to_file);
    void prepareResponce();

    void setPathToFiles(const QString &path);
    inline QString getPathToFiles() const { return path_to_files; }
    inline QString getResponce() const { return responces.first(); }
};

#endif // SERVER_H
