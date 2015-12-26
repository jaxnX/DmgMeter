#include "charactergrid.h"
#include <QImage>
#include "imageattributes.h"

#ifdef CHARACTERGRID_CREATE_PATTERN
#include <QFile>
#include <QTextStream>
#endif // CHARACTERGRID_CREATE_PATTERN

using namespace GW2;

CharacterGrid::CharacterGrid() :
    m_Width(0),
    m_Height(0),
    m_LowerRgbLimit(CHARACTERGRID_LOWER_RGB_LIMIT),
    m_FuncPtr(NULL)
{

}

CharacterGrid::~CharacterGrid()
{

}

void CharacterGrid::SetLowerRgbLimit(int lowerRgbLimit)
{
    m_LowerRgbLimit = lowerRgbLimit;
}

void CharacterGrid::FindCharacter(const QImage& image, int startX, int startY, const ImageAttributes& imageAttributes)
{
    m_FuncPtr = &CharacterGrid::IsPartOfCharacter;
    FindSymbol(image, startX, startY, imageAttributes);
}

void CharacterGrid::FindNumber(const QImage& image, int startX, int startY, const ImageAttributes& imageAttributes)
{
    m_FuncPtr = &CharacterGrid::IsPartOfNumber;
    FindSymbol(image, startX, startY, imageAttributes);
}

bool CharacterGrid::IsPartOfCharacter(QRgb rgb)
{
    int red = qRed(rgb);
    int green = qGreen(rgb);
    int blue = qBlue(rgb);
    if (red < m_LowerRgbLimit && green < m_LowerRgbLimit && blue < m_LowerRgbLimit)
    {
        return false;
    }

    return true;
}

bool CharacterGrid::IsPartOfNumber(QRgb rgb)
{
    int red = qRed(rgb);
    int green = qGreen(rgb);
    int blue = qBlue(rgb);
    if (red < m_LowerRgbLimit && green < m_LowerRgbLimit && blue < m_LowerRgbLimit)
    {
        return false;
    }

    if (red > m_LowerRgbLimit && green > m_LowerRgbLimit && blue > m_LowerRgbLimit)
    {
        return false;
    }

    return true;
}

bool CharacterGrid::IsDeviationTooLarge(const QImage& image, QRgb rgb, int x, int y)
{
    QRgb newRgb = image.pixel(x, y);
    int red = qRed(newRgb);
    int green = qGreen(newRgb);
    int blue = qBlue(newRgb);
    int redDeviation = qAbs(red - qRed(rgb));
    int greenDeviation = qAbs(green - qGreen(rgb));
    int blueDeviation = qAbs(blue - qBlue(rgb));
    if (redDeviation > CHARACTERGRID_MAX_DEVIATION)
    {
        return true;
    }

    if (greenDeviation > CHARACTERGRID_MAX_DEVIATION)
    {
        return true;
    }

    if (blueDeviation > CHARACTERGRID_MAX_DEVIATION)
    {
        return true;
    }

    return false;
}

void CharacterGrid::AddPos(const QImage& image, int x, int y, int index)
{
    if (!(m_Data[index] & Closed) && IsValidPartOfSymbol(image, x, y))
    {
        m_Data[index] |= (Closed | Valid);
        m_OpenList.append(QPoint(x, y));
        if (x < m_XOffset)
        {
            m_XOffset = x;
        }

        if (y < m_YOffset)
        {
            m_YOffset = y;
        }
    }
}

void CharacterGrid::ResortData()
{
    // Find min x
    int minX = INT_MAX;
    int minY = INT_MAX;
    for (int y = 0; y < m_Height; ++y)
    {
        for (int x = 0; x < m_Width; ++x)
        {
            const int i = y * m_Width + x;
            if (m_Data[i] & Valid)
            {
                if (x < minX)
                {
                    minX = x;
                }

                if (y < minY)
                {
                    minY = y;
                }
            }
        }
    }

    // Resort data
    const int maxX = m_Width - minX;
    const int maxY = m_Height - minY;
    for (int y = 0, y2 = minY; y < maxY; ++y, ++y2)
    {
        for (int x = 0, x2 = minX; x < maxX; ++x, ++x2)
        {
            const int newIndex = y * m_Width + x;
            const int oldIndex = y2 * m_Width + x2;
            m_Data[newIndex] = m_Data[oldIndex];
            m_Data[oldIndex] = Open;
        }
    }

    // Find out used width and height
    m_UsedWidth = -1;
    m_UsedHeight = -1;
    for (int y = 0; y < m_Height; ++y)
    {
        for (int x = 0; x < m_Width; ++x)
        {
            if (m_Data[y * m_Width + x] & Valid)
            {
                if (x > m_UsedWidth)
                {
                    m_UsedWidth = x;
                }

                if (y > m_UsedHeight)
                {
                    m_UsedHeight = y;
                }
            }
        }
    }

    ++m_UsedWidth;
    ++m_UsedHeight;
}

void CharacterGrid::FindSymbol(const QImage& image, int startX, int startY, const ImageAttributes& imageAttributes)
{
    // Reset old data
    const int width = imageAttributes.GetCharacterWidth() * CHARACTERGRID_MAX_AT_ONCE;
    const int height = imageAttributes.GetCharacterHeight() + CHARACTERGRID_Y_OFFSET * 2;
    if (m_Width != width || m_Height != height)
    {
        m_Data = QVector<unsigned char>(width * height);
        m_Width = width;
        m_Height = height;
    }
    else
    {
        for (int i = 0; i < m_Data.size(); ++i)
        {
            m_Data[i] = Open;
        }
    }

    // Expand pixel to neighbours using dijkstra
    const int dataCount = m_Data.size();
    const int xOffset = startX - imageAttributes.GetCharacterWidth() * (CHARACTERGRID_MAX_AT_ONCE - 1);
    const int yOffset = startY - imageAttributes.GetLineYOffset() - CHARACTERGRID_Y_OFFSET;
    int x, y;
    int iterations = 0;
    int dataX = startX - xOffset;
    int dataY = startY - yOffset;
    m_XOffset = startX;
    m_YOffset = startY;
    m_OpenList.erase(m_OpenList.begin(), m_OpenList.end());
    m_Data[dataY * m_Width + dataX] |= (Closed | Valid);
    m_OpenList.append(QPoint(startX, startY));
    m_Rgb = image.pixel(startX, startY);
    m_RedSum = qRed(m_Rgb);
    m_GreenSum = qGreen(m_Rgb);
    m_BlueSum = qBlue(m_Rgb);
    m_RgbCount = 1;
    while (!m_OpenList.isEmpty() && iterations < dataCount)
    {
        QPoint pos = m_OpenList.takeLast();
        x = pos.x();
        y = pos.y();
        dataX = x - xOffset;
        dataY = y - yOffset;
        if (dataX <= 0 || dataX >= m_Width - 1 || dataY <= 0 || dataY >= m_Height - 1)
        {
            continue;
        }

        if (x < 1 || x >= image.width() - 1 || y < 1 || y >= image.height() - 1)
        {
            continue;
        }

        m_Rgb = qRgb(m_RedSum / m_RgbCount, m_GreenSum / m_RgbCount, m_BlueSum / m_RgbCount);
        ++iterations;
        AddPos(image, x, y + 1, (dataY + 1) * m_Width + dataX); // Bottom
        AddPos(image, x - 1, y + 1, (dataY + 1) * m_Width + (dataX - 1)); // Bottom Left
        AddPos(image, x - 1, y, dataY * m_Width + (dataX - 1)); // Left
        AddPos(image, x - 1, y - 1, (dataY - 1) * m_Width + (dataX - 1)); // Top Left
        AddPos(image, x, y - 1, (dataY - 1) * m_Width + dataX); // Top
        AddPos(image, x + 1, y - 1, (dataY - 1) * m_Width + (dataX + 1)); // Top Right
        AddPos(image, x + 1, y, dataY * m_Width + (dataX + 1)); // Right
        AddPos(image, x + 1, y + 1, (dataY + 1) * m_Width + (dataX + 1)); // Bottom Right
    }

#ifdef CHARACTERGRID_DEBUG
    for (y = 0; y < m_Height; ++y)
    {
        for (x = 0; x < m_Width; ++x)
        {
            if (m_Data[y * m_Width + x] & CharacterGrid::Valid)
            {
                const int px = x + xOffset;
                const int py = y + yOffset;
                m_Image.setPixel(px, py, image.pixel(px, py));
            }
        }
    }
#endif // CHARACTERGRID_DEBUG

    ResortData();

#ifdef CHARACTERGRID_DEBUG_SAVE_SINGLE
    m_Image.save("../Images/debugImage.png");
#endif // CHARACTERGRID_DEBUG_SAVE_SINGLE

#ifdef CHARACTERGRID_CREATE_PATTERN
    QFile file("../Dmg-Meter/characterpattern.cpp");
    Q_ASSERT(file.open(QIODevice::Append | QIODevice::Text));
    QTextStream out(&file);
    const int pWidth = 20;
    const int pHeight = 18;
    out << "\n// Generated pattern: x = " + QString::number(startX) + ", y = " + QString::number(startY) +
           ", width = " + QString::number(pWidth) + ", height = " + QString::number(pHeight) + "\n";
    out << "{\n";
    for (y = 0; y < pHeight; ++y)
    {
        out << "\t{";
        for (x = 0; x < pWidth; ++x)
        {
            out << QString::number((m_Data[y * m_Width + x] & CharacterGrid::Valid) == CharacterGrid::Valid);
            if (x < pWidth - 1)
            {
                out << ",";
            }
        }

        out << "}";
        if (y < pHeight - 1)
        {
            out << ",";
        }

        out << "\n";
    }

    out << "}\n";
#endif // CHARACTERGRID_CREATE_PATTERN
}

bool CharacterGrid::IsValidPartOfSymbol(const QImage& image, int x, int y)
{
    Q_ASSERT(m_FuncPtr);

    QRgb rgb = image.pixel(x, y);
    if (!CharacterGrid::IsDeviationTooLarge(image, m_Rgb, x, y) && CHARACTERGRID_CALL_IS_PART_OF_FUNC(this, m_FuncPtr)(rgb))
    {
        int red = qRed(rgb);
        int green = qGreen(rgb);
        int blue = qBlue(rgb);
        m_RedSum += red;
        m_GreenSum += green;
        m_BlueSum += blue;
        ++m_RgbCount;

        return true;
    }

    return false;
}
