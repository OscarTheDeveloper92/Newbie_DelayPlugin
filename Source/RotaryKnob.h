/*
  ==============================================================================

    RotaryKnob.h
    Created: 3 Aug 2024 2:41:39am
    Author:  marat

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class RotaryKnob  : public juce::Component
{
public:
    RotaryKnob(const juce::String& text);
    ~RotaryKnob() override;

    void resized() override;

    juce::Slider slider;
    juce::Label label;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RotaryKnob)
};
