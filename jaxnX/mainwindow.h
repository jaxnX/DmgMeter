#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define MAINWINDOW_WEBSITE_URL "http://www.gw2dmg-meter.de/howto.html"

#include <QMainWindow>
#include "screenrecorderthread.h"

namespace Ui {
    class MainWindow;
}

namespace GW2
{
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    private:
        Ui::MainWindow *ui;
        ScreenRecorderThread m_RecorderThread;

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

    private slots:
        void EnableOpacity(bool isOpaque);
        void LinkToWebsite();
    };
}

#endif // MAINWINDOW_H
