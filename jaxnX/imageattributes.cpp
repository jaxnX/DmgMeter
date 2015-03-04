#include "imageattributes.h"

using namespace GW2;

const int ImageAttributes::s_LastLineControlYOffsets[] = {0,21,0, 0,20,0, 0,17,0, 0,21,0};
const int ImageAttributes::s_LineDistances[] = {0,0,0, 0,16,0, 0,0,0, 0,0,20};
const int ImageAttributes::s_CharacterWidths[] = {0,0,0, 0,7,0, 0,0,0, 0,0,0};
const int ImageAttributes::s_CharacterHeights[] = {0,0,0, 0,10,0, 0,0,0, 0,0,0};


ImageAttributes::ImageAttributes() :
    m_InterfaceSize(NormalInterfaceSize),
    m_ChatTextSize(MediumTextSize),
    m_InterfaceChatTextSizeIndex(m_InterfaceSize * ChatTextSizeCount + m_ChatTextSize),
    m_LastLineY(-1)
{

}

ImageAttributes::~ImageAttributes()
{

}

void ImageAttributes::SetInterfaceSize(InterfaceSize interfaceSize)
{
    m_InterfaceSize = interfaceSize;
    m_InterfaceChatTextSizeIndex = m_InterfaceSize * ChatTextSizeCount + m_ChatTextSize;
}

void ImageAttributes::SetChatTextSize(ImageAttributes::ChatTextSize chatTextSize)
{
    m_ChatTextSize = chatTextSize;
    m_InterfaceChatTextSizeIndex = m_InterfaceSize * ChatTextSizeCount + m_ChatTextSize;
}

void ImageAttributes::SetLastLineY(int lastLineY)
{
    m_LastLineY = lastLineY;
}

ImageAttributes::InterfaceSize ImageAttributes::GetInterfaceSize() const
{
    return m_InterfaceSize;
}

ImageAttributes::ChatTextSize ImageAttributes::GetChatTextSize() const
{
    return m_ChatTextSize;
}

int ImageAttributes::GetLastLineControlYOffset() const
{
    return s_LastLineControlYOffsets[m_InterfaceChatTextSizeIndex];
}

int ImageAttributes::GetLineDistance() const
{
    return s_LineDistances[m_InterfaceChatTextSizeIndex];
}

int ImageAttributes::GetCharacterWidth() const
{
    return s_CharacterWidths[m_InterfaceChatTextSizeIndex];
}

int ImageAttributes::GetCharacterHeight() const
{
    return s_CharacterHeights[m_InterfaceChatTextSizeIndex];
}

int ImageAttributes::GetLineYOffset() const
{
    return GetCharacterHeight() / 2 + 1;
}

int ImageAttributes::GetLastLineY() const
{
    return m_LastLineY;
}
