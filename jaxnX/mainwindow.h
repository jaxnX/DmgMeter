#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define MAINWINDOW_WEBSITE_URL "http://www.gw2dmg-meter.de/howto.html"

#include <QMainWindow>
#include <QThread>
#include "configurator.h"

namespace Ui
{
    class MainWindow;
}

namespace GW2
{
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

    private slots:
        void EnableTransparency(bool isAlmostTransparent);
        void LinkToWebsite();
        void UpdateTime(int timeInMsecs);
        void UpdateDmg(unsigned long long dmg);
        void UpdateDps(int dps);
        void UpdateMaxDmg(int maxDmg);

    private:
        Ui::MainWindow *ui;
        QThread m_ScreenRecorderThread;
        Configurator m_Configurator;
    };
}

#endif // MAINWINDOW_H
