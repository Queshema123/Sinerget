#include "mainwindow.h"
#include <QApplication>

// http://localhost:8081/metrics

// Доделать фильтр и добавить функцию отправки вектора с токенами для фильтра данных
// Добавить grafaana

// Сделать в фильтре так, чтобы при выборе поля, например метрика, считывалась соответствующая метрика из labels и в QComboBox оставались только ее labels
// Добавить кнопку и/или, которая вставляет QLabel("и") и после нее виджеты фильтра( операции и все данные сбрасываются)
// Добавить сбор инфо с виджетов и их фильтрацию
// Добавить отображение данных в ServerWidget

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
