#ifndef SCREENRECORDER_H
#define SCREENRECORDER_H

#define SCREENRECORDER_TIMEOUT 200

#include <QGuiApplication>
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QThread>
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

    inline ScreenRecorder::ScreenRecorder(WId desktopId) :
        m_Screens(QGuiApplication::screens()),
        m_DesktopWId(desktopId),
        m_CurrentScreenIndex(0),
        m_Y(0),
        m_IsValid(false)
    {
        if (!m_Screens.isEmpty())
        {
            m_Height = m_Screens[0]->size().height();
        }

        qDebug() << "Resolution:" << m_Screens[0]->size();

//        QPixmap p = m_Screens->grabWindow(desktopId/*, 0, m_Y, m_ImageAttributes.GetMaxWidth(), m_ImageAttributes.GetMaxHeight()*/);
//        p.save("../Images/gw2InterfaceLargerMidLargeTextSize.png");
    }

    inline ScreenRecorder::~ScreenRecorder()
    {
    }

    inline void ScreenRecorder::StartRecording()
    {
        QObject::connect(&m_Timer, SIGNAL(timeout()), this, SLOT(Capture()));
        m_Timer.start(SCREENRECORDER_TIMEOUT);
    }

    inline void ScreenRecorder::StopRecording()
    {
        m_Timer.stop();
    }

    inline void ScreenRecorder::Capture()
    {
        Q_ASSERT(m_CurrentScreenIndex >= 0 && m_CurrentScreenIndex <= m_Screens.size());

        QScreen* screen = m_Screens[m_CurrentScreenIndex];
        const int x = 0;
        const int width = IMAGEATTRIBUTES_MAX_WIDTH;
        const QImage image = screen->grabWindow(m_DesktopWId, x, m_Y, width, m_Height).toImage();
        const ImageAttributes::InterfaceSize oldInterfaceSize = m_ImageAttributes.GetInterfaceSize();
        ImageReader& reader = m_DmgMeter.m_Reader;
        CharacterPattern::ControlType controlType = reader.UpdateImageAttributes(m_ImageAttributes, image);
        switch(controlType)
        {
            case CharacterPattern::InvalidControl:
            {
                if (!m_IsValid)
                {
                    // Chat block position not found, change screen
                    m_CurrentScreenIndex = (m_CurrentScreenIndex + 1) % m_Screens.size();
                }

                m_IsValid = false;
                m_Y = 0;
                m_Height = m_Screens[m_CurrentScreenIndex]->size().height();
                emit RequestInfoUpdate(s_Infos[CharacterPattern::InvalidControl]);
                break;
            }
            case CharacterPattern::VisibleControl:
            {
                // Update y pos and height
                const int endY = reader.GetCharacterGrid().GetYOffset() + IMAGEATTRIBUTES_CONTROL_MAX_HEIGHT;
                if (!m_IsValid)
                {
                    // Control type first time valid since screen or chat block position change, update y and height
                    m_IsValid = true;
                    m_Y = m_ImageAttributes.GetLastLineY() - (IMAGEATTRIBUTES_MAX_CONSIDERED_LINE_COUNT - 1) * m_ImageAttributes.GetLineDistance();
                    m_Height = endY - m_Y;
                }

                bool isLogScrolledDown = reader.IsLogScrolledDown(image, m_ImageAttributes);
                if (!isLogScrolledDown)
                {
                    // Log is not scrolled down, request scroll down and return
                    RequestInfoUpdate(s_Infos[CharacterPattern::ScrollBarControl]);
                    return;
                }

                if (oldInterfaceSize == m_ImageAttributes.GetInterfaceSize())
                {
                    // Image attributes are valid, evaluate
                    m_DmgMeter.EvaluateImage(image, m_ImageAttributes);
                }

                emit RequestInfoUpdate(s_Infos[CharacterPattern::VisibleControl]);
                return;
            }
            case CharacterPattern::HiddenControl:
            {
                emit RequestInfoUpdate(s_Infos[CharacterPattern::HiddenControl]);
                return;
            }
            default:
            {
                qDebug() << "ScreenRecorder::Capture: Unknown CharacterPattern::ControlType:" << controlType;
                return;
            }
        }
    }
}

#endif // SCREENRECORDER_H
