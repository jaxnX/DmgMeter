#ifndef SCREENRECORDER_H
#define SCREENRECORDER_H

#include <QScreen>
#include "imagereader.h"
#include "dmgmeter.h"

namespace GW2
{
    class ScreenRecorder : public QObject
    {
        Q_OBJECT

    signals:
        void RequestInfoUpdate(const QString&);

    public slots:
        void StartRecording();
        void SetScreenshotsPerSecond(const QString& screenshotsPerSecond);

    public:
        ScreenRecorder();
        ~ScreenRecorder();

        inline DmgMeter& GetDmgMeter();

    private slots:
        void Capture();

    private:
        QTimer m_Timer;
        DmgMeter m_DmgMeter;
        ImageReader m_ImageReader;
        ImageAttributes m_ImageAttributes;
        WId m_DesktopWId;
        int m_CurrentScreenIndex;
        int m_Y;
        int m_Height;
        bool m_IsValid;

        static const QString s_Infos[CharacterPattern::ControlTypeCount];
    };

    inline DmgMeter& ScreenRecorder::GetDmgMeter()
    {
        return m_DmgMeter;
    }
}

#endif // SCREENRECORDER_H
