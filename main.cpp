#include <QApplication>
#include "mainwindow.h"

#include "type.h"

// http://localhost:8081/metrics

// Доделать фильтр и добавить функцию отправки вектора с токенами для фильтра данных
// Добавить grafaana

// Сделать в фильтре так, чтобы при выборе поля, например метрика, считывалась соответствующая метрика из labels и в QComboBox оставались только ее labels
// Добавить кнопку и/или, которая вставляет QLabel("и") и после нее в новом виджете фильтра можно выбирать из всего списка

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
