#ifndef IMAGEATTRIBUTES_H
#define IMAGEATTRIBUTES_H

#define IMAGEATTRIBUTES_LINE_X_START 19
#define IMAGEATTRIBUTES_CONTROL_X_START 22
#define IMAGEATTRIBUTES_CONTROL_MAX_HEIGHT 28
#define IMAGEATTRIBUTES_CONTROL_MAX_DEVIATION 40
#define IMAGEATTRIBUTES_MAX_SCROLLBAR_SEARCH_HEIGHT 40
#define IMAGEATTRIBUTES_MIN_SCROLLBAR_Y_OFFSET 4
#define IMAGEATTRIBUTES_MAX_CONSIDERED_LINE_COUNT 8
#define IMAGEATTRIBUTES_MAX_WIDTH 611
#define IMAGEATTRIBUTES_MAX_HEIGHT 556

#include <QObject>

namespace GW2
{
    class ImageAttributes
    {
    public:
        enum InterfaceSize
        {
            SmallInterfaceSize,
            NormalInterfaceSize,
            LargeInterfaceSize,
            LargerInterfaceSize,
            InterfaceSizeCount
        };

        enum ChatTextSize
        {
            SmallTextSize,
            MediumTextSize,
            LargeTextSize,
            ChatTextSizeCount
        };

    private:
        InterfaceSize m_InterfaceSize;
        ChatTextSize m_ChatTextSize;
        int m_InterfaceChatTextSizeIndex;
        int m_LastLineY;

        static const int s_LastLineControlYOffsets[InterfaceSizeCount * ChatTextSizeCount];
        static const int s_LineDistances[InterfaceSizeCount * ChatTextSizeCount];
        static const int s_CharacterWidths[InterfaceSizeCount * ChatTextSizeCount];
        static const int s_CharacterHeights[InterfaceSizeCount * ChatTextSizeCount];

    public:
        ImageAttributes();
        ~ImageAttributes();

        void SetInterfaceSize(InterfaceSize interfaceSize);
        void SetChatTextSize(ChatTextSize chatTextSize);
        void SetLastLineY(int lastLineY);
        InterfaceSize GetInterfaceSize() const;
        ChatTextSize GetChatTextSize() const;
        int GetLastLineControlYOffset() const;
        int GetLineDistance() const;
        int GetCharacterWidth() const;
        int GetCharacterHeight() const;
        int GetLineYOffset() const;
        int GetLastLineY() const;
    };

    inline ImageAttributes::ImageAttributes() :
        m_InterfaceSize(NormalInterfaceSize),
        m_ChatTextSize(MediumTextSize),
        m_InterfaceChatTextSizeIndex(m_InterfaceSize * ChatTextSizeCount + m_ChatTextSize),
        m_LastLineY(-1)
    {

    }

    inline ImageAttributes::~ImageAttributes()
    {

    }

    inline void ImageAttributes::SetInterfaceSize(InterfaceSize interfaceSize)
    {
        m_InterfaceSize = interfaceSize;
        m_InterfaceChatTextSizeIndex = m_InterfaceSize * ChatTextSizeCount + m_ChatTextSize;
    }

    inline void ImageAttributes::SetChatTextSize(ImageAttributes::ChatTextSize chatTextSize)
    {
        m_ChatTextSize = chatTextSize;
        m_InterfaceChatTextSizeIndex = m_InterfaceSize * ChatTextSizeCount + m_ChatTextSize;
    }

    inline void ImageAttributes::SetLastLineY(int lastLineY)
    {
        m_LastLineY = lastLineY;
    }

    inline ImageAttributes::InterfaceSize ImageAttributes::GetInterfaceSize() const
    {
        return m_InterfaceSize;
    }

    inline ImageAttributes::ChatTextSize ImageAttributes::GetChatTextSize() const
    {
        return m_ChatTextSize;
    }

    inline int ImageAttributes::GetLastLineControlYOffset() const
    {
        return s_LastLineControlYOffsets[m_InterfaceChatTextSizeIndex];
    }

    inline int ImageAttributes::GetLineDistance() const
    {
        return s_LineDistances[m_InterfaceChatTextSizeIndex];
    }

    inline int ImageAttributes::GetCharacterWidth() const
    {
        return s_CharacterWidths[m_InterfaceChatTextSizeIndex];
    }

    inline int ImageAttributes::GetCharacterHeight() const
    {
        return s_CharacterHeights[m_InterfaceChatTextSizeIndex];
    }

    inline int ImageAttributes::GetLineYOffset() const
    {
        return GetCharacterHeight() / 2 + 1;
    }

    inline int ImageAttributes::GetLastLineY() const
    {
        return m_LastLineY;
    }
}

#endif // IMAGEATTRIBUTES_H
