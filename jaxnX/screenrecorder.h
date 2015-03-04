#ifndef SCREENRECORDER_H
#define SCREENRECORDER_H

#define SCREENRECORDER_TIMEOUT 200

#include <QScreen>
#include "imageattributes.h"
#include "dmgmeter.h"

namespace GW2
{
    class ScreenRecorder : public QObject
    {
        Q_OBJECT

    private:
        QList<QScreen*> m_Screens;
        QTimer m_Timer;
        DmgMeter m_DmgMeter;
        ImageAttributes m_ImageAttributes;
        WId m_DesktopWId;
        int m_CurrentScreenIndex;
        int m_Y;
        int m_Height;
        bool m_IsValid;

        static const QString s_Infos[CharacterPattern::ControlTypeCount];

        friend class ScreenRecorderThread;

    public:
        ScreenRecorder(WId desktopId);
        ~ScreenRecorder();

        void StartRecording();
        void StopRecording();

    signals:
        void RequestInfoUpdate(const QString&);

    private slots:
        void Capture();
    };
}

#endif // SCREENRECORDER_H
