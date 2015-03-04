#include "screenrecorderthread.h"
#include "screenrecorder.h"
#include <QDesktopWidget>
#include <QtDebug>

using namespace GW2;

void ScreenRecorderThread::run()
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

ScreenRecorderThread::ScreenRecorderThread(Ui::MainWindow* ui) :
    m_pUi(ui),
    m_DesktopId(QApplication::desktop()->winId())
{
}

ScreenRecorderThread::~ScreenRecorderThread()
{
}

void ScreenRecorderThread::UpdateDmg(int dmg)
{
    m_pUi->labelDmgValue->setText(QString::number(dmg));
}

void ScreenRecorderThread::UpdateDps(double dps)
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

void ScreenRecorderThread::UpdateMaxDmg(int maxDmg)
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
