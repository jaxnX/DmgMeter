#include "screenrecorder.h"

using namespace GW2;

const QString ScreenRecorder::s_Infos[static_cast<int>(CharacterPattern::ControlTypeCount)] = {
        "Waiting", // Invalid
        "", // Visible
        "Please unhide chat panel", // Hidden
        "Please scroll down log" // Scrollbar
        };
