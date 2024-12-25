/*
  ==============================================================================

    Tempo.h
    Created: 25 Dec 2024 12:18:43am
    Author:  marat

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class Tempo
{
public:
    void reset() noexcept;

    void update(const juce::AudioPlayHead* playhead) noexcept;

    double getMilisecondsForNoteLength(int index) const noexcept;

    double getTempo() const noexcept
    {
        return bpm;
    }

private:
    double bpm = 120.0;
};
