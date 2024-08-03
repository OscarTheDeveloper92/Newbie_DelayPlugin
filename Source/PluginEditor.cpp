/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Delay1_0AudioProcessorEditor::Delay1_0AudioProcessorEditor (Delay1_0AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    addAndMakeVisible(gainKnob);

    setSize (500, 330); //Why does everything go before setSize?
}

Delay1_0AudioProcessorEditor::~Delay1_0AudioProcessorEditor()
{
}

//==============================================================================
void Delay1_0AudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::darkgrey);
}

void Delay1_0AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    gainKnob.setTopLeftPosition(215, 120);
}
