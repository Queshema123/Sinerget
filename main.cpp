#include <QApplication>
#include <QObject>
#include "mainwindow.h"

// ���� � �����
// �������� ����� ���������� � �� (TSDB)

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QObject::connect(&w, &MainWindow::themeStyleSheet, [&a](const QString& s){ a.setStyleSheet(s); });
    w.setupSettings();
    w.show();
    return a.exec();
}
