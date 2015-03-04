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
    private:
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

        void FindPatternMatches(const unsigned char* patternGrids, const CharacterGrid& charGrid, unsigned int patternCount, int xOffset = 0);
        void FindBestMatchingPattern(unsigned int patternCount, int xOffset = -CHARACTERPATTERN_X_OFFSET);
        int FindBestOverallMatchingPattern(unsigned int matchCount, int* bestI = NULL);
        void ResetBestMatchingPatterns();

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
    };

    inline void CharacterPattern::FindPatternMatches(const unsigned char* patternGrids, const CharacterGrid& charGrid, unsigned int patternCount, int xOffset)
    {
        Q_ASSERT(patternGrids);
        Q_ASSERT(patternCount <= CHARACTERPATTERN_COUNT);

        // Reset pattern matches
        for (unsigned int i = 0; i < patternCount; ++i)
        {
            m_PatternMatches[i] = 0;
        }

        const QVector<unsigned char>& data = charGrid.m_Data;
        const int charGridWidth = charGrid.m_Width;
        const int width = m_CurrentWidth;
        const int height = m_CurrentHeight;
        const int size = width * height;
        for (unsigned int i = 0; i < patternCount; ++i)
        {
            const unsigned int iIndex = i * size;
            for (int y = 0; y < height; ++y)
            {
                const unsigned int iyIndex = iIndex + y * width;
                for (int x = 0; x < width; ++x)
                {
                    const int newX = x + xOffset;
                    const unsigned int index = iyIndex + x;
                    if (newX < 0)
                    {
                        if (patternGrids[index] == 1)
                        {
                            // Data does not match the pattern
                            m_PatternMatches[i] -= m_CurrentNegativeWeight;
                        }

                        continue;
                    }

                    if (!(data[y * charGridWidth + newX] & CharacterGrid::Valid))
                    {
                        if (patternGrids[index] == 1)
                        {
                            // Data does not match the pattern
                            m_PatternMatches[i] -= m_CurrentNegativeWeight;
                        }

                        continue;
                    }

                    if (patternGrids[index] == 1)
                    {
                        // Data matches the pattern
                        m_PatternMatches[i] += m_CurrentPositiveWeight;
                    }
                    else
                    {
                        // Data does not match the pattern
                        m_PatternMatches[i] -= m_CurrentNegativeWeight;
                    }
                }
            }
        }
    }

    inline void CharacterPattern::FindBestMatchingPattern(unsigned int patternCount, int xOffset)
    {
        const unsigned int index = xOffset + CHARACTERPATTERN_X_OFFSET;

        Q_ASSERT(patternCount <= CHARACTERPATTERN_COUNT);
        Q_ASSERT(index < ImageAttributes::InterfaceSizeCount);

        int bestPattern = INT_MIN;
        for (unsigned int i = 0; i < patternCount; ++i)
        {
            if (m_PatternMatches[i] > bestPattern)
            {
                // Found new best matching pattern
                bestPattern = m_PatternMatches[i];
                m_BestMatchingIndices[index] = i;
                m_BestMatchingPatterns[index] = bestPattern;
                m_BestMatchingPatternCounts[index] = 0;
            }
            else if (m_PatternMatches[i] == bestPattern)
            {
                // Multiple pattern match best
                ++m_BestMatchingPatternCounts[index];
            }
        }
    }

    inline int CharacterPattern::FindBestOverallMatchingPattern(unsigned int matchCount, int* bestI)
    {
        Q_ASSERT(matchCount <= ImageAttributes::InterfaceSizeCount);

        int bestIndex = -1;
        int bestMatchingPattern = 0;
        for (unsigned int i = 0; i < matchCount; ++i)
        {
            if (m_BestMatchingPatterns[i] > bestMatchingPattern)
            {
                bestIndex = m_BestMatchingIndices[i];
                bestMatchingPattern = m_BestMatchingPatterns[i];
                if (bestI)
                {
                    *bestI = i;
                }
            }
        }

        return bestIndex;
    }

    inline void CharacterPattern::ResetBestMatchingPatterns()
    {
        const int matchCount = ImageAttributes::InterfaceSizeCount;
        for (int i = 0; i < matchCount; ++i)
        {
            m_BestMatchingIndices[i] = -1;
            m_BestMatchingPatternCounts[i] = 0;
            m_BestMatchingPatterns[i] = 0;
        }
    }

    inline CharacterPattern::CharacterPattern() :
        m_CurrentWidth(-1),
        m_CurrentHeight(-1)
    {
    }

    inline CharacterPattern::~CharacterPattern()
    {

    }

    inline QString CharacterPattern::ConvertCharGridToCharacter(const CharacterGrid& charGrid, const ImageAttributes& imageAttributes, int offsetAdd)
    {
        // Flatten array for function
        const unsigned char* patternGrids = &s_PatternGrids[imageAttributes.GetInterfaceSize()][imageAttributes.GetChatTextSize()][0][0][0];

        // Find best matching patterns with offset
        ResetBestMatchingPatterns();
        m_CurrentWidth = imageAttributes.GetCharacterWidth();
        m_CurrentHeight = imageAttributes.GetCharacterHeight();
        m_CurrentPositiveWeight = CHARACTERPATTERN_POSITIVE_WEIGHT;
        m_CurrentNegativeWeight = CHARACTERPATTERN_NEGATIVE_WEIGHT;
        for (int xOffset = -CHARACTERPATTERN_X_OFFSET; xOffset <= CHARACTERPATTERN_X_OFFSET; ++xOffset)
        {
            FindPatternMatches(patternGrids, charGrid, CHARACTERPATTERN_COUNT, xOffset + offsetAdd);
            FindBestMatchingPattern(CHARACTERPATTERN_COUNT, xOffset);
        }

        const int bestIndex = FindBestOverallMatchingPattern(CHARACTERPATTERN_X_OFFSET * 2 + 1);
        if (bestIndex >= 0)
        {
            // Found best matching pattern, return resulting character
            return s_Pattern[bestIndex];
        }

        return "";
    }

    inline CharacterPattern::ControlType CharacterPattern::ConvertCharGridToImageAttributes(ImageAttributes& imageAttributes, const CharacterGrid& charGrid)
    {
        ResetBestMatchingPatterns();
        m_CurrentWidth = CHARACTERPATTERN_CONTROL_MAX_WIDTH;
        m_CurrentHeight = CHARACTERPATTERN_CONTROL_MAX_HEIGHT;
        m_CurrentPositiveWeight = CHARACTERPATTERN_POSITIVE_WEIGHT;
        m_CurrentNegativeWeight = CHARACTERPATTERN_NEGATIVE_WEIGHT * 10;
        const int interfaceSizeCount = ImageAttributes::InterfaceSizeCount;
        for (int i = 0; i < interfaceSizeCount; ++i)
        {
            // Flatten array for function
            const unsigned char* patternGrids = &s_ControlPatternGrids[i][0][0][0];

            FindPatternMatches(patternGrids, charGrid, CHARACTERPATTERN_CONTROL_COUNT);
            FindBestMatchingPattern(CHARACTERPATTERN_CONTROL_COUNT, -CHARACTERPATTERN_X_OFFSET + i);
        }

        int interfaceIndex;
        const int bestIndex = FindBestOverallMatchingPattern(ImageAttributes::InterfaceSizeCount, &interfaceIndex);
        if (bestIndex >= 0)
        {
            // Found interface size, set it as new interface size;
            imageAttributes.SetInterfaceSize(static_cast<ImageAttributes::InterfaceSize>(interfaceIndex));
        }

        return static_cast<ControlType>(bestIndex + CHARACTERPATTERN_CONTROL_INDEX_OFFSET);
    }

    inline CharacterPattern::ControlType CharacterPattern::ConvertCharGridToScrollBarControlType(const CharacterGrid& charGrid, const ImageAttributes& imageAttributes)
    {
        // Flatten array for function
        const unsigned char* patternGrids = &s_ControlPatternGrids[imageAttributes.GetInterfaceSize()][ScrollBarControl - CHARACTERPATTERN_CONTROL_INDEX_OFFSET][0][0];

        ResetBestMatchingPatterns();
        m_CurrentWidth = CHARACTERPATTERN_CONTROL_MAX_WIDTH;
        m_CurrentHeight = CHARACTERPATTERN_CONTROL_MAX_HEIGHT;
        m_CurrentPositiveWeight = CHARACTERPATTERN_POSITIVE_WEIGHT;
        m_CurrentNegativeWeight = CHARACTERPATTERN_NEGATIVE_WEIGHT;
        for (int xOffset = -CHARACTERPATTERN_X_OFFSET; xOffset <= CHARACTERPATTERN_X_OFFSET; ++xOffset)
        {
            FindPatternMatches(patternGrids, charGrid, CHARACTERPATTERN_SCROLLBAR_CONTROL_COUNT, xOffset);
            FindBestMatchingPattern(CHARACTERPATTERN_COUNT, xOffset);
        }

        const int bestIndex = FindBestOverallMatchingPattern(CHARACTERPATTERN_X_OFFSET * 2 + 1);
        if (bestIndex >= 0)
        {
            // Found pattern, return scrollbar control
            return ScrollBarControl;
        }

        return InvalidControl;
    }
}

#endif // CHARACTERPATTERN_H
