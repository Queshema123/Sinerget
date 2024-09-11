#include "filterwidget.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>

const QMap<FilterWidget::Field, Type* > FilterWidget::type_to_field
    = {{FilterWidget::Field::Metric, new String},
       {FilterWidget::Field::Value, new Number},
       {FilterWidget::Field::Labels, new String}};
const QMap<FilterWidget::Field, QString> FilterWidget::field_views
    = {{FilterWidget::Field::Metric, "Метрика"},
       {FilterWidget::Field::Value, "Значение"},
       {FilterWidget::Field::Labels, "Аттрибуты"}};

QPushButton* FilterWidget::addBtn(const QString& view, const QString& obj_name, QLayout* layout)
{
    QPushButton* btn = new QPushButton(view);
    btn->setObjectName(obj_name);
    layout->addWidget(btn);
    return btn;
}

void FilterWidget::addBtns(QVBoxLayout *main_layout)
{
    QWidget *btns_wgt = new QWidget;
    QHBoxLayout *btns_layout = new QHBoxLayout(btns_wgt);
    main_layout->addWidget(btns_wgt, 0, Qt::AlignTop);

    QPushButton *submit_btn = addBtn("Submit", "SubmitButton", btns_layout);
    QPushButton *add_btn    = addBtn("Add",    "AddButton",    btns_layout);
    QPushButton *delete_btn = addBtn("Delete", "DeleteButton", btns_layout);
    QPushButton *clear_btn  = addBtn("Clear",  "ClearButton",  btns_layout);
    QPushButton *and_btn    = addBtn("AND",     "ANDButton",   btns_layout);

    connect(submit_btn, &QPushButton::clicked, this, &FilterWidget::submit);
    connect(delete_btn, &QPushButton::clicked, this, &FilterWidget::deleteDataFilter);
    connect(clear_btn,  &QPushButton::clicked, this, &FilterWidget::clear);
    connect(and_btn,    &QPushButton::clicked, this, &FilterWidget::addOperationAND);
    connect(add_btn,    &QPushButton::clicked, this, [this](){
        if(isSubFilter)
            addSubFilter();
        else
        {
            addFilterLine();
            isSubFilter = true;
        }
    });
}

FilterWidget::FilterWidget(QWidget *parent)
    : QDialog{parent}, section_idx{0}, isSubFilter{false}
{
    setModal(true);
    QVBoxLayout *main_layout = new QVBoxLayout(this);
    addBtns(main_layout);
    filtered_sections.resize(1);
}

QString FilterWidget::getFieldView(Field field)
{
    return (field_views.contains(field)) ? field_views[field] : "Неизвестное поле";
}

FilterWidget::Field FilterWidget::getFieldByName(const QString &name)
{
    return field_views.key(name);
}

QStringList FilterWidget::getOperationsTo(const QString &value)
{
    QRegularExpression re("^[0-9]*$"); // Проверка на число
    if (re.match(value).hasMatch())
        return type_to_field[Field::Value]->getOperations(); // Field::Value - число
    else
        return type_to_field[Field::Metric]->getOperations(); // Field::Metric - строка
    return {};
}

void FilterWidget::fillFieldsBox(QComboBox *box, const QMap<QString, QString>* labels)
{
    if( !labels )
        labels = &this->labels;

    foreach (const QString &name, labels->keys())
        box->addItem(name, section_idx);

    foreach (const QString &name, field_views.values())
        box->addItem(name, section_idx);
}

void FilterWidget::fillOperationsBox(const QString &field)
{
    QComboBox *box = qobject_cast<QComboBox *>(sender())->parentWidget()->findChild<QComboBox *>(
        "Operation");
    box->clear();

    QStringList operations;
    if (getFieldByName(field) == Field::Labels)
        operations = getOperationsTo(field);
    else
        operations = type_to_field.value(getFieldByName(field))->getOperations();

    foreach (const QString &operation, operations) {
        box->addItem(operation);
    }
}

QList<QWidget*> FilterWidget::extractLastFilterLines()
{
    QList<QWidget*> wgts = this->findChildren<QWidget*>();
    QList<QWidget*>::const_iterator and_lbl_it = wgts.cend();
    for(auto it = wgts.cbegin(); it != wgts.cend(); ++it)
    {
        if( (*it)->objectName() == "AndLabel")
            and_lbl_it = it;
    }

    if(and_lbl_it != wgts.end())
        wgts.erase(wgts.cbegin(), and_lbl_it);
    else
        and_lbl_it = wgts.cbegin();

    for(auto it = and_lbl_it; it != wgts.cend(); )
    {
        if( (*it)->objectName() != "FilterLine" )
            it = wgts.erase( it );
        else
            ++it;
    }

    return wgts;
}

void FilterWidget::updateFilterSection()
{
    auto wgts{ extractLastFilterLines() };

    if(filtered_sections[section_idx].size() > 0)
        filtered_sections[section_idx] = applyFilterToData( parseFilterInfo(wgts), filtered_sections[section_idx] );
    else
        filtered_sections[section_idx] = applyFilterToData( parseFilterInfo(wgts), data );
}

void FilterWidget::addSubFilter()
{
    addFilterLine();
    auto line_wgts = this->findChildren<QWidget*>("FilterLine");
    QWidget* line = line_wgts.last();
    auto fieldbox = line->findChild<QComboBox*>("Field");
    fieldbox->clear();
    line->findChild<QComboBox*>("Operation")->clear();

    QMap<QString, QString> lbls;
    foreach(const Token& token, filtered_sections[section_idx] )
        for(auto [key, value] : token.getLabels().asKeyValueRange())
            lbls.insert(key, value);

    fillFieldsBox( fieldbox, &lbls );
}

void FilterWidget::addFilterLine()
{
    QVBoxLayout *main_layout = qobject_cast<QVBoxLayout *>(this->layout());
    QWidget *filter_line_wgt = new QWidget;
    filter_line_wgt->setObjectName("FilterLine");
    main_layout->addWidget(filter_line_wgt);

    QHBoxLayout *filter_line_layout = new QHBoxLayout(filter_line_wgt);
    QComboBox *field_box = new QComboBox;
    QComboBox *operation_box = new QComboBox;
    QLineEdit *input_value_wgt = new QLineEdit;

    field_box->setObjectName("Field");
    operation_box->setObjectName("Operation");
    input_value_wgt->setObjectName("InputValue");

    filter_line_layout->addWidget(field_box);
    filter_line_layout->addWidget(operation_box);
    filter_line_layout->addWidget(input_value_wgt);

    fillFieldsBox(field_box);

    connect(field_box, &QComboBox::currentTextChanged,  this, &FilterWidget::fillOperationsBox);
    connect(input_value_wgt, &QLineEdit::returnPressed, this, &FilterWidget::updateFilterSection);
}

void FilterWidget::deleteDataFilter()
{
    auto lbls = this->findChildren<QWidget*>();
    auto lbl = lbls.last();
    if( lbl->objectName() == "AndLabel")
    {
        this->layout()->removeWidget( lbl );
        lbl->deleteLater();

        filtered_sections.resize( filtered_sections.size() - 1 );
        --section_idx;
        return;
    }

    auto wgts = this->findChildren<QWidget *>("FilterLine");
    if (!wgts.size())
        return;
    QWidget *deleted_wgt = wgts.last();
    this->layout()->removeWidget(deleted_wgt);
    deleted_wgt->deleteLater();

    isSubFilter = ( wgts.size() - 1 == 0) ? false : true;
}

void FilterWidget::clear()
{
    QList<QWidget *> line_wgts = this->findChildren<QWidget *>("FilterLine");
    foreach (QWidget *wgt, line_wgts) {
        wgt->findChild<QComboBox *>("Field")->setCurrentIndex(0);
        wgt->findChild<QComboBox *>("Operation")->setCurrentIndex(0);
        wgt->findChild<QLineEdit *>("InputValue")->clear();
    }

    filtered_sections.resize(1);
    section_idx = 0;
    filtered_sections[section_idx].clear();

    emit filteredData({});
}

QVector<Info> FilterWidget::parseFilterInfo(const QList<QWidget*> &wgts)
{
    QList<QWidget *> line_wgts;
    if( !wgts.size() )
        line_wgts = this->findChildren<QWidget *>("FilterLine");
    else
        line_wgts = wgts;

    QVector<Info> info;
    foreach (QWidget *wgt, line_wgts) {
        Info inf;
        inf.field = wgt->findChild<QComboBox *>("Field")->currentText();
        inf.operation = wgt->findChild<QComboBox *>("Operation")->currentText();
        inf.value = wgt->findChild<QLineEdit *>("InputValue")->text();
        info.push_back(std::move(inf));
    }

    return info;
}

bool FilterWidget::matchToken(const Info &info, const Token &token)
{
    Field f = field_views.key(info.field);
    QStringList vals;
    switch (f) {
    case Field::Labels:
        vals.append(token.getLabels().keys().join(""));
        break;
    case Field::Metric:
        vals.append({token.getMetricName()});
        break;
    case Field::Value:
        vals.append(token.getMetricValue());
        break;
    default:
        break;
    }

    const Type *field_type = type_to_field[f];
    foreach (const QString &str, vals) {
        if (!field_type->compareValue(str, info.value, info.operation))
            return false;
    }

    return true;
}

QVector<Token> FilterWidget::applyFilterToData(const QVector<Info> &filter_info, const QVector<Token>& data)
{
    QVector<Token> filtered_tokens;

    foreach (const Token &token, data) {
        bool isMatch{true};
        foreach (const Info &inf, filter_info) {
            if (!matchToken(inf, token)) {
                isMatch = false;
                break;
            }
        }

        if (isMatch)
            filtered_tokens.push_back(token);
    }

    return filtered_tokens;
}

void FilterWidget::submit()
{
    QVector<Token> filtered_data;
    foreach (const QVector<Token> tokens, filtered_sections) {
        filtered_data.append( tokens.cbegin(), tokens.cend() );
    }

    emit filteredData(filtered_data);
}

void FilterWidget::setData(const QVector<Token> &tokens)
{
    data = tokens;
    for (const auto &token : tokens)
        for (auto [key, value] : token.getLabels().asKeyValueRange())
            labels.insert(key, value);
}

void FilterWidget::addOperationAND()
{
    QVBoxLayout *main_layout = qobject_cast<QVBoxLayout *>(this->layout());
    QLabel* lbl = new QLabel("AND");
    lbl->setObjectName("AndLabel");
    main_layout->addWidget( lbl );
    ++section_idx;
    filtered_sections.resize( section_idx + 1);
    isSubFilter = false;
}
