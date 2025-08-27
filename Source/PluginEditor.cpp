/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ProcrastinatorAudioProcessorEditor::ProcrastinatorAudioProcessorEditor (ProcrastinatorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
mix(audioProcessor.treeState, "MIX", "Mix"), delay(audioProcessor.treeState, "DELAYTIME", "Delay"), feedback(audioProcessor.treeState, "FEEDBACK", "Feedback"), rate(audioProcessor.treeState, "RATE", "Rate"), depth(audioProcessor.treeState, "DEPTH", "Depth")
{
    int width = 300;
    int height = width * 7/5;
    setSize (width, height);
    
    addAndMakeVisible(mix);
    addAndMakeVisible(delay);
    addAndMakeVisible(feedback);
    addAndMakeVisible(rate);
    addAndMakeVisible(depth);
}

ProcrastinatorAudioProcessorEditor::~ProcrastinatorAudioProcessorEditor()
{
}

//==============================================================================
void ProcrastinatorAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void ProcrastinatorAudioProcessorEditor::resized()
{
    /*
     TOP ROW: Feedback - Rate - Depth
     BOT ROW: Mix             - Delay
     Lower Half: Power switch
     */
    
    int dialWidth = getWidth() / 3;
    int dialHeight = getHeight() / 4;
    
    mix.setBounds(0, 0, dialWidth, dialHeight);
    delay.setBounds(mix.getRight(), 0, dialWidth, dialHeight);
    feedback.setBounds(delay.getRight(), 0, dialWidth, dialHeight);
    
    rate.setBounds(0, dialHeight, dialWidth, dialHeight);
    depth.setBounds(delay.getRight(), dialHeight, dialWidth, dialHeight);
    
    
}
