#ifndef DMGMETER_H
#define DMGMETER_H

//#define DMGMETER_DEBUG

#define DMGMETER_UPDATE_IN_MSEC 1000
#define DMGMETER_RESET_IN_SEC 3
#define DMGMETER_LOW_DPS_LIMIT 2000.0
#define DMGMETER_NORMAL_DPS_LIMIT 4000.0
#define DMGMETER_HIGH_DPS_LIMIT 6000.0
#define DMGMETER_LOW_MAX_DMG_LIMIT 2000
#define DMGMETER_NORMAL_MAX_DMG_LIMIT 4500
#define DMGMETER_HIGH_MAX_DMG_LIMIT 10000

#include <QString>
#include <QThread>
#include <QTimer>
#include <QDebug>
#include "imagereader.h"

namespace GW2
{
    class DmgMeter : public QObject
    {
        Q_OBJECT

    private:
        QVector<QString> m_OldParams;
        QVector<QString> m_Params;
        QTimer m_Timer;
        ImageReader m_Reader;
        double m_Dps;
        int m_TimeSinceCombat;
        int m_TimeSinceEvaluation;
        int m_Dmg;
        int m_MaxDmg;
        bool m_IsActive;
        bool m_IsAutoResetting;

        void EvaluateLine(const QString& params);

        friend class ScreenRecorder;

    public:
        static const QString s_LowStyle;
        static const QString s_NormalStyle;
        static const QString s_HighStyle;
        static const QString s_UltraStyle;

        DmgMeter();
        ~DmgMeter();

        bool IsActive() const;

    signals:
        void RequestDmgUpdate(int);
        void RequestDpsUpdate(double);
        void RequestMaxDmgUpdate(int);

    private slots:
        void ComputeDps();

    public slots:
        void EvaluateImage(const QImage& image, const ImageAttributes& imageAttributes);
        void Reset(bool emitSignals = true);
        void SetIsAutoResetting(bool isAutoResetting);
    };

    inline void DmgMeter::EvaluateLine(const QString& params)
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

    inline DmgMeter::DmgMeter() :
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

    inline DmgMeter::~DmgMeter()
    {
    }

    inline bool DmgMeter::IsActive() const
    {
        return m_IsActive;
    }

    inline void DmgMeter::ComputeDps()
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

    inline void DmgMeter::EvaluateImage(const QImage& image, const ImageAttributes& imageAttributes)
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

    inline void DmgMeter::Reset(bool emitSignals)
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

    inline void DmgMeter::SetIsAutoResetting(bool isAutoResetting)
    {
        m_IsAutoResetting = isAutoResetting;
        if (isAutoResetting && m_TimeSinceEvaluation >= 3)
        {
            Reset(false);
        }
    }
}

#endif // DMGMETER_H
