#include <QApplication>

#include "ui/mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    int result = w.show();
    return result ? result : a.exec();
}
