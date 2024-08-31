#ifndef SERVERWIDGET_H
#define SERVERWIDGET_H

#include "server.h"
#include <QWidget>
#include <QTextEdit>

class ServerWidget : public QWidget
{
    Q_OBJECT

    Server* server;
    quint16 port;
    QTextEdit* txt_editor;
public:
    explicit ServerWidget(quint16 port, QWidget *parent = nullptr);
    inline QString getResponce() const { return server->getResponce(); }
public slots:
    void setPath();
    void setData(const QVector<Token>& tokens);
signals:
    void changePathToFiles();
    void responceData(const QVector<Token>& tokens);
};

#endif // SERVERWIDGET_H
