#include "token.h"
#include <QDebug>

Token::Token(const QString &metric_name, const QMap<QString, QString> &labels)
    : metric_name{metric_name}
    , labels{labels}
    , metric_value{"0"}
{}
Token::Token(const Token &other)
    : metric_name{other.metric_name}
    , labels{other.labels}
    , metric_value{other.metric_value}
{}
Token::Token(Token &&other)
    : metric_name{other.metric_name}
    , labels{other.labels}
    , metric_value{other.metric_value}
{}

Token::Token(const QString &metric_name, const QDomNamedNodeMap &labels)
    : metric_name{metric_name}
    , metric_value{"0"}
{
    addLabels(labels);
}

void Token::changeKeyName(const QString &old_name, const QString &new_name)
{
    QString val{labels.find(old_name).value()};
    labels.erase(labels.find(old_name));
    labels.insert(new_name, val);
}

Token &Token::operator=(const Token &other)
{
    if (this == &other)
        return *this;
    this->metric_name = other.metric_name;
    this->metric_value = other.metric_value;
    this->labels = other.labels;
    return *this;
}

void Token::deleteLabel(const QString &name)
{
    auto it = labels.find(name);
    if (it == labels.end()) {
        qWarning() << "Token has not this key";
        return;
    }

    labels.erase(it);
}

void Token::addLabel(const QString &name, const QString &value)
{
    if (name == "") {
        qWarning() << "Can't add label with empty name";
        return;
    }
    labels.insert(name, value);
}

void Token::addLabels(const QMap<QString, QString> &labels)
{
    for (auto it = labels.begin(); it != labels.end(); ++it) {
        this->addLabel(it.key(), it.value());
    }
}

void Token::addLabels(const QDomNamedNodeMap &labels)
{
    for (int i{0}; i < labels.size(); ++i)
        this->labels.insert(labels.item(i).nodeName(), labels.item(i).nodeValue());
}

bool operator==(const Token &left, const Token &right)
{
    return left.metric_name == right.metric_name && left.labels == right.labels;
}

bool operator!=(const Token &left, const Token &right)
{
    return !(left == right);
}
