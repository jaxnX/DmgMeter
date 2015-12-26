#include "mainwindow.h"
#include <QApplication>
#include <QtDebug>

int main(int argc, char *argv[])
{
#ifdef QT_DEBUG
    qDebug() << "main:" << "thread id" << QThread::currentThreadId();
#endif // QT_DEBUG

    QApplication a(argc, argv);
    GW2::MainWindow w;
    w.show();

    return a.exec();
}
