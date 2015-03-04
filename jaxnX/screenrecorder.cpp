#include "screenrecorder.h"
#include <QGuiApplication>
#include <QApplication>
#include <QDesktopWidget>
#include <QtDebug>

using namespace GW2;

const QString ScreenRecorder::s_Infos[static_cast<int>(CharacterPattern::ControlTypeCount)] = {
        "Waiting", // Invalid
        "", // Visible
        "Please unhide chat panel", // Hidden
        "Please scroll down log" // Scrollbar
        };


ScreenRecorder::ScreenRecorder(WId desktopId) :
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

ScreenRecorder::~ScreenRecorder()
{
}

void ScreenRecorder::StartRecording()
{
    QObject::connect(&m_Timer, SIGNAL(timeout()), this, SLOT(Capture()));
    m_Timer.start(SCREENRECORDER_TIMEOUT);
}

void ScreenRecorder::StopRecording()
{
    m_Timer.stop();
}

void ScreenRecorder::Capture()
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
