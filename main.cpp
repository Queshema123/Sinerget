#include <QApplication>
#include "mainwindow.h"


// http://localhost:8081/metrics


// Добавить grafaana
// Добавить возможность сохранение filterLine фильтра с именованием их

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
