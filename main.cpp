#include <QApplication>
#include <QObject>
#include "mainwindow.h"

// http://localhost:8081/metrics

// Добавить обработку операции AND
// Убрать из лога метрики с 0 значением
// Поменять темы

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QObject::connect(&w, &MainWindow::styleToWidget, [&a](const QString& s){ a.setStyleSheet(s); });
    w.show();
    return a.exec();
}
