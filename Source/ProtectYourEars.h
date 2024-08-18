/*
  ==============================================================================

    ProtectYourEars.h
    Created: 18 Aug 2024 2:20:40pm
    Author:  marat

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

inline void protectYourEars(juce::AudioBuffer<float>& buffer)
{
    bool firstWarning = true;
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float x = channelData[sample];
            bool silence = false;
            if (std::isnan(x))
            {
                DBG("!!! Warning: nan detected in audio buffer, silencing !!!");
                silence = true;
            }
            else if (std::isinf(x)) {
                DBG("!!! WARNING: inf detected in audio buffer, silencing !!!");
                silence = true;
            }
            else if (x < -2.0f || x > 2.0f) {
                DBG("!!! WARNING: sample out of range, silencing !!!");
                silence = true;
            }
            else if (x < -1.0f || x > 1.0f)
            {
                if (firstWarning) {
                    DBG("!!! WARNING: sample out of range: " << x << " !!!");
                    firstWarning = false;
                }
            }
            if (silence) {
                buffer.clear();
                return;
            }
        }
    }
}