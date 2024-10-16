#include <QDomDocument>
#include <QFile>
#include <QString>
#include <QVector>

#include "token.h"

#ifndef _CONVERTER_H_
#define _CONVERTER_H_

namespace Converter {
const static QVector<QString> reserved_words = {"class", "struct"};

QString prepareData(QFile &file);
void changeMetricValue(Token &token);

QVector<Token> parseNodeChilds(const QDomNode &node);
QVector<Token> parseDocument(const QDomDocument &doc);
QVector<Token> parseFile(const QString &file_path);
QString convertToPrometheus(const QVector<Token> &data);
QString convertToPrometheus(const QString &file_path);
} // namespace Converter

#endif // _CONVERTER_H_
