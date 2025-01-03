/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ProtectYourEars.h"

//==============================================================================
Delay1_0AudioProcessor::Delay1_0AudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
    ),
    params(apvts)
{
    lowCutFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    highCutFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
}

Delay1_0AudioProcessor::~Delay1_0AudioProcessor()
{
}

//==============================================================================
const juce::String Delay1_0AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Delay1_0AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Delay1_0AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Delay1_0AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Delay1_0AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Delay1_0AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Delay1_0AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Delay1_0AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Delay1_0AudioProcessor::getProgramName (int index)
{
    return {};
}

void Delay1_0AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Delay1_0AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    params.prepareToPlay(sampleRate);
    params.reset();

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = juce::uint32(samplesPerBlock);
    spec.numChannels = 2;

    delayLine.prepare(spec);

    double numSamples = Parameters::maxDelayTime / 1000.0 * sampleRate;
    int maxDelayInSamples = int(std::ceil(numSamples));
    delayLine.setMaximumDelayInSamples(maxDelayInSamples);
    delayLine.reset();

    feedbackL = 0.0f;
    feedbackR = 0.0f;

    lowCutFilter.prepare(spec);
    lowCutFilter.reset();

    highCutFilter.prepare(spec);
    highCutFilter.reset();

    lastLowCut = -1.0f;
    lastHighCut = -1.0f;

    //DBG(maxDelayInSamples);
    tempo.reset();
}

void Delay1_0AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool Delay1_0AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto mono = juce::AudioChannelSet::mono();
    const auto stereo = juce::AudioChannelSet::stereo();
    const auto mainIn = layouts.getMainInputChannelSet();
    const auto mainOut = layouts.getMainOutputChannelSet();

    if (mainIn == mono && mainOut == mono) { return true; }
    if (mainIn == mono && mainOut == stereo) { return true; }
    if (mainIn == stereo && mainOut == stereo) { return true; }

    return false;
}

void Delay1_0AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, [[maybe_unused]] juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    params.update();

    tempo.update(getPlayHead());

    float syncedTime = float(tempo.getMilisecondsForNoteLength(params.delayNote));
    if (syncedTime > Parameters::maxDelayTime) {
        syncedTime = Parameters::maxDelayTime;
    }

    float sampleRate = float(getSampleRate());

    auto mainInput = getBusBuffer(buffer, true, 0);
    auto mainInputChannels = mainInput.getNumChannels();
    auto isMainInputStereo = mainInputChannels > 1;
    const float* inputDataL = mainInput.getReadPointer(0);
    const float* inputDataR = mainInput.getReadPointer(isMainInputStereo ? 1 : 0);

    auto mainOutput = getBusBuffer(buffer, false, 0);
    auto mainOutputChannels = mainOutput.getNumChannels();
    auto isMainOutputStereo = mainOutputChannels > 1;
    float* outputDataL = mainOutput.getWritePointer(0);
    float* outputDataR = mainOutput.getWritePointer(isMainOutputStereo ? 1 : 0);

    if (isMainOutputStereo) 
    {
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) //Stereo audio processing loop
        {
            params.smoothen();

            float delayTime = params.tempoSync ? syncedTime : params.delayTime;
            /*
            The elongated way of writing the previous line of code which uses the ? : syntax (mini if statement) is

            float delayTime;
            if (params.tempoSync) {
                delayTime = syncedTime;
            } else {
                delayTime = params.delayTime;
            }
            */
            float delayInSamples = delayTime / 1000.0f * sampleRate;
            delayLine.setDelay(delayInSamples);

            if (params.lowCut != lastLowCut) {
                lowCutFilter.setCutoffFrequency(params.lowCut);
                lastLowCut = params.lowCut;
            }

            if (params.highCut != lastHighCut) {
                highCutFilter.setCutoffFrequency(params.highCut);
                lastHighCut = params.highCut;
            }

            float dryL = inputDataL[sample];
            float dryR = inputDataR[sample];

            float mono = (dryL + dryR) * 0.5f;

            delayLine.pushSample(0, mono * params.panL + feedbackR);
            delayLine.pushSample(1, mono * params.panR + feedbackL);

            float wetL = delayLine.popSample(0);
            float wetR = delayLine.popSample(1);

            feedbackL = wetL * params.feedback;
            feedbackL = lowCutFilter.processSample(0, feedbackL);
            feedbackL = highCutFilter.processSample(0, feedbackL);

            feedbackR = wetR * params.feedback;
            feedbackR = lowCutFilter.processSample(1, feedbackR);
            feedbackR = highCutFilter.processSample(1, feedbackR);

            float mixL = dryL + wetL * params.mix; // a * (1-c) + b * c where c is some value between 0 and 1 is linear interpolation
            float mixR = dryR + wetR * params.mix;

            outputDataL[sample] = mixL * params.gain;
            outputDataR[sample] = mixR * params.gain;
        }
    }
    else {
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) //Mono processing loop
        {
            params.smoothen();

            float delayInSamples = params.delayTime / 1000.0f * sampleRate;
            delayLine.setDelay(delayInSamples);

            float dry = inputDataL[sample];
            delayLine.pushSample(0, dry + feedbackL);

            float wet = delayLine.popSample(0);
            feedbackL = wet * params.feedback;

            float mix = dry + wet * params.mix;
            outputDataL[sample] = mix * params.gain;
        }
    }

        
#if JUCE_DEBUG
    protectYourEars(buffer);
#endif
}

//==============================================================================
bool Delay1_0AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Delay1_0AudioProcessor::createEditor()
{
    return new Delay1_0AudioProcessorEditor (*this);
}

//==============================================================================
void Delay1_0AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    copyXmlToBinary(*apvts.copyState().createXml(), destData);
    //DBG(apvts.copyState().toXmlString());
}

void Delay1_0AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml.get() != nullptr && xml->hasTagName(apvts.state.getType())) {
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Delay1_0AudioProcessor();
}
