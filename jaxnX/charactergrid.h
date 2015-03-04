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

#include <QVector>
#include <QPoint>
#include <QRgb>

class QImage;

namespace GW2
{
    class CharacterGrid;
    typedef bool (CharacterGrid::*IsParfOfFuncPtr)(QRgb);

    class ImageAttributes;

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
}


#endif // CHARACTERGRID_H
