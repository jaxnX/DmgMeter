#ifndef IMAGEREADER_H
#define IMAGEREADER_H

#include "charactergrid.h"
#include "characterpattern.h"

namespace GW2
{
    class ImageReader
    {
    public:
        ImageReader();
        ~ImageReader();

        QString Read(const QImage& image, const ImageAttributes& imageAttributes, int lineCount);
        QString ReadLineFromBottom(const QImage& image, const ImageAttributes& imageAttributes, int lineCount, int lineIndex = 0);
        CharacterPattern::ControlType UpdateImageAttributes(ImageAttributes& imageAttributes, const QImage& image);
        bool IsLogScrolledDown(const QImage& image, const ImageAttributes& imageAttributes);
        inline const CharacterGrid& GetCharacterGrid() const;

    private:
        CharacterGrid m_CharacterGrid;
        CharacterPattern m_Pattern;
    };

    inline const CharacterGrid& ImageReader::GetCharacterGrid() const
    {
        return m_CharacterGrid;
    }
}

#endif // IMAGEREADER_H
