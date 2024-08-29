#include "mainwindow.h"
#include "converter.h"
#include <string>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), server{nullptr}, port{8081}
{
    server = new Server(port, this);
}

MainWindow::~MainWindow() {}
