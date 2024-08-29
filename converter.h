#include <QVector>
#include <QString>
#include <QByteArray>
#include <QFile>
#include <QDomDocument>

#include "token.h"

#ifndef _CONVERTER_H_
#define _CONVERTER_H_

namespace Converter
{
    QString prepareData(QFile& file);
    void addLabels(Token& token, const QMultiMap<QString, QString>& labels);

    QVector<Token> parseNodeChilds(const QDomNode& node);
    QVector<Token> parseDocument(const QDomDocument& doc);
    QVector<Token> parseFile(const QString &file_path);
    QString convertToPrometheus(const QVector<Token> &data);
    QString convertToPrometheus(const QString &file_path);
};

#endif // _CONVERTER_H_
