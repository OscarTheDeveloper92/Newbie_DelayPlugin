/*
  ==============================================================================

    Paramters.cpp
    Created: 22 Jul 2024 10:34:10pm
    Author:  marat

  ==============================================================================
*/

#include "Parameters.h"

template<typename T>
static void castParameter(juce::AudioProcessorValueTreeState& apvts,
    const juce::ParameterID& id, T& destination)
{
    destination = dynamic_cast<T>(apvts.getParameter(id.getParamID()));
    jassert(destination); 
}

static juce::String stringFromMilliseconds(float value, int)
{
    if (value < 10.0f) {
        return juce::String(value, 2) + " ms";
    }
    else if (value < 100.0f) {
        return juce::String(value, 1) + " ms";
    }
    else if (value < 1000.0f) {
        return juce::String(int(value)) + " ms";
    }
    else {
        return juce::String(value * 0.001f, 2) + " s";
    }
}

static float millisecondsFromString(const juce::String& text)
{
    float value = text.getFloatValue();

    if (!text.endsWithIgnoreCase("ms")) {
        if (text.endsWithIgnoreCase("s") || value < Parameters::minDelayTime) {
            return value * 1000.0f;
        }
    }
    return value;
}

static juce::String stringFromDecibels(float value, int)
{
    return juce::String(value, 1) + " dB";
}

static juce::String stringFromPercent(float value, int)
{
    return juce::String(int(value)) + " %";
}

Parameters::Parameters(juce::AudioProcessorValueTreeState& apvts) //This is the constructor?
{
    castParameter(apvts, gainParamID, gainParam);
    castParameter(apvts, delayTimeParamID, delayTimeParam);
    castParameter(apvts, mixParamID, mixParam);
}

juce::AudioProcessorValueTreeState::ParameterLayout // defined the createParameterLayout() function
Parameters::createParameterLayout()     // also added a range from -12db to 12db
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        gainParamID, //simply changed the gain ID to a const variable to avoid typo errors
        "Output Gain",
        juce::NormalisableRange<float> {-12.0f, 12.0f},
        0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromDecibels)));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        delayTimeParamID,
        "Delay Time",
        juce::NormalisableRange<float> { minDelayTime, maxDelayTime, 0.001f, 0.25f },
        100.0f,
        juce::AudioParameterFloatAttributes()
                    .withStringFromValueFunction(stringFromMilliseconds)
                    .withValueFromStringFunction(millisecondsFromString)));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        mixParamID,
        "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f), //<- goes from 0.0f to 100.0f with steps of 1.0f
        100.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromPercent)));

    return layout;
}

void Parameters::update() noexcept
{
    gainSmoother.setTargetValue(juce::Decibels::decibelsToGain(gainParam->get()));

    //delayTime = delayTimeParam->get(); replaced by code (lines 76-79) below.
    targetDelayTime = delayTimeParam->get();
    if (delayTime == 0.0f) {
        delayTime = targetDelayTime;
    }
    mixSmoother.setTargetValue(mixParam->get() * 0.01f);
}

void Parameters::prepareToPlay(double sampleRate) noexcept
{
    double duration = 0.02;
    gainSmoother.reset(sampleRate, duration);
    coeff = 1.0f - std::exp(-1.0f / (0.2f * float(sampleRate))); //filter coefficient calculation depending on the sample rate.
                                                                 // 0.2f represents 200 milliseconds. Mimics the charge time of an actual analog capacitor
    mixSmoother.reset(sampleRate, duration);                                                             
}

void Parameters::reset() noexcept
{
    gain = 0.0f;
    delayTime = 0.0f;

    gainSmoother.setCurrentAndTargetValue(
        juce::Decibels::decibelsToGain(gainParam->get()));

    mix = 1.0f;
    mixSmoother.setCurrentAndTargetValue(mixParam->get() * 0.01f);
}

void Parameters::smoothen() noexcept
{
    gain = gainSmoother.getNextValue();
    delayTime += (targetDelayTime - delayTime) * coeff; //The infamous one-pole filter formula
    mix = mixSmoother.getNextValue();
}
