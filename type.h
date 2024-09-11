#ifndef TYPE_H
#define TYPE_H

#include <QMap>
#include <QString>
#include <QVariant>

class Type
{
public:
    explicit Type() = default;
    virtual bool isContains(const QString &op) const = 0;
    virtual bool compareValue(QVariant left, QVariant right, const QString &operation) const = 0;
    virtual QString getName() const = 0;
    virtual QStringList getOperations() const = 0;
    virtual ~Type() = default;
};

class Number : public Type
{
public:
    Number() = default;
    ~Number() = default;
    enum class Operations { Equal, NotEqual, Greater, Less, GreaterOrEqual, LessOrEqual };

    bool isContains(const QString &op) const override;
    bool compareValue(QVariant left, QVariant right, const QString &operation) const override;
    QString getName() const override;
    QStringList getOperations() const override;
    QString getOperationView(Operations operation) const;

private:
    static const QMap<const Operations, QString> operations_view;
    static const QMap<const Operations, std::function<bool(QVariant l, QVariant r)>>
        operations_compare;
};

class String : public Type
{
public:
    String() = default;
    ~String() = default;
    enum class Operations { StartsWith, EndsWith, Contains };
    bool isContains(const QString &op) const override;
    bool compareValue(QVariant left, QVariant right, const QString &operation) const override;
    QString getName() const override;
    QStringList getOperations() const override;
    QString getOperationView(Operations operation) const;

private:
    static const QMap<const Operations, QString> operations_view;
    static const QMap<const Operations, std::function<bool(QString l, QString r)>> operations_compare;
};

#endif // TYPE_H
