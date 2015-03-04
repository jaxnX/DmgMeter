#ifndef IMAGEREADER_H
#define IMAGEREADER_H

#include "charactergrid.h"
#include "characterpattern.h"

namespace GW2
{
    class ImageReader
    {
    private:
        CharacterGrid m_CharacterGrid;
        CharacterPattern m_Pattern;

    public:

        ImageReader();
        ~ImageReader();

        QString Read(const QImage& image, const ImageAttributes& imageAttributes);
        QString ReadLineFromBottom(const QImage& image, const ImageAttributes& imageAttributes, int lineIndex = 0);
        CharacterPattern::ControlType UpdateImageAttributes(ImageAttributes& imageAttributes, const QImage& image);
        bool IsLogScrolledDown(const QImage& image, const ImageAttributes& imageAttributes);
        const CharacterGrid& GetCharacterGrid() const;
    };
}

#endif // IMAGEREADER_H
