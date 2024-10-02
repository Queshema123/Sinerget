#ifndef SERVERWIDGET_H
#define SERVERWIDGET_H

#include "server.h"

#include <QWidget>
#include <QTimer>
#include <QStandardItemModel>
#include <QAbstractItemView>


class ServerWidget : public QWidget
{
    Q_OBJECT

    Server *server;
    quint16 port;
    QString last_responce;
    QTimer* timer;
    QStandardItemModel* model;
    QAbstractItemView* view;
public:
    explicit ServerWidget(quint16 port, QWidget *parent = nullptr);
    inline QString getResponce() const { return server->getResponce(); }
    void setupModelAndView(QLayout* main_layout);
public slots:
    void setPath();
    void setData(const QVector<Token> &tokens);
    void updateViewData();
    void enableAutoUpdate(bool enable);
    void changeDataView();
    void selectRow(qsizetype row);
signals:
    void changePathToFiles();
    void responceData(const QVector<Token> &tokens);
};

#endif // SERVERWIDGET_H
