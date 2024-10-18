#include "server.h"
#include "converter.h"
#include <QDir>
#include <QHttpServerResponse>
#include <QtConcurrent>
#include <algorithm>
#include <numeric>

Server::Server(quint16 port, QObject *parent)
    : QObject{parent} , server{nullptr}, port{port}
    , subdomain{"/metrics"}, path_to_files{""}, responce{""} , model{nullptr}
{
    server = new QHttpServer(this);
    server->route(subdomain, [this]() {
        QHttpServerResponse resp(responce);
        return resp;
    });

    server->listen(QHostAddress::Any, port);
    dir_watcher = new QFileSystemWatcher(this);
    dir_watcher->addPath(path_to_files);
    connect(dir_watcher, &QFileSystemWatcher::directoryChanged, this, &Server::prepareResponce);
    model = new QStandardItemModel(this);
}

void Server::setPathToFiles(const QString &path)
{
    dir_watcher->removePath(path_to_files);
    dir_watcher->addPath(path);
    path_to_files = path;
}

void Server::deleteUnusedMetrics(QList<Token> &data)
{
    QRegularExpression reg_exp{"([a-zA-z]+)"};
    const double eps{ std::numeric_limits<double>().epsilon() };
    auto isUnused = [&reg_exp, &eps](const Token& t)
    {
        QRegularExpressionMatch match{};
        QString parents{ t.getLabels().value("parents") };
        parents.lastIndexOf(reg_exp, -1, &match);
        double val{ t.getMetricValue().toDouble()};
        return match.captured(0) == "SNObjectProxy" || match.captured(0) == "SNServerObjectProxy" || fabs(val) <= eps;
    };

    data.erase( std::remove_if(data.begin(), data.end(), isUnused), data.end() );
}

void Server::setData(const QList<Token>& data)
{
    auto tokens = data;
    deleteUnusedMetrics(tokens);

    model->setRowCount(tokens.size());
    model->setColumnCount(2);
    model->setHeaderData(0, Qt::Horizontal, "Название");
    model->setHeaderData(1, Qt::Horizontal, "Значение");

    int row{0};
    foreach (const Token& token, tokens) {
        QStandardItem *root_left_item = new QStandardItem(token.getMetricName());
        QStandardItem *root_right_item = new QStandardItem(token.getMetricValue());

        model->setItem(row, 0, root_left_item);
        model->setItem(row, 1, root_right_item);

        for(auto [key,value] : token.getLabels().asKeyValueRange())
        {
            QStandardItem *left_item = new QStandardItem(key);
            QStandardItem *right_item = new QStandardItem(value);
            root_left_item->appendRow( {left_item, right_item} );
        }
        ++row;
    }
    auto convert = [](const QList<Token>& tokens) { return Converter::convertToPrometheus(tokens); };
    QtConcurrent::run( convert, tokens ).then([this](const QString& str) { responce = str; });
    emit this->responceData(model);
}

void Server::addResponce(const QString &path_to_file)
{
    setData( Converter::parseFile(path_to_file) );
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

    QtConcurrent::run( addResponce, this, path_to_files + "/" + files[0] );
}
