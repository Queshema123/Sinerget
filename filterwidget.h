#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include <QDialog>
#include <QVBoxLayout>
#include <QComboBox>

#include "token.h"
#include "type.h"

struct Info
{
    QString field, operation, value;
    explicit Info() = default;
    Info(const Info& other): field{other.field}, operation{other.operation}, value{other.value} { }
    Info(const QString& field, const QString& operation, const QString& value) : field{field}, operation{operation}, value{value} {  }
    Info(Info&& other) : field{other.field}, operation{other.operation}, value{other.value} { }
    Info& operator=(const Info& other)
    {
        field = other.field;
        operation = other.operation;
        value = other.value;
        return *this;
    }
};

class FilterWidget : public QDialog
{
    Q_OBJECT

protected:
    void fillFieldsBox(QComboBox* box);
    void fillOperationsBox(const QString& field);
    void add();
    void deleteDataFilter();
    void submit();
    void clear();
public:
    explicit FilterWidget(QWidget* parent = nullptr);
    void addBtns(QVBoxLayout* main_layout);
    QVector<Info> parseFilterInfo();
    QVector<Token> applyFilterToData(const QVector<Info>& filter_info);
    bool matchToken(const Info& info, const Token& token);

    enum class Field{ Metric, Value, Labels };
    static QString getFieldView(Field field);
    static Field getFieldByName(const QString& name);
    static QStringList getOperationsTo(const QString& value);
public slots:
    void setData(const QVector<Token>& tokens);
signals:
    void filteredData(const QVector<Token>& data);
private:
    QVector<Token> data, filtered_data;
    QMap<QString, QString> labels;

    static const QMap<Field, Type*> type_to_field;
    static const QMap<Field, QString> field_views;
};

#endif // FILTERWIDGET_H
