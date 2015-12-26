#ifndef DMGMETER_H
#define DMGMETER_H

//#define DMGMETER_DEBUG

#define DMGMETER_LOW_DPS_LIMIT 2000.0
#define DMGMETER_NORMAL_DPS_LIMIT 4000.0
#define DMGMETER_HIGH_DPS_LIMIT 6000.0
#define DMGMETER_LOW_MAX_DMG_LIMIT 2000
#define DMGMETER_NORMAL_MAX_DMG_LIMIT 4500
#define DMGMETER_HIGH_MAX_DMG_LIMIT 10000

#include <QTimer>
#include <QTime>
#include "imagereader.h"

namespace GW2
{
    class DmgMeter : public QObject
    {
        Q_OBJECT

    signals:
        void RequestTimeUpdate(int);
        void RequestDmgUpdate(unsigned long long);
        void RequestDpsUpdate(int);
        void RequestMaxDmgUpdate(int);

    public slots:
        void SetUpdatesPerSecond(const QString& updatesPerSecond);
        void SetSecondsInCombat(const QString& secondsInCombat);
        void SetConsideredLineCount(const QString& consideredLineCount);
        void EvaluateImage(const QImage& image, const ImageAttributes& imageAttributes);
        void Reset(bool emitSignals = true);
        void SetIsAutoResetting(bool isAutoResetting);

    public:
        DmgMeter();
        ~DmgMeter();

        inline int GetConsideredLineCount() const;
        inline bool IsActive() const;

        static const QString s_LowStyle;
        static const QString s_NormalStyle;
        static const QString s_HighStyle;
        static const QString s_UltraStyle;

    private slots:
        void ComputeDps();

    private:
        void EvaluateLine(const QString& params);
        int ComputeDmg(const QString& dmgStr);
        void UpdateMaxDmg(int dmg);
        void StartEvaluation();

        QVector<QString> m_OldParams;
        QVector<QString> m_Params;
        QTimer m_Timer;
        QTime m_TimeSinceCombat;
        QTime m_TimeSinceEvaluation;
        ImageReader m_Reader;
        int m_ElapsedTimeSinceCombatInMsec;
        int m_Dps;
        unsigned long long m_Dmg;
        int m_MaxDmg;
        int m_TimeoutInMsec;
        int m_SecsInCombat;
        bool m_IsActive;
        bool m_IsAutoResetting;
    };

    inline int DmgMeter::GetConsideredLineCount() const
    {
        return m_Params.size();
    }

    inline bool DmgMeter::IsActive() const
    {
        return m_IsActive;
    }
}

#endif // DMGMETER_H
