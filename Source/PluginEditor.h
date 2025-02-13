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
#include "LookAndFeel.h"

//==============================================================================
/**
*/
class Delay1_0AudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::AudioProcessorParameter::Listener
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

    void parameterValueChanged(int, float) override;
    void parameterGestureChanged(int, bool) override {}

    Delay1_0AudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Delay1_0AudioProcessorEditor)
    
    RotaryKnob gainKnob { "Gain", audioProcessor.apvts, gainParamID, true };
    RotaryKnob mixKnob { "Mix", audioProcessor.apvts, mixParamID };
    RotaryKnob delayTimeKnob { "Time", audioProcessor.apvts, delayTimeParamID };
    RotaryKnob feedbackKnob { "Feedback", audioProcessor.apvts, feedbackParamID, true };
    RotaryKnob stereoKnob { "Stereo", audioProcessor.apvts, stereoParamID, true };
    RotaryKnob lowCutKnob { "Low Cut", audioProcessor.apvts, lowCutParamID };
    RotaryKnob highCutKnob { "High Cut", audioProcessor.apvts, highCutParamID };
    RotaryKnob delayNoteKnob{ "Note", audioProcessor.apvts, delayNoteParamID };

    juce::TextButton tempoSyncButton;

    juce::AudioProcessorValueTreeState::ButtonAttachment tempoSyncAttachment{
        audioProcessor.apvts, tempoSyncParamID.getParamID(), tempoSyncButton
    };

    juce::GroupComponent delayGroup, feedbackGroup, outputGroup;

    MainLookAndFeel mainLF;

    void updateDelayKnobs(bool tempoSyncActive);
};
