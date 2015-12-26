#include "dmgmeter.h"
#include <QString>

using namespace GW2;

const QString DmgMeter::s_LowStyle = "color: rgb(255, 255, 255);";
const QString DmgMeter::s_NormalStyle = "color: rgb(0, 255, 0);";
const QString DmgMeter::s_HighStyle = "color: rgb(255, 165, 0);";
const QString DmgMeter::s_UltraStyle = "color: rgb(255, 128, 128);";

void DmgMeter::SetUpdatesPerSecond(const QString& updatesPerSecond)
{
    if (updatesPerSecond == "max")
    {
        // Update as fast es possible
        m_TimeoutInMsec = 1;
    }
    else
    {
        m_TimeoutInMsec = 1000 / updatesPerSecond.toInt();
    }
}

void DmgMeter::SetSecondsInCombat(const QString& secondsInCombat)
{
    m_SecsInCombat = secondsInCombat.toInt();
}

void DmgMeter::SetConsideredLineCount(const QString& consideredLineCount)
{
    m_Params.resize(consideredLineCount.toInt());
    m_OldParams.resize(m_Params.size());
}

void DmgMeter::EvaluateImage(const QImage& image, const ImageAttributes& imageAttributes)
{
    int offset = 0;
    int offsetOld = 0;
    const int paramCount = m_Params.size();
    for (int i = 0; i < paramCount; ++i)
    {
        const QString params = m_Reader.ReadLineFromBottom(image, imageAttributes, paramCount, i);
        if (params == "")
        {
            ++offset;
            ++offsetOld;
        }
        else
        {
            m_Params[i - offset] = params;
            if (m_OldParams[i - offsetOld] != params)
            {
                // Found valid difference, evaluate
                EvaluateLine(params);
                ++offsetOld;
            }
        }
    }

    m_Params.swap(m_OldParams);
    if (m_IsActive)
    {
        emit RequestTimeUpdate(m_ElapsedTimeSinceCombatInMsec + m_TimeSinceCombat.elapsed());
    }
}

void DmgMeter::Reset(bool emitSignals)
{
    m_Dmg = 0;
    m_Dps = 0;
    m_MaxDmg = 0;
    m_ElapsedTimeSinceCombatInMsec = 0;
    m_TimeSinceCombat.start();
    if (emitSignals)
    {
        emit RequestTimeUpdate(0);
        emit RequestDmgUpdate(m_Dmg);
        emit RequestDpsUpdate(m_Dps);
        emit RequestMaxDmgUpdate(m_MaxDmg);
    }
}

void DmgMeter::SetIsAutoResetting(bool isAutoResetting)
{
    m_IsAutoResetting = isAutoResetting;
    if (isAutoResetting && (m_TimeSinceEvaluation.elapsed() / 1000.0f) >= m_SecsInCombat)
    {
        Reset(false);
    }
}

DmgMeter::DmgMeter() :
    m_Timer(this),
    m_ElapsedTimeSinceCombatInMsec(0),
    m_Dps(0),
    m_Dmg(0),
    m_MaxDmg(0),
    m_TimeoutInMsec(0),
    m_SecsInCombat(0),
    m_IsActive(false),
    m_IsAutoResetting(false)
{
    QObject::connect(&m_Timer, SIGNAL(timeout()), this, SLOT(ComputeDps()));

//    ImageAttributes a;
//    QImage image("../../Screenshots/screen2png.png");

//    m_Reader.UpdateImageAttributes(a, image);
//    m_Reader.Read(image, a);
//    m_Reader.ReadLineFromBottom(image, a, 0);
}

DmgMeter::~DmgMeter()
{
}

void DmgMeter::ComputeDps()
{
    const double elapsedSecsSinceCombat = (m_ElapsedTimeSinceCombatInMsec + m_TimeSinceCombat.elapsed()) / 1000.0f;
    const double elapsedSecsSinceEvaluation = m_TimeSinceEvaluation.elapsed() / 1000.0f;
    m_Dps = elapsedSecsSinceCombat == 0.0 ? m_Dmg : m_Dmg / elapsedSecsSinceCombat; // Prevent division by zero
    emit RequestDpsUpdate(m_Dps);
    if (elapsedSecsSinceEvaluation >= m_SecsInCombat)
    {
        // No data received since m_SecsInCombat. End evaluation
        m_Timer.stop();
        m_ElapsedTimeSinceCombatInMsec += m_TimeSinceCombat.elapsed();
        emit RequestTimeUpdate(m_ElapsedTimeSinceCombatInMsec);
        m_IsActive = false;
        if (m_IsAutoResetting)
        {
            Reset(false);
        }
    }
}

void DmgMeter::EvaluateLine(const QString& params)
{
    const int dmg = ComputeDmg(params);
    if (dmg > m_MaxDmg)
    {
        // New max dmg found, set it as max dmg
        UpdateMaxDmg(dmg);
    }

#ifdef DMGMETER_DEBUG
    if (dmg < 100)
    {
        // Dmg low, could be wrong
        qDebug() << "Adding dmg:" << dmg;
    }
#endif // DMGMETER_DEBUG

    m_Dmg += dmg;
    m_TimeSinceEvaluation.start();
    emit RequestDmgUpdate(m_Dmg);
    if (!m_IsActive)
    {
        // Evaluation starts, configure timer and start
        StartEvaluation();
    }
}

int DmgMeter::ComputeDmg(const QString& dmgStr)
{
    unsigned int dmg = 0;
    const int size = dmgStr.size();
    for (int i = 0; i < size; ++i)
    {
        const QChar& c = dmgStr[i];
        if (c.isDigit())
        {
            // Param is digit, append it as dmg
            dmg *= 10;
            dmg += c.digitValue();
        }
    }

    return dmg;
}

void DmgMeter::UpdateMaxDmg(int dmg)
{
    m_MaxDmg = dmg;
    emit RequestMaxDmgUpdate(m_MaxDmg);
}

void DmgMeter::StartEvaluation()
{
    m_IsActive = true;
    m_TimeSinceCombat.start();
    m_Timer.start(m_TimeoutInMsec);
    m_Dps = m_Dmg;
}
