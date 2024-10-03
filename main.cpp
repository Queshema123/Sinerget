#include <QApplication>
#include <QObject>
#include "mainwindow.h"


// http://localhost:8081/metrics

// Отделить логику фильтрации от FilterWidget'а, сделать отдельным файлом структуру info,
// само хранение отфильтрованных данных и их отправку нужно оставить для корректной работы
// Добавить объект класса фильтра в реализацию QSortFilterProxyModel
// Добавить объект класса фильтра в реализацию SearchedWidget'а

// Изменить сохранение шаблона фильтра, вместа хранения виджетов, хранить Info

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QObject::connect(&w, &MainWindow::styleToWidget, [&a](const QString& s){ a.setStyleSheet(s); });
    w.show();
    return a.exec();
}
