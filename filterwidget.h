#ifndef FILTERWIDGET_H
#define FILTERWIDGET_H

#include <QDialog>
#include <QVBoxLayout>
#include <QAbstractItemModel>

#include "info.h"
#include "infobox.h"

class FilterWidget : public QDialog
{
    Q_OBJECT

    QMap<QString, QList<Info>> filter_templates;
    QMap<QString, QVariant> labels_and_value;
    QList<QWidget*> info_wgts;
    bool addLine;

    void addBtns(QVBoxLayout *main_layout);
public:
    explicit FilterWidget(QWidget *parent = nullptr);
    QPushButton* addBtn(const QString& view, const QString& obj_name, QLayout* layout);
    QStringList getOperations(const QString& field);
    QList<Info> getAllInfo() const;
    void setData(QAbstractItemModel* model);
public slots:
    void applyFilterTemplate(const QString& name);
    void saveFilterTemplate();
    void clearLayout();
    void addOperationAND();
    void deleteLastLine();
    void submit();
    void add();
signals:
    void info(const QList<Info>& info);
    void templateName(const QString& name);
    void status(const QString& status);
};

#endif // FILTERWIDGET_H
