#pragma once

#include <string>

namespace BrainCommon {
    struct Electrode {
        std::string name;
        int x;
        int y;
        int z;
        double value;
    };
}
