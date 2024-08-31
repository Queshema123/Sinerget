#include "converter.h"
#include <QTextCodec>
#include <QDebug>

QString Converter::prepareData(QFile& file)
{
    QTextCodec* codec_1251 = QTextCodec::codecForName("Windows-1251");
    QString s{ codec_1251->toUnicode( file.readAll() ) };
    QTextCodec* codec_utf8 = QTextCodec::codecForName("UTF-8");
    QByteArray arr_utf8{ codec_utf8->fromUnicode(s) };
    QString str{ QString::fromUtf8(arr_utf8) };

    str.replace("windows-1251", "UTF-8");
    str.replace("::", "_");
    str.replace(",", "_");
    str.replace("&", "ref");
    return str;
}

void Converter::addLabels(Token& token, const QMultiMap<QString, QString>& labels)
{
    int idx{1};
    for(auto it = labels.begin(); it != labels.end(); ++it)
    {
        QString key{ it.key() };
        if(token.getLabels().contains(key))
            key += QString::number(++idx);
        token.addLabel(key, it.value() );
    }
}

QVector<Token> Converter::parseNodeChilds(const QDomNode& node)
{
    QVector<Token> tokens;
    QDomNodeList childs = node.childNodes();
    for(int i{0}; i < childs.size(); ++i)
    {
        QDomNode n{ childs.at(i) };

        if( n.hasChildNodes() )
        {
            auto new_tokens{ parseNodeChilds(n) };
            for(int i{0}; i < new_tokens.size(); ++i )
                tokens.push_back(new_tokens[i]);
        }
        else
        {
            Token t{ n.nodeName(), n.attributes() };
            t.addLabel( "parent", node.nodeName() );
            tokens.push_back(t);
        }
    }
    return tokens;
}

QVector<Token> Converter::parseDocument(const QDomDocument& doc)
{
    QDomElement doc_element = doc.documentElement();
    QDomNode node{ doc_element.firstChild() };

    QVector<Token> tokens;
    while( !node.isNull() )
    {
        Token tmp{ node.nodeName(), node.attributes() };

        if( node.hasChildNodes() )
        {
            tokens.append( parseNodeChilds(node) );
        }

        tokens.push_back(tmp);
        node = node.nextSibling();
    }

    return tokens;
}

QVector<Token> Converter::parseFile(const QString &file_path)
{
    QFile file{ file_path };
    if( !file.open( QIODevice::ReadOnly ) )
    {
        qWarning() << "Can't open file " << file.fileName();
        return {};
    }

    QString utf8_text{ prepareData(file) };
    QDomDocument doc;
    QString msg;
    int line, col;
    if( !doc.setContent(utf8_text, &msg, &line, &col) )
    {
        file.close();
        qWarning("Invalid XML in file %s. Error = %s, Line = %d, Column = %d", qPrintable(file.fileName()), qPrintable(msg), line, col);
        return {};
    }
    file.close();

    return parseDocument(doc);
}

QString Converter::convertToPrometheus(const QVector<Token> &data)
{
    QString prometheus_data, value;
    for(const auto &token : data)
    {
        prometheus_data.append(token.getMetricName() + "{");
        value = "0";
        const auto& labels = token.getLabels();
        for(auto[key, val] : labels.asKeyValueRange())
        {
            if(key == "value") // Если в метрике есть label с именем value, то сохраняем его, чтобы потом добавить в конец записи
            {
                value = val;
                continue;
            }

            prometheus_data.append(key + " = \"" + val + "\" ");
        }
        prometheus_data.append("}" + value + "\n");
    }
    return prometheus_data;
}

QString Converter::convertToPrometheus(const QString &file_path)
{
    return Converter::convertToPrometheus(Converter::parseFile(file_path));
}