#include "mainwindow.h"
#include "BFEApplication.h"

int main(int argc, char *argv[])
{
    BFEApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
