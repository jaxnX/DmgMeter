#include "imageattributes.h"

using namespace GW2;

const int ImageAttributes::s_LastLineControlYOffsets[] = {
    0,21,0,
    0,20,0,
    0,17,0,
    0,21,0
};

const int ImageAttributes::s_LineDistances[] = {
    0,0,0,
    0,16,0,
    0,0,0,
    0,0,20
};

const int ImageAttributes::s_CharacterWidths[] = {
    0,0,0,
    0,7,0,
    0,0,0,
    0,0,0
};

const int ImageAttributes::s_CharacterHeights[] = {
    0,0,0,
    0,10,0,
    0,0,0,
    0,0,0
};


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
