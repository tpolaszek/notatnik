#include "mainwindow.h"
#include "startscreen.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    StartScreen s(&w);
    s.show();
    return a.exec();
}
