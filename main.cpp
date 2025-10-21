#include <QApplication>
#include "mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication newApp(argc, argv);
    MainWindow newWindow;
    newWindow.show();

    return newApp.exec();
}
