#ifndef SERVERWIDGET_H
#define SERVERWIDGET_H

#include "server.h"
#include "customproxymodel.h"

#include <QWidget>
#include <QStandardItemModel>
#include <QAbstractItemView>


class ServerWidget : public QWidget
{
    Q_OBJECT

    Server *server;
    quint16 port;
    QString last_responce;
    QStandardItemModel* model;
    QAbstractItemView* view;
    CustomProxyModel* proxy_model;
public:
    explicit ServerWidget(quint16 port, QWidget *parent = nullptr);
    void setupModelAndView(QLayout* main_layout);
    QAbstractItemModel* getModel() { return proxy_model; }
    QString getPathToFiles() const { return server->getPathToFiles(); }
public slots:
    void setPathToFiles(const QString& path) { server->setPathToFiles(path); }
    void setPath();
    void setData(const QList<QList<Info>>& info);
    void changeDataView();
    void selectRow(qsizetype row);
signals:
    void changePathToFiles();
    void status(const QString& status);
    void modelData(QAbstractItemModel* model);
};

#endif // SERVERWIDGET_H
