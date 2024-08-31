#ifndef SERVER_H
#define SERVER_H

#include <QtHttpServer/QHttpServer>
#include <QString>
#include <QThread>
#include <QVector>
#include <QFileSystemWatcher>
#include <QFutureWatcher>
#include "token.h"

class Server : public QObject
{
    Q_OBJECT

    QHttpServer* server;
    quint16 port;
    QString path_to_files, subdomain, last_parsed_file, file_separator;
    QStringList responces;
    QVector<QVector<Token>> tokens_responces;
    QFileSystemWatcher* dir_watcher;
    QFutureWatcher<QString> file_watcher;
    int parsed_files_count;

public:
    Server(quint16 port, QObject* parent = nullptr);
    QString createResponce(const QString &path_to_file);
    void prepareResponce();

    void setPathToFiles(const QString &path);
    inline QString getPathToFiles() const { return path_to_files; }
    inline QString getResponce() const { return (responces.size() > 0) ? responces.first() : ""; }
    bool hasData() const { return tokens_responces.size() > 0; }
    const QVector<Token>& getData() const { return tokens_responces.first(); }
};

#endif // SERVER_H
