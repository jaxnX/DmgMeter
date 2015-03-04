#include "dmgmeter.h"
#include <QString>
#include <QDebug>

using namespace GW2;

const QString DmgMeter::s_LowStyle = "color: rgb(255, 255, 255);";
const QString DmgMeter::s_NormalStyle = "color: rgb(0, 255, 0);";
const QString DmgMeter::s_HighStyle = "color: rgb(255, 165, 0);";
const QString DmgMeter::s_UltraStyle = "color: rgb(255, 128, 128);";


void DmgMeter::EvaluateLine(const QString& params)
{
    int dmg = 0;
    const int size = params.size();
    for (int i = 0; i < size; ++i)
    {
        const QChar& c = params[i];
        if (c.isDigit())
        {
            // Param is digit, append it as dmg
            dmg *= 10;
            dmg += c.digitValue();
        }
    }

    if (dmg > m_MaxDmg)
    {
        // New max dmg found, set it as max dmg
        m_MaxDmg = dmg;
        emit RequestMaxDmgUpdate(m_MaxDmg);
    }

#ifdef DMGMETER_DEBUG
    if (dmg < 100)
    {
        // Dmg low, could be wrong
        qDebug() << "Adding dmg:" << dmg;
    }
#endif // DMGMETER_DEBUG

    m_Dmg += dmg;
    m_TimeSinceEvaluation = 0;
    emit RequestDmgUpdate(m_Dmg);
    if (!m_IsActive)
    {
        // Evaluation starts, configure timer and start
        m_IsActive = true;
        m_Timer.start(DMGMETER_UPDATE_IN_MSEC);
        m_Dps = m_Dmg;
    }
}

DmgMeter::DmgMeter() :
    m_OldParams(IMAGEATTRIBUTES_MAX_CONSIDERED_LINE_COUNT),
    m_Params(IMAGEATTRIBUTES_MAX_CONSIDERED_LINE_COUNT),
    m_Dps(0.0f),
    m_TimeSinceCombat(0),
    m_TimeSinceEvaluation(0),
    m_Dmg(0),
    m_MaxDmg(0),
    m_IsActive(false),
    m_IsAutoResetting(false)
{
    QObject::connect(&m_Timer, SIGNAL(timeout()), this, SLOT(ComputeDps()));

//        ImageAttributes a;
//        QImage image("../Images/gw2InterfaceNormal.png");
//        m_Reader.UpdateImageAttributes(a, image);
//        m_Reader.Read(image, ImageAttributes());
//        m_Reader.ReadLineFromBottom(image, ImageAttributes(), 0);
}

DmgMeter::~DmgMeter()
{
}

bool DmgMeter::IsActive() const
{
    return m_IsActive;
}

void DmgMeter::ComputeDps()
{
    ++m_TimeSinceCombat;
    ++m_TimeSinceEvaluation;
    m_Dps = m_Dmg / m_TimeSinceCombat;
    emit RequestDpsUpdate(m_Dps);
    if (m_TimeSinceEvaluation >= 3)
    {
        // No data received since evaluation time. End evaluation
        m_Timer.stop();
        m_IsActive = false;
        if (m_IsAutoResetting)
        {
            Reset(false);
        }
    }
}

void DmgMeter::EvaluateImage(const QImage& image, const ImageAttributes& imageAttributes)
{
//        qDebug() << "DmgMeter::EvaluateImage: thread id" << QThread::currentThreadId();
    int offset = 0;
    int offset2 = 0;
    int paramCount = IMAGEATTRIBUTES_MAX_CONSIDERED_LINE_COUNT;
    for (int i = 0; i < paramCount; ++i)
    {
        const QString params = m_Reader.ReadLineFromBottom(image, imageAttributes, i);
        if (params == "")
        {
            ++offset;
            ++offset2;
        }
        else
        {
            m_Params[i - offset2] = params;
            if (m_OldParams[i - offset] != params)
            {
                // Found valid difference, evaluate
                EvaluateLine(params);
                ++offset;
            }
        }
    }

    m_Params.swap(m_OldParams);
}

void DmgMeter::Reset(bool emitSignals)
{
    m_Dmg = 0;
    m_Dps = 0;
    m_MaxDmg = 0;
    m_TimeSinceCombat = 0;
    if (emitSignals)
    {
        emit RequestDmgUpdate(m_Dmg);
        emit RequestDpsUpdate(m_Dps);
        emit RequestMaxDmgUpdate(m_MaxDmg);
    }
}

void DmgMeter::SetIsAutoResetting(bool isAutoResetting)
{
    m_IsAutoResetting = isAutoResetting;
    if (isAutoResetting && m_TimeSinceEvaluation >= 3)
    {
        Reset(false);
    }
}
