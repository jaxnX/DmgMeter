#ifndef SCREENRECORDERTHREAD_H
#define SCREENRECORDERTHREAD_H

#include <QThread>
#include "ui_mainwindow.h"

namespace GW2
{
    class ScreenRecorderThread : public QThread
    {
        Q_OBJECT

    private:
        Ui::MainWindow* m_pUi;
        WId m_DesktopId;

        void run();

    public:
        ScreenRecorderThread(Ui::MainWindow* ui);
        ~ScreenRecorderThread();

    private slots:
        void UpdateDmg(int dmg);
        void UpdateDps(double dps);
        void UpdateMaxDmg(int maxDmg);
    };
}


#endif // SCREENRECORDERTHREAD_H
