/*
  ==============================================================================

    LookAndFeel.h
    Created: 9 Aug 2024 1:24:59am
    Author:  marat

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace Colors
{
    const juce::Colour background { 240, 240, 240 };
    const juce::Colour header { 40,40,40 };

    namespace Knob
    {
        const juce::Colour trackBackground { 232,35,35 };
        const juce::Colour trackActive { 177,101,135 };
        const juce::Colour outline { 255,250,245 };
        const juce::Colour gradientTop { 250,245,240 };
        const juce::Colour gradientBottom { 240,235,230 };
        const juce::Colour dial { 100,100,100 };
        const juce::Colour dropShadow { 195,190,185 };
        const juce::Colour label { 80,80,80 };
        const juce::Colour textBoxBackground { 80,80,80 };
        const juce::Colour value { 240,240,240 };
        const juce::Colour caret { 255, 255, 255 };
    }
}

//The RotaryKnobLookAndFeel class is based off the juce::LookAndFeel_V4 class.
class RotaryKnobLookAndFeel : public juce::LookAndFeel_V4
{
    public:
        //Constructor takes no arguments
        RotaryKnobLookAndFeel();

        //This function allows us to use RotaryKnobLookAndFeel globally in the code.
        static RotaryKnobLookAndFeel* get()
        {
            static RotaryKnobLookAndFeel instance;
            return &instance;
        }

        //This function is what will draw the knob. It's called from the juce::Slider paint function.
        void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
            float sliderPos, float rotaryStartAngle,
            float rotaryEndAngle, juce::Slider& slider) override;
    private:
        juce::DropShadow dropShadow { Colors::Knob::dropShadow, 6, {0,3} };

        //Makes sure the object can't be copied and create memory leaks.
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotaryKnobLookAndFeel)
};
