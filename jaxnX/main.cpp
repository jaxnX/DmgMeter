#include "mainwindow.h"
#include <QApplication>
#include <QtDebug>

int main(int argc, char *argv[])
{
    qDebug() << "main:" << "Thread Id:" << QThread::currentThreadId();

    QApplication a(argc, argv);
    GW2::MainWindow w;
    w.show();

    return a.exec();
}
