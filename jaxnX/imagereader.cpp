#include "imagereader.h"
#include <QColor>
#include <QImage>
#include <QtDebug>

using namespace GW2;

ImageReader::ImageReader()
{
}

ImageReader::~ImageReader()
{

}

QString ImageReader::Read(const QImage& image, const ImageAttributes& imageAttributes, int lineCount)
{
    QString result;
    for (int i = 0; i < lineCount; ++i)
    {
        QString line = ReadLineFromBottom(image, imageAttributes, lineCount, i);
        if (line != "")
        {
            result += line;
            result += "\n";
        }
    }

    return result;
}

QString ImageReader::ReadLineFromBottom(const QImage& image, const ImageAttributes& imageAttributes, int lineCount, int lineIndex)
{
#ifdef CHARACTERGRID_DEBUG
    if (m_CharacterGrid.GetImage().size() != image.size() || lineIndex == 0)
    {
        m_CharacterGrid.GetImage() = QImage(image.size(), image.format());
    }
#endif // CHARACTERGRID_DEBUG

    const int lastLineY = imageAttributes.GetLastLineY() - lineIndex * imageAttributes.GetLineDistance() + imageAttributes.GetLineYOffset();
    if (lastLineY < 0 || lastLineY >= image.height())
    {
        // line position invalid, return
        return "";
    }

    Q_ASSERT(lineIndex >= 0 && lineIndex < lineCount);

    QString numbers;
    QRgb rgb = QColor(Qt::black).rgb();
    m_CharacterGrid.SetLowerRgbLimit(CHARACTERGRID_LOWER_RGB_LIMIT);
    int lastValidX = INT_MAX;
    int lastValidCharX = IMAGEATTRIBUTES_LINE_X_START;
    const int lineWidth = IMAGEATTRIBUTES_MAX_WIDTH;
    for (int x = IMAGEATTRIBUTES_LINE_X_START, y = lastLineY; x < lineWidth; ++x)
    {
        const int minValidX = x - imageAttributes.GetCharacterWidth() * 3;
        if (lastValidX < minValidX || lastValidCharX < minValidX)
        {
            // Line numbers all found, break
            break;
        }

        if (!CharacterGrid::IsDeviationTooLarge(image, rgb, x, y))
        {
            continue;
        }

        if (m_CharacterGrid.IsPartOfCharacter(image.pixel(x, y)))
        {
            lastValidCharX = x;
        }

        if (!m_CharacterGrid.IsPartOfNumber(image.pixel(x, y)))
        {
            continue;
        }

        m_CharacterGrid.FindNumber(image, x, y, imageAttributes);
        int offsetAdd = 0;
        if (m_CharacterGrid.GetXOffset() <= x - imageAttributes.GetCharacterWidth())
        {
            // Read more numbers at once, set offset to begin of last completely read number
            offsetAdd = (x - m_CharacterGrid.GetXOffset()) / imageAttributes.GetCharacterWidth() * imageAttributes.GetCharacterWidth();
        }

        const QString number = m_Pattern.ConvertCharGridToCharacter(m_CharacterGrid, imageAttributes, offsetAdd);
        if (number != "")
        {
            lastValidX = x;
            numbers += number;
            x = qMax(x, m_CharacterGrid.GetXOffset() + offsetAdd + imageAttributes.GetCharacterWidth() - 1);
        }
    }

#ifdef CHARACTERGRID_DEBUG
    m_CharacterGrid.GetImage().save("../Images/debugImage.png");
#endif // CHARACTERGRID_DEBUG

    return numbers;
}

CharacterPattern::ControlType ImageReader::UpdateImageAttributes(ImageAttributes& imageAttributes, const QImage& image)
{
#ifdef CHARACTERGRID_DEBUG
    if (m_CharacterGrid.GetImage().size() != image.size())
    {
        m_CharacterGrid.GetImage() = QImage(image.size(), image.format());
    }
#endif // CHARACTERGRID_DEBUG

    m_CharacterGrid.SetLowerRgbLimit(CHARACTERGRID_LOWER_RGB_LIMIT);
    const int height = image.height();
    for (int x = IMAGEATTRIBUTES_CONTROL_X_START, y = 0; y < height; ++y)
    {
        if (!m_CharacterGrid.IsPartOfCharacter(image.pixel(x, y)))
        {
            continue;
        }

        m_CharacterGrid.FindCharacter(image, x, y, imageAttributes);
        CharacterPattern::ControlType controlType = m_Pattern.ConvertCharGridToImageAttributes(imageAttributes, m_CharacterGrid);
        if (controlType != CharacterPattern::InvalidControl)
        {
            // Control type valid, update image attributes and return control type
            imageAttributes.SetLastLineY(m_CharacterGrid.GetYOffset() - imageAttributes.GetLastLineControlYOffset());
            return controlType;
        }

        y = qMax(y, m_CharacterGrid.GetYOffset() + m_CharacterGrid.GetUsedHeight() - 1);
    }

    imageAttributes.SetLastLineY(-1);

#ifdef CHARACTERGRID_DEBUG
    m_CharacterGrid.GetImage().save("../Images/debugImage.png");
#endif // CHARACTERGRID_DEBUG

    return CharacterPattern::InvalidControl;
}

bool ImageReader::IsLogScrolledDown(const QImage& image, const ImageAttributes& imageAttributes)
{
#ifdef CHARACTERGRID_DEBUG
    if (m_CharacterGrid.GetImage().size() != image.size())
    {
        m_CharacterGrid.GetImage() = QImage(image.size(), image.format());
    }
#endif // CHARACTERGRID_DEBUG

    const int startX = m_CharacterGrid.GetXOffset();
    const int startY = m_CharacterGrid.GetYOffset() - 1;

    Q_ASSERT(startX >= 0 && startX < image.width() && startY >= 0 && startY < image.height());

    m_CharacterGrid.SetLowerRgbLimit(CHARACTERGRID_SCROLLBAR_LOWER_RGB_LIMIT);
    for (int x = startX, y = startY, endY = y - IMAGEATTRIBUTES_MAX_SCROLLBAR_SEARCH_HEIGHT; y >= endY; --y)
    {
        if (!m_CharacterGrid.IsPartOfCharacter(image.pixel(x, y)))
        {
            continue;
        }

        m_CharacterGrid.FindCharacter(image, x, y, imageAttributes);
        CharacterPattern::ControlType controlType = m_Pattern.ConvertCharGridToScrollBarControlType(m_CharacterGrid, imageAttributes);
        if (controlType == CharacterPattern::ScrollBarControl)
        {
            // Found scrollbar arrow, scrollbar exists, return whether it's scrolled down or not
            return m_CharacterGrid.IsPartOfCharacter(image.pixel(x, m_CharacterGrid.GetYOffset() - IMAGEATTRIBUTES_MIN_SCROLLBAR_Y_OFFSET));
        }

        y = qMin(y, m_CharacterGrid.GetYOffset() - m_CharacterGrid.GetUsedHeight() + 1);
    }

#ifdef CHARACTERGRID_DEBUG
    m_CharacterGrid.GetImage().save("../Images/debugImage.png");
#endif // CHARACTERGRID_DEBUG

    return true;
}
