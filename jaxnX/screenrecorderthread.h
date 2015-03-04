#ifndef SCREENRECORDERTHREAD_H
#define SCREENRECORDERTHREAD_H

#include <QThread>
#include "screenrecorder.h"
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

    inline void ScreenRecorderThread::run()
    {
        Q_ASSERT(m_pUi);

        qDebug() << "ScreenRecorderThread::run" << "Thread Id:" << QThread::currentThreadId();

        ScreenRecorder r(m_DesktopId);
        const DmgMeter* dmgMeter = &r.m_DmgMeter;
        QObject::connect(&r, SIGNAL(RequestInfoUpdate(QString)), m_pUi->labelInfo, SLOT(setText(QString)));
        QObject::connect(dmgMeter, SIGNAL(RequestDmgUpdate(int)), this, SLOT(UpdateDmg(int)));
        QObject::connect(dmgMeter, SIGNAL(RequestDpsUpdate(double)), this, SLOT(UpdateDps(double)));
        QObject::connect(dmgMeter, SIGNAL(RequestMaxDmgUpdate(int)), this, SLOT(UpdateMaxDmg(int)));
        QObject::connect(m_pUi->actionReset, SIGNAL(triggered()), dmgMeter, SLOT(Reset()));
        QObject::connect(m_pUi->actionAutoReset, SIGNAL(triggered(bool)), dmgMeter, SLOT(SetIsAutoResetting(bool)));
        r.StartRecording();

        exec();
    }

    inline ScreenRecorderThread::ScreenRecorderThread(Ui::MainWindow* ui) :
        m_pUi(ui),
        m_DesktopId(QApplication::desktop()->winId())
    {
    }

    inline ScreenRecorderThread::~ScreenRecorderThread()
    {
    }

    inline void ScreenRecorderThread::UpdateDmg(int dmg)
    {
        m_pUi->labelDmgValue->setText(QString::number(dmg));
    }

    inline void ScreenRecorderThread::UpdateDps(double dps)
    {
        QLabel* dpsLabel = m_pUi->labelDpsValue;
        dpsLabel->setText(QString::number(dps));
        if (dps > DMGMETER_HIGH_DPS_LIMIT)
        {
            dpsLabel->setStyleSheet(DmgMeter::s_UltraStyle);
        }
        else if (dps > DMGMETER_NORMAL_DPS_LIMIT)
        {
            dpsLabel->setStyleSheet(DmgMeter::s_HighStyle);
        }
        else if (dps > DMGMETER_LOW_DPS_LIMIT)
        {
            dpsLabel->setStyleSheet(DmgMeter::s_NormalStyle);
        }
        else
        {
            dpsLabel->setStyleSheet(DmgMeter::s_LowStyle);
        }
    }

    inline void ScreenRecorderThread::UpdateMaxDmg(int maxDmg)
    {
        QLabel* maxDmgLabel = m_pUi->labelMaxDmgValue;
        maxDmgLabel->setText(QString::number(maxDmg));
        if (maxDmg > DMGMETER_HIGH_MAX_DMG_LIMIT)
        {
            maxDmgLabel->setStyleSheet(DmgMeter::s_UltraStyle);
        }
        else if (maxDmg > DMGMETER_NORMAL_MAX_DMG_LIMIT)
        {
            maxDmgLabel->setStyleSheet(DmgMeter::s_HighStyle);
        }
        else if (maxDmg > DMGMETER_LOW_MAX_DMG_LIMIT)
        {
            maxDmgLabel->setStyleSheet(DmgMeter::s_NormalStyle);
        }
        else
        {
            maxDmgLabel->setStyleSheet(DmgMeter::s_LowStyle);
        }
    }
}


#endif // SCREENRECORDERTHREAD_H
