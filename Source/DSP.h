/*
  ==============================================================================

    DSP.h
    Created: 22 Aug 2024 2:13:02am
    Author:  marat

  ==============================================================================
*/

#pragma once
#include <cmath>

inline void panningEqualPower(float panning, float& left, float& right)
{
    float x = 0.7853981633974483f * (panning + 1.0f);
    left = std::cos(x);
    right = std::sin(x);
}
