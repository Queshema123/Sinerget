#include "mainwindow.h"
#include <QApplication>

// http://localhost:8081/metrics

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
