#ifndef CHARACTERGRID_H
#define CHARACTERGRID_H

//#define CHARACTERGRID_DEBUG
//#define CHARACTERGRID_DEBUG_SAVE_SINGLE
//#define CHARACTERGRID_CREATE_PATTERN

#define CHARACTERGRID_MAX_DEVIATION 100 // Has to be smaller than CHARACTERGRID_LOWER_RGB_LIMIT
#define CHARACTERGRID_LOWER_RGB_LIMIT 110
#define CHARACTERGRID_SCROLLBAR_LOWER_RGB_LIMIT 70
#define CHARACTERGRID_MAX_AT_ONCE 7
#define CHARACTERGRID_Y_OFFSET 20
#define CHARACTERGRID_CALL_IS_PART_OF_FUNC(charGridPtr, isPartOfFuncPtr) ((charGridPtr)->*(isPartOfFuncPtr))

#ifdef CHARACTERGRID_CREATE_PATTERN
#include <QFile>
#include <QTextStream>
#endif // CHARACTERGRID_CREATE_PATTERN

#include <QVector>
#include <QImage>
#include "imageattributes.h"

namespace GW2
{
    class CharacterGrid;
    typedef bool (CharacterGrid::*IsParfOfFuncPtr)(QRgb);

    class CharacterGrid
    {
    private:
        enum State
        {
            Open = 0x0,
            Closed = 0x1,
            Valid = 0x2
        };

        QVector<unsigned char> m_Data;
        QVector<QPoint> m_OpenList;
        QRgb m_Rgb;
        int m_Width;
        int m_Height;
        int m_RedSum;
        int m_GreenSum;
        int m_BlueSum;
        int m_RgbCount;
        int m_UsedWidth;
        int m_UsedHeight;
        int m_XOffset;
        int m_YOffset;
        int m_LowerRgbLimit;
        IsParfOfFuncPtr m_FuncPtr;

        void AddPos(const QImage& image, int x, int y, int index);
        void ResortData();
        void FindSymbol(const QImage& image, int startX, int startY, const ImageAttributes& imageAttributes);
        bool IsValidPartOfSymbol(const QImage& image, int x, int y);

        friend class CharacterPattern;

#ifdef CHARACTERGRID_DEBUG
        QImage m_Image;
#endif // CHARACTERGRID_DEBUG

    public:
        CharacterGrid();
        ~CharacterGrid();

        void SetLowerRgbLimit(int lowerRgbLimit);
        void FindCharacter(const QImage& image, int startX, int startY, const ImageAttributes& imageAttributes);
        void FindNumber(const QImage& image, int startX, int startY, const ImageAttributes& imageAttributes);
        int GetUsedWidth() const;
        int GetUsedHeight() const;
        int GetXOffset() const;
        int GetYOffset() const;
        bool IsPartOfCharacter(QRgb rgb);
        bool IsPartOfNumber(QRgb rgb);
        static bool IsDeviationTooLarge(const QImage& image, QRgb rgb, int x, int y);

#ifdef CHARACTERGRID_DEBUG
        QImage& GetImage() { return m_Image; }
#endif // CHARACTERGRID_DEBUG
    };

    inline void CharacterGrid::AddPos(const QImage& image, int x, int y, int index)
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

    inline void CharacterGrid::ResortData()
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

    inline void CharacterGrid::FindSymbol(const QImage& image, int startX, int startY, const ImageAttributes& imageAttributes)
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

    inline bool CharacterGrid::IsValidPartOfSymbol(const QImage& image, int x, int y)
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

    inline CharacterGrid::CharacterGrid() :
        m_Width(0),
        m_Height(0),
        m_LowerRgbLimit(CHARACTERGRID_LOWER_RGB_LIMIT),
        m_FuncPtr(NULL)
    {

    }

    inline CharacterGrid::~CharacterGrid()
    {

    }

    inline void CharacterGrid::SetLowerRgbLimit(int lowerRgbLimit)
    {
        m_LowerRgbLimit = lowerRgbLimit;
    }

    inline void CharacterGrid::FindCharacter(const QImage& image, int startX, int startY, const ImageAttributes& imageAttributes)
    {
        m_FuncPtr = &CharacterGrid::IsPartOfCharacter;
        FindSymbol(image, startX, startY, imageAttributes);
    }

    inline void CharacterGrid::FindNumber(const QImage& image, int startX, int startY, const ImageAttributes& imageAttributes)
    {
        m_FuncPtr = &CharacterGrid::IsPartOfNumber;
        FindSymbol(image, startX, startY, imageAttributes);
    }

    inline int CharacterGrid::GetUsedWidth() const
    {
        return m_UsedWidth;
    }

    inline int CharacterGrid::GetUsedHeight() const
    {
        return m_UsedHeight;
    }

    inline int CharacterGrid::GetXOffset() const
    {
        return m_XOffset;
    }

    inline int CharacterGrid::GetYOffset() const
    {
        return m_YOffset;
    }

    inline bool CharacterGrid::IsPartOfCharacter(QRgb rgb)
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

    inline bool CharacterGrid::IsPartOfNumber(QRgb rgb)
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

    inline bool CharacterGrid::IsDeviationTooLarge(const QImage& image, QRgb rgb, int x, int y)
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
}


#endif // CHARACTERGRID_H
