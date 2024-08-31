#include <QString>
#include <QMultiMap>
#include <QDomNamedNodeMap>

#ifndef _TOKEN_H_
#define _TOKEN_H_

class Token
{
    QString metric_name;
    QMultiMap<QString, QString> labels;
public:
    explicit Token(const QString &metric_name = "", const QMap<QString, QString> &labels = {});
    Token(const QString &metric_name, const QDomNamedNodeMap &labels);
    Token(const Token& other);
    Token(Token&& other);
    Token& operator=(const Token& other);

    friend bool operator==(const Token& left, const Token& right);
    friend bool operator!=(const Token& left, const Token& right);

    void changeKeyName(const QString &old_name, const QString &new_name);
    void addLabels(const QMap<QString, QString> &labels);
    void addLabels(const QDomNamedNodeMap &labels);
    void deleteLabel(const QString& name);

    inline QString getMetricName() const { return metric_name; }
    inline const QMultiMap<QString, QString>& getLabels() const { return labels; }

    inline void setMetricName(const QString &name) { metric_name = name; }
    void addLabel(const QString &name, const QString &value);
};

#endif // _TOKEN_H_