#ifndef SERVER_H
#define SERVER_H

#include <QFileSystemWatcher>
#include <QString>
#include <QList>
#include <QtHttpServer>
#include <QStandardItemModel>
#include "token.h"

class Server : public QObject
{
    Q_OBJECT

    QHttpServer *server;
    quint16 port;
    QString path_to_files, subdomain, responce;

    QStandardItemModel* model;
    QFileSystemWatcher *dir_watcher;

public:
    Server(quint16 port, QObject *parent = nullptr);
    void deleteUnusedMetrics(QList<Token> &data);
    void addResponce(const QString &path_to_file);
    void prepareResponce();

    void setData(const QList<Token>& data);
    void setPathToFiles(const QString &path);
    inline QString getPathToFiles() const { return path_to_files; }
    inline QStandardItemModel* getData() const { return model; }
signals:
    void responceData(QAbstractItemModel* model);
};

#endif // SERVER_H
