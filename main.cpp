#include <QApplication>
#include "mainwindow.h"
#include "subwindow.h"
#include"server.h"
#include"log.h"
#include<QTimer>
using namespace std;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    int ret;
    ret = a.exec();
    return ret;
}
