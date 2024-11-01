#include "converter.h"
#include <QByteArray>
#include <QDebug>
#include <QTextCodec>
#include <QMap>

QString Converter::prepareData(QFile &file)
{
    QTextCodec *codec_1251 = QTextCodec::codecForName("Windows-1251");
    QString s{codec_1251->toUnicode(file.readAll())};
    QTextCodec *codec_utf8 = QTextCodec::codecForName("UTF-8");
    QByteArray arr_utf8{codec_utf8->fromUnicode(s)};
    QString str{QString::fromUtf8(arr_utf8)};

    str.replace("windows-1251", "UTF-8");
    str.replace("::", "_");
    str.replace(",", "_");
    str.replace("&", "ref");
    return str;
}

void Converter::changeMetricValue(Token &token)
{
    auto it = token.getLabels().find("value");
    if (it != token.getLabels().end()) {
        token.setMetricValue(it.value());
        token.deleteLabel(it.key());
    }
}

QVector<Token> Converter::parseNodeChilds(const QDomNode &node, QString& parent_path, QMap<QString, int>& parents_pathes)
{
    QVector<Token> tokens;
    QDomNodeList childs = node.childNodes();
    for (int i{0}; i < childs.size(); ++i) {
        QDomNode n{childs.at(i)};

        if (n.hasChildNodes())
        {
            int number = parents_pathes[parent_path+n.nodeName()+'|']; // Количество тегов на том же уровне вложенности с тем же названием
            parents_pathes[parent_path+n.nodeName()+'|'] += 1;
            QString elem = (number > 0) ? n.nodeName() + QString::number(number) + '|' : n.nodeName() + '|'; // Добавляемый элемент
            parent_path.push_back(elem);

            auto new_tokens{parseNodeChilds(n, parent_path, parents_pathes)};
            for (int i{0}; i < new_tokens.size(); ++i)
                tokens.push_back(new_tokens[i]);

            parent_path.remove(elem); // Удаляем последнее добавленное имя родительского тега
        }
        else
        {
            Token t{n.nodeName(), n.attributes()};
            changeMetricValue(t);
            t.addLabel("parents", parent_path);
            tokens.push_back(t);
        }
    }
    return tokens;
}

QVector<Token> Converter::parseDocument(const QDomDocument &doc)
{
    QDomElement doc_element = doc.documentElement();
    QDomNode node{doc_element.firstChild()};

    QVector<Token> tokens;
    QString parent_path;
    QMap<QString, int> parents_pathes;
    while (!node.isNull()) {
        Token tmp{node.nodeName(), node.attributes()};
        changeMetricValue(tmp);

        if (node.hasChildNodes())
        {
            parent_path.push_back(node.nodeName() + '|');
            parents_pathes[parent_path] += 1;
            tokens.append(parseNodeChilds(node, parent_path, parents_pathes));
            parent_path.clear();
        }

        tokens.push_back(tmp);
        node = node.nextSibling();
    }

    return tokens;
}

QVector<Token> Converter::parseFile(const QString &file_path)
{
    QFile file{file_path};
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Can't open file " << file.fileName();
        return {};
    }

    QString utf8_text{prepareData(file)};
    QDomDocument doc;
    QString msg;
    int line, col;
    if (!doc.setContent(utf8_text, &msg, &line, &col)) {
        file.close();
        qWarning("Invalid XML in file %s. Error = %s, Line = %d, Column = %d",
                 qPrintable(file.fileName()),
                 qPrintable(msg),
                 line,
                 col);
        return {};
    }
    file.close();

    return parseDocument(doc);
}

QString Converter::convertToPrometheus(const QVector<Token> &data)
{
    QString prometheus_data;
    for (const auto &token : data) {
        prometheus_data.append(token.getMetricName() + "{");
        const auto &labels = token.getLabels();
        for (auto [key, val] : labels.asKeyValueRange()) {
            prometheus_data.append(key + " = \"" + val + "\" ");
        }

        prometheus_data.append("}" + token.getMetricValue() + "\n");
    }
    return prometheus_data;
}

QString Converter::convertToPrometheus(const QString &file_path)
{
    return Converter::convertToPrometheus(Converter::parseFile(file_path));
}
