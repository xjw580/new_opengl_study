#pragma once

#include <vector>
#include "common.h"

void loadElectrode(
    std::vector<BrainCommon::Electrode> &electrodes,
    int xLength, int yLength, int zLength
);

int test(std::vector<float> &values, int xLength, int yLength, int zLength);
