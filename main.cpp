#include <QApplication>
#include "mainwindow.h"


// http://localhost:8081/metrics

// Отделить логику фильтрации от FilterWidget'а, сделать отдельным файлом структуру info,
// само хранение отфильтрованных данных и их отправку нужно оставить для корректной работы
// Добавить объект класса фильтра в реализацию QSortFilterProxyModel
// Добавить объект класса фильтра в реализацию SearchedWidget'а
// Добавить возможность сохранение filterLine фильтра с именованием их

// Добавить смену языка виджетов(надписи на кнопках и тд) ( инструмент по смене языка )
// Подобрать размер окна
// Добавить status bar
// Добавить смену на темную тему/светлую ( таблица стилей )

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
