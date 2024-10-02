#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include <QComboBox>
#include <QDialog>
#include <QVBoxLayout>

#include "token.h"
#include "type.h"

struct Info
{
    QString field, operation, value;
    explicit Info() = default;
    Info(const Info &other)
        : field{other.field}
        , operation{other.operation}
        , value{other.value}
    {}
    Info(const QString &field, const QString &operation, const QString &value)
        : field{field}
        , operation{operation}
        , value{value}
    {}
    Info(Info &&other)
        : field{other.field}
        , operation{other.operation}
        , value{other.value}
    {}
    Info &operator=(const Info &other)
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
    void fillFieldsBox(QComboBox *box, const QMap<QString, QString>& labels, int idx = -1 );
    void fillOperationsBox(const QString &field);
    void addBtns(QVBoxLayout *main_layout);
    void addSubFilter();
    void deleteDataFilter();
    void submit();
    void clear();
    void addOperationAND();
    void updateFilterSection(int idx);
    void saveFilterTemplate();
    void clearLayout();
public:
    explicit FilterWidget(QWidget *parent = nullptr);

    QPushButton* addBtn(const QString& view, const QString& obj_name, QLayout* layout);
    QList<QWidget*> extractFilterLines(int idx);
    QVector<Info> parseFilterInfo(const QList<QWidget*>& wgts = {});
    QVector<Token> applyFilterToData(const QVector<Info> &filter_info, const QVector<Token>& data);
    QMap<QString, QString> getAllLabels( const QVector<Token>& tokens );
    bool matchToken(const Info &info, const Token &token);
    void copyBoxData(QComboBox* original, QComboBox* copy);
    QList<QWidget*> extractFilter();
    QWidget* copyLineWidget(QWidget* line);
    QWidget* addFilterLine(bool isAddToMainLayout = true);

    enum class Field { Metric, Value, Labels };
    static QString getFieldView(Field field);
    static Field getFieldByName(const QString &name);
    static QStringList getOperationsTo(const QString &value);
public slots:
    void setData(const QVector<Token> &tokens);
    void applyFilterTemplate(const QString& name);
private slots:
    void refilterInfo();
signals:
    void filteredData(const QVector<Token> &data);
    void filterTemplate(const QString& name);
protected:
    QVector<Token> data;
    QMap<QString, QString> labels;
    QMap<QString, QList<QWidget*>> filter_templates;

    QVector<QVector<Token>> filtered_sections;
    int section_idx;
private:
    bool isSubFilter;

    static const QMap<Field, Type* > type_to_field;
    static const QMap<Field, QString> field_views;
};

#endif // FILTERWIDGET_H
