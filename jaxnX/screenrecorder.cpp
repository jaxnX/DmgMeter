#include "screenrecorder.h"
#include <QGuiApplication>
#include <QApplication>
#include <QDesktopWidget>
#include <QtDebug>
#include <QThread>

using namespace GW2;

const QString ScreenRecorder::s_Infos[static_cast<int>(CharacterPattern::ControlTypeCount)] = {
        "Waiting", // Invalid
        "", // Visible
        "Please unhide chat panel", // Hidden
        "Please scroll down log" // Scrollbar
        };

void ScreenRecorder::StartRecording()
{
#ifdef QT_DEBUG
    qDebug() << "ScreenRecorder::StartRecording: thread id" << QThread::currentThreadId();
#endif // QT_DEBUG
    QObject::connect(&m_Timer, SIGNAL(timeout()), this, SLOT(Capture()));
}

void ScreenRecorder::SetScreenshotsPerSecond(const QString& screenshotsPerSecond)
{
    if (screenshotsPerSecond == "max")
    {
        // Record as fast as possible
        m_Timer.start(1);
    }
    else
    {
        const int msec = 1000 / screenshotsPerSecond.toInt();
        m_Timer.start(msec);
    }
}

ScreenRecorder::ScreenRecorder() :
    m_Timer(this),
    m_DesktopWId(QApplication::desktop()->winId()),
    m_CurrentScreenIndex(0),
    m_Y(0),
    m_IsValid(false)
{
    QList<QScreen*> screens = QGuiApplication::screens();
    if (!screens.isEmpty())
    {
        m_Height = screens[0]->size().height();
    }

#ifdef QT_DEBUG
    qDebug() << "Resolution:" << screens[0]->size();
#endif // QT_DEBUG

//        QPixmap p = m_Screens[0]->grabWindow(desktopId/*, 0, m_Y, m_ImageAttributes.GetMaxWidth(), m_ImageAttributes.GetMaxHeight()*/);
//        p.save("../Images/gw2InterfaceLargerMidLargeTextSize.png");
}

ScreenRecorder::~ScreenRecorder()
{
}

void ScreenRecorder::Capture()
{
    QList<QScreen*> screens = QGuiApplication::screens();
    if (m_CurrentScreenIndex >= screens.size())
    {
        // Current screen index out of bounds (can appear, when amount of used screens change)
        m_CurrentScreenIndex = 0;
    }

    QScreen* screen = screens[m_CurrentScreenIndex];
    const int x = 0;
    const int width = IMAGEATTRIBUTES_MAX_WIDTH;
    const QImage image = screen->grabWindow(QApplication::desktop()->winId(), x, m_Y, width, m_Height).toImage();
    const ImageAttributes::InterfaceSize oldInterfaceSize = m_ImageAttributes.GetInterfaceSize();
    CharacterPattern::ControlType controlType = m_ImageReader.UpdateImageAttributes(m_ImageAttributes, image);
    switch(controlType)
    {
        case CharacterPattern::InvalidControl:
        {
            if (!m_IsValid)
            {
                // Chat block position not found, change screen
                m_CurrentScreenIndex = (m_CurrentScreenIndex + 1) % screens.size();
            }

            m_IsValid = false;
            m_Y = 0;
            m_Height = screens[m_CurrentScreenIndex]->size().height();
            emit RequestInfoUpdate(s_Infos[CharacterPattern::InvalidControl]);
            break;
        }
        case CharacterPattern::VisibleControl:
        {
            // Update y pos and height
            const int endY = m_ImageReader.GetCharacterGrid().GetYOffset() + IMAGEATTRIBUTES_CONTROL_MAX_HEIGHT;
            if (!m_IsValid)
            {
                // Control type first time valid since screen or chat block position change, update y and height
                m_IsValid = true;
                m_Y = m_ImageAttributes.GetLastLineY() - (m_DmgMeter.GetConsideredLineCount() - 1) * m_ImageAttributes.GetLineDistance();
                m_Height = endY - m_Y;
            }

            bool isLogScrolledDown = m_ImageReader.IsLogScrolledDown(image, m_ImageAttributes);
            if (!isLogScrolledDown)
            {
                // Log is not scrolled down, request scroll down and return
                emit RequestInfoUpdate(s_Infos[CharacterPattern::ScrollBarControl]);
            }
            else
            {
                if (oldInterfaceSize == m_ImageAttributes.GetInterfaceSize())
                {
                    // Image attributes ared valid, evaluate
                    m_DmgMeter.EvaluateImage(image, m_ImageAttributes);
                }

                emit RequestInfoUpdate(s_Infos[CharacterPattern::VisibleControl]);
            }

            break;
        }
        case CharacterPattern::HiddenControl:
        {
            emit RequestInfoUpdate(s_Infos[CharacterPattern::HiddenControl]);
            break;
        }
        default:
        {
            qDebug() << "ScreenRecorder::Capture: Unknown CharacterPattern::ControlType:" << controlType;
            break;
        }
    }
}
