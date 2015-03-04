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

#include <QTimer>
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
}

#endif // DMGMETER_H
