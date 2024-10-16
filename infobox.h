#ifndef INFOBOX_H
#define INFOBOX_H

#include <QWidget>
#include <QComboBox>
#include <QList>

#include "info.h"

class InfoBox : public QWidget
{
    Q_OBJECT

    QStringList fields;
    QWidget* last_clicked_wgt;
public:
    explicit InfoBox(QWidget* parent = nullptr, const QList<Info> &info = {});
    QList<Info> getInfo() const;
    bool hasLines() const { return this->findChildren<QWidget*>("FilterLine").size() > 0; }
public slots:
    void addFilterLine();
    void deleteLastFilterLine();
    void setFields(const QStringList &fields);
    void setOperations(const QStringList &operations);
    void deleteAllLines();
signals:
    void addLine();
    void isAddedFilterLine(bool isFilterLine);
    void chooseField(const QString &name);
    void chooseOperation(const QString &name);
    void inputValue(const QString &value);
    void isDeletedLastFilterLine(bool);
    void isClear(bool);
};

#endif // INFOBOX_H
