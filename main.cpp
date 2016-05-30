#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationName("Lisa Co LTD");
    a.setApplicationName("Text editor");

    MainWindow w;
    w.show();

    return a.exec();
}
