#ifndef CHARACTERPATTERN_H
#define CHARACTERPATTERN_H

#define CHARACTERPATTERN_POSITIVE_WEIGHT 1
#define CHARACTERPATTERN_NEGATIVE_WEIGHT 3
#define CHARACTERPATTERN_X_OFFSET 1
#define CHARACTERPATTERN_COUNT 10
#define CHARACTERPATTERN_MAX_WIDTH 7
#define CHARACTERPATTERN_MAX_HEIGHT 10
#define CHARACTERPATTERN_CONTROL_COUNT 3
#define CHARACTERPATTERN_CONTROL_INDEX_OFFSET 1
#define CHARACTERPATTERN_CONTROL_MAX_WIDTH 20
#define CHARACTERPATTERN_CONTROL_MAX_HEIGHT 18
#define CHARACTERPATTERN_SCROLLBAR_CONTROL_COUNT 1

#include "imageattributes.h"
#include "charactergrid.h"

namespace GW2
{
    class CharacterPattern
    {
    public:
        enum ControlType
        {
            InvalidControl,
            VisibleControl,
            HiddenControl,
            ScrollBarControl,
            ControlTypeCount
        };

        CharacterPattern();
        ~CharacterPattern();

        QString ConvertCharGridToCharacter(const CharacterGrid& charGrid, const ImageAttributes& imageAttributes, int offsetAdd = 0);
        ControlType ConvertCharGridToImageAttributes(ImageAttributes& imageAttributes, const CharacterGrid& charGrid);
        ControlType ConvertCharGridToScrollBarControlType(const CharacterGrid& charGrid, const ImageAttributes& imageAttributes);

    private:
        void FindPatternMatches(const unsigned char* patternGrids, const CharacterGrid& charGrid, unsigned int patternCount, int xOffset = 0);
        void FindBestMatchingPattern(unsigned int patternCount, int xOffset = -CHARACTERPATTERN_X_OFFSET);
        int FindBestOverallMatchingPattern(unsigned int matchCount, int* bestI = NULL);
        void ResetBestMatchingPatterns();

        int m_PatternMatches[CHARACTERPATTERN_COUNT];
        int m_BestMatchingIndices[ImageAttributes::InterfaceSizeCount];
        int m_BestMatchingPatternCounts[ImageAttributes::InterfaceSizeCount];
        int m_BestMatchingPatterns[ImageAttributes::InterfaceSizeCount];
        int m_CurrentWidth;
        int m_CurrentHeight;
        int m_CurrentPositiveWeight;
        int m_CurrentNegativeWeight;

        static const QString s_Pattern;
        static const unsigned char s_PatternGrids[ImageAttributes::InterfaceSizeCount][ImageAttributes::ChatTextSizeCount][CHARACTERPATTERN_COUNT][CHARACTERPATTERN_MAX_HEIGHT][CHARACTERPATTERN_MAX_WIDTH];
        static const unsigned char s_ControlPatternGrids[ImageAttributes::InterfaceSizeCount][CHARACTERPATTERN_CONTROL_COUNT][CHARACTERPATTERN_CONTROL_MAX_HEIGHT][CHARACTERPATTERN_CONTROL_MAX_WIDTH];
    };
}

#endif // CHARACTERPATTERN_H
