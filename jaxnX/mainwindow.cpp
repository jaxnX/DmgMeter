#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_configurator.h"
#include <QUrl>
#include <QDesktopServices>
#include <QtDebug>
#include "screenrecorder.h"
#include "settings.h"

using namespace GW2;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_Configurator(this)
{
    ui->setupUi(this);
    ui->toolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    this->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);

    QObject::connect(ui->actionEnableTransparency, SIGNAL(triggered(bool)), this, SLOT(EnableTransparency(bool)));
    QObject::connect(ui->actionHelp, SIGNAL(triggered()), this, SLOT(LinkToWebsite()));
    QObject::connect(ui->actionConfig, SIGNAL(triggered()), &m_Configurator, SLOT(exec()));

    ScreenRecorder* screenRecorder = new ScreenRecorder;
    DmgMeter* dmgMeter = &screenRecorder->GetDmgMeter();
    screenRecorder->moveToThread(&m_ScreenRecorderThread);

    Ui::Configurator* uiConfig = m_Configurator.ui;
    dmgMeter->moveToThread(&m_ScreenRecorderThread);

    QObject::connect(&m_ScreenRecorderThread, SIGNAL(finished()), screenRecorder, SLOT(deleteLater()));
    QObject::connect(&m_ScreenRecorderThread, SIGNAL(started()), screenRecorder, SLOT(StartRecording()));
    QObject::connect(screenRecorder, SIGNAL(RequestInfoUpdate(QString)), ui->labelInfo, SLOT(setText(QString)));
    QObject::connect(dmgMeter, SIGNAL(RequestTimeUpdate(int)), this, SLOT(UpdateTime(int)));
    QObject::connect(dmgMeter, SIGNAL(RequestDmgUpdate(unsigned long long)), this, SLOT(UpdateDmg(unsigned long long)));
    QObject::connect(dmgMeter, SIGNAL(RequestDpsUpdate(int)), this, SLOT(UpdateDps(int)));
    QObject::connect(dmgMeter, SIGNAL(RequestMaxDmgUpdate(int)), this, SLOT(UpdateMaxDmg(int)));
    QObject::connect(ui->actionReset, SIGNAL(triggered()), dmgMeter, SLOT(Reset()));
    QObject::connect(ui->actionAutoReset, SIGNAL(triggered(bool)), dmgMeter, SLOT(SetIsAutoResetting(bool)));
    QObject::connect(uiConfig->comboBoxScreenshots, SIGNAL(currentIndexChanged(QString)), screenRecorder, SLOT(SetScreenshotsPerSecond(QString)));
    QObject::connect(uiConfig->comboBoxUpdates, SIGNAL(currentIndexChanged(QString)), dmgMeter, SLOT(SetUpdatesPerSecond(QString)));
    QObject::connect(uiConfig->comboBoxSecondsInCombat, SIGNAL(currentIndexChanged(QString)), dmgMeter, SLOT(SetSecondsInCombat(QString)));
    QObject::connect(uiConfig->comboBoxConsideredLines, SIGNAL(currentIndexChanged(QString)), dmgMeter, SLOT(SetConsideredLineCount(QString)));
    QObject::connect(uiConfig->pushButtonReset, SIGNAL(clicked(bool)), &m_Configurator, SLOT(RestoreDefaults()));

    dmgMeter->SetUpdatesPerSecond(uiConfig->comboBoxUpdates->currentText());
    dmgMeter->SetSecondsInCombat(uiConfig->comboBoxSecondsInCombat->currentText());
    dmgMeter->SetConsideredLineCount(uiConfig->comboBoxConsideredLines->currentText());

    m_ScreenRecorderThread.start();

    Settings::ReadSettings<QMainWindow>(this);

    // Start screenshot timer from separate thread
    const int oldIndex = uiConfig->comboBoxScreenshots->currentIndex();
    uiConfig->comboBoxScreenshots->setCurrentIndex((uiConfig->comboBoxScreenshots->currentIndex() + 1) % uiConfig->comboBoxScreenshots->count());
    uiConfig->comboBoxScreenshots->setCurrentIndex(oldIndex);
}

MainWindow::~MainWindow()
{
    m_ScreenRecorderThread.quit();
    Settings::WriteSettings<QMainWindow>(this);
    if (!m_ScreenRecorderThread.wait(1000))
    {
        // Failed to exit thread => terminate it.
        m_ScreenRecorderThread.terminate();
        m_ScreenRecorderThread.wait();
    }

    delete ui;
}

void MainWindow::EnableTransparency(bool isAlmostTransparent)
{
    if (isAlmostTransparent)
    {
        this->setWindowOpacity(0.4);
    }
    else
    {
        this->setWindowOpacity(1.0);
    }
}

void MainWindow::LinkToWebsite()
{
    QDesktopServices::openUrl(QUrl(MAINWINDOW_WEBSITE_URL));
}

void MainWindow::UpdateTime(int timeInMsecs)
{
    int secs = timeInMsecs / 1000;
    int mins = secs / 60;
    secs -= mins * 60;
    int hours = mins / 60;
    mins -= hours * 60;
    QString time;
    if (hours > 0)
    {
        time += QString::number(hours) + "h ";
    }

    if (mins > 0)
    {
        time += QString::number(mins) + "m ";
    }

    time += QString::number(secs) + "s";
    ui->labelTimeValue->setText(time);
}

void MainWindow::UpdateDmg(unsigned long long dmg)
{
    ui->labelDmgValue->setText(QString::number(dmg));
}

void MainWindow::UpdateDps(int dps)
{
    QLabel* dpsLabel = ui->labelDpsValue;
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

void MainWindow::UpdateMaxDmg(int maxDmg)
{
    QLabel* maxDmgLabel = ui->labelMaxDmgValue;
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
