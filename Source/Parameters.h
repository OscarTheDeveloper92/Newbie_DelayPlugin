/*
  ==============================================================================

    Paramters.h
    Created: 22 Jul 2024 10:34:10pm
    Author:  marat

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

const juce::ParameterID gainParamID { "gain",1 };
const juce::ParameterID delayTimeParamID { "delayTime",1 };
const juce::ParameterID mixParamID{ "mix", 1 };

class Parameters
{
public:
    Parameters(juce::AudioProcessorValueTreeState& apvts);

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void update() noexcept;

    float gain = 0.0f;
    float delayTime = 0.0f;
    float mix = 1.0f; //This variable holds between 0% (0.0f) and 100% (1.0f).

    void prepareToPlay(double sampleRate) noexcept;
    void reset() noexcept;
    void smoothen() noexcept;
    static constexpr float minDelayTime = 5.0f;
    static constexpr float maxDelayTime = 5000.0f;

private:
    juce::AudioParameterFloat* gainParam; 
    juce::LinearSmoothedValue<float> gainSmoother;
    juce::AudioParameterFloat* delayTimeParam;
    juce::AudioParameterFloat* mixParam;
    juce::LinearSmoothedValue<float> mixSmoother;

    float targetDelayTime = 0.0f;
    float coeff = 0.0f; // one-pole smoothing

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameters);
};
