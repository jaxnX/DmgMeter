#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define MAINWINDOW_WEBSITE_URL "http://www.gw2dmg-meter.de/howto.html"

#include <QMainWindow>
#include <QUrl>
#include <QDesktopServices>
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

    inline void MainWindow::EnableOpacity(bool isOpaque)
    {
        if (isOpaque)
        {
            this->setWindowOpacity(0.4);
        }
        else
        {
            this->setWindowOpacity(1.0);
        }
    }

    inline void MainWindow::LinkToWebsite()
    {
        QDesktopServices::openUrl(QUrl(MAINWINDOW_WEBSITE_URL));
    }
}

#endif // MAINWINDOW_H
