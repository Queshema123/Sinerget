#include "filterwidget.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QSignalBlocker>
#include <QInputDialog>

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
    QPushButton *and_btn    = addBtn("AND",    "ANDButton",    btns_layout);
    QPushButton *save_btn   = addBtn("Save",   "SaveButton",   btns_layout);

    QComboBox *templates_box = new QComboBox;
    templates_box->setObjectName("FilterTemplatesBox");
    templates_box->addItem("Empty");
    btns_layout->addWidget(templates_box);

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
    connect(save_btn,   &QPushButton::clicked, this, &FilterWidget::saveFilterTemplate);
    connect(this, &FilterWidget::filterTemplate, templates_box, [templates_box](const QString& n){ templates_box->addItem(n); });
    connect(templates_box, &QComboBox::currentTextChanged, this, &FilterWidget::applyFilterTemplate);
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

void FilterWidget::fillFieldsBox(QComboBox *box, const QMap<QString, QString>& labels, int idx)
{
    idx = (idx == -1) ? section_idx : idx;
    QSignalBlocker block{ box };
    foreach (const QString &name, labels.keys())
        box->addItem(name, idx);

    foreach (const QString &name, field_views.values())
        box->addItem(name, idx);
}

void FilterWidget::fillOperationsBox(const QString &field)
{
    QComboBox *box = qobject_cast<QComboBox *>(sender())->parentWidget()->findChild<QComboBox *>(
        "Operation");
    QSignalBlocker block{ box };
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

QList<QWidget*> FilterWidget::extractFilterLines(int idx)
{
    QList<QWidget*> wgts = this->findChildren<QWidget*>();
    QList<QWidget*>::const_iterator and_lbl_it = wgts.cend();
    int i{ 0 };
    for(auto it = wgts.cbegin(); it != wgts.cend(); ++it)
    {
        if(i == idx)
            break;
        if( (*it)->objectName() != "AndLabel")
            ++i;
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

QList<QWidget*> FilterWidget::extractFilter()
{
    QList<QWidget*> wgts = this->findChildren<QWidget*>();

    for(auto it = wgts.cbegin(); it != wgts.cend(); )
    {
        if( (*it)->objectName() != "FilterLine" && (*it)->objectName() != "AndLabel" )
            it = wgts.erase( it );
        else
            ++it;
    }

    return wgts;
}

QMap<QString, QString> FilterWidget::getAllLabels( const QVector<Token>& tokens )
{
    QMap<QString, QString> lbls;
    foreach(const Token& token, tokens )
        for(auto [key, value] : token.getLabels().asKeyValueRange())
            lbls.insert(key, value);
    return lbls;
}

void FilterWidget::updateFilterSection(int idx)
{
    auto wgts{ extractFilterLines(idx) };

    if(filtered_sections[idx].size() > 1)
        filtered_sections[idx] = applyFilterToData( parseFilterInfo(wgts), filtered_sections[idx] );
    else
        filtered_sections[idx] = applyFilterToData( parseFilterInfo(wgts), data );
}

void FilterWidget::refilterInfo()
{
    QObject* f_line = sender()->parent();
    if(f_line->findChild<QLineEdit*>("InputValue")->text().size() == 0 )
        return;

    int idx{ f_line->findChild<QComboBox*>("Field")->currentData().toInt() };
    filtered_sections[idx].clear();
    updateFilterSection(idx);
    QObjectList wgts = sender()->parent()->parent()->children();
    qsizetype i{ wgts.indexOf(f_line) + 1 };
    for( ; i < wgts.size(); ++i )
    {
        if( wgts[i]->objectName() == "AndLabel" )
            break;
        if(wgts[i]->objectName() != "FilterLine")
            continue;
        QComboBox* box = wgts[i]->findChild<QComboBox*>("Field");
        box->clear();
        fillFieldsBox( box, getAllLabels( filtered_sections[idx] ) );
    }
}

void FilterWidget::addSubFilter()
{
    QWidget* line = addFilterLine();
    auto fieldbox = line->findChild<QComboBox*>("Field");
    fieldbox->clear();
    line->findChild<QComboBox*>("Operation")->clear();

    fillFieldsBox( fieldbox, getAllLabels(filtered_sections[section_idx]) );
}

QWidget* FilterWidget::addFilterLine(bool isAddToMainLayout)
{
    QVBoxLayout *main_layout = qobject_cast<QVBoxLayout *>(this->layout());
    QWidget *filter_line_wgt = new QWidget;
    filter_line_wgt->setObjectName("FilterLine");
    if(isAddToMainLayout)
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

    fillFieldsBox(field_box, this->labels);

    connect(field_box,       &QComboBox::currentTextChanged, this,            &FilterWidget::fillOperationsBox);
    connect(field_box,       &QComboBox::currentTextChanged, input_value_wgt, &QLineEdit::clear);
    connect(operation_box,   &QComboBox::currentTextChanged, this,            &FilterWidget::refilterInfo);
    connect(input_value_wgt, &QLineEdit::editingFinished,    this,            &FilterWidget::refilterInfo);

    return filter_line_wgt;
}

void FilterWidget::deleteDataFilter()
{
    auto lbls = this->findChildren<QWidget*>();
    auto lbl = lbls.last();
    if( lbl->objectName() == "AndLabel")
    {
        this->layout()->removeWidget( lbl );
        lbl->deleteLater();

        section_idx = (section_idx - 1 >= 0) ? section_idx-1 : 0;
        filtered_sections.resize( section_idx + 1 );
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
        wgt->findChild<QComboBox *>("Operation")->clear();
        wgt->findChild<QLineEdit *>("InputValue")->clear();
    }

    filtered_sections.resize(1);
    section_idx = 0;
    filtered_sections[section_idx].clear();

    emit filteredData(data);
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
{ // Доработать функцию, не учитывает AND из-за чего некорректно работает
    // Добавить в конейнер объект Info с операцией "AND" и добавить эту обработку в функцию ниже
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

void FilterWidget::copyBoxData(QComboBox* original, QComboBox* copy)
{
    for(int i{0}; i < original->count(); ++i)
        copy->addItem( original->itemText(i) );
    copy->setCurrentText(original->currentText());
}

QWidget* FilterWidget::copyLineWidget(QWidget* line)
{
    QWidget* copy = addFilterLine(false);

    copyBoxData( line->findChild<QComboBox*>("Field"), copy->findChild<QComboBox*>("Field") );
    copyBoxData( line->findChild<QComboBox*>("Operation"), copy->findChild<QComboBox*>("Operation") );
    copy->findChild<QLineEdit*>("InputValue")->setText( line->findChild<QLineEdit*>("InputValue")->text() );
    return copy;
}

void FilterWidget::saveFilterTemplate()
{
    QList<QWidget*> wgts_pointers = extractFilter();
    QList<QWidget*> wgts;
    foreach (QWidget* wgt, wgts_pointers)
    {
        if(wgt->objectName() != "FilterLine")
        {
            QLabel* lbl = new QLabel("AND");
            lbl->setObjectName("AndLabel");
            wgts.append( lbl );
            continue;
        }
        QWidget* line = copyLineWidget(wgt);
        wgts.append( line );
    }

    filter_templates.insert( QInputDialog::getText(this, "Template name", "Input name"),  std::move(wgts) );
    emit filterTemplate( filter_templates.lastKey() );
}

void FilterWidget::clearLayout()
{
    filtered_sections.resize(1);
    section_idx = 0;
    filtered_sections[section_idx].clear();

    QList<QWidget*> wgts = this->findChildren<QWidget*>();
    foreach(QWidget* wgt, wgts)
    {
        if(wgt->objectName() != "FilterLine" || wgt->objectName() != "AndLabel")
            continue;
        this->layout()->removeWidget(wgt);
        wgt->deleteLater();
    }
}

void FilterWidget::applyFilterTemplate(const QString& name)
{
    this->clearLayout();
    QWidgetList wgts = filter_templates[name];
    QVBoxLayout *main_layout = qobject_cast<QVBoxLayout *>(this->layout());
    foreach (QWidget* wgt, wgts)
        main_layout->addWidget(wgt);

    updateFilterSection(0);
}
