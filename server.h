#ifndef SERVER_H
#define SERVER_H

#include <QFileSystemWatcher>
#include <QFutureWatcher>
#include <QString>
#include <QVector>
#include <QtHttpServer>
#include "token.h"

class Server : public QObject
{
    Q_OBJECT

    QHttpServer *server;
    quint16 port;
    QString path_to_files, subdomain, last_parsed_file, file_separator, responce;
    QStringList responces;
    QVector<QVector<Token>> all_data;
    QVector<Token> responce_data;
    QFileSystemWatcher *dir_watcher;
    QFutureWatcher<void> file_watcher;
    int parsed_files_count;

public:
    Server(quint16 port, QObject *parent = nullptr);
    void addResponce(const QString &path_to_file);
    void prepareResponce();

    void setPathToFiles(const QString &path);
    inline QString getPathToFiles() const { return path_to_files; }
    inline QString getResponce() const { return responce; }
    inline const QVector<Token> &getData() const { return responce_data; }
};

#endif // SERVER_H
