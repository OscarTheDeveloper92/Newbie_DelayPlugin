/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Parameters.h"
#include "RotaryKnob.h"

//==============================================================================
/**
*/
class Delay1_0AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Delay1_0AudioProcessorEditor (Delay1_0AudioProcessor&);
    ~Delay1_0AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Delay1_0AudioProcessor& audioProcessor;

    RotaryKnob gainKnob { "Gain" };
    juce::AudioProcessorValueTreeState::SliderAttachment attachment{
        audioProcessor.apvts, gainParamID.getParamID(), gainKnob.slider
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Delay1_0AudioProcessorEditor)
};
