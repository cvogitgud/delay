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
mix(audioProcessor.treeState, "MIX", "Mix"), delay(audioProcessor.treeState, "DELAYTIME", "Delay"), feedback(audioProcessor.treeState, "FEEDBACK", "Feedback"), rate(audioProcessor.treeState, "RATE", "Rate"), depth(audioProcessor.treeState, "DEPTH", "Depth"), power(audioProcessor.treeState, "POWER"), led(juce::Colours::red)
{
    int width = 300;
    int height = width * 7/5;
    setSize (width, height);
    
    addAndMakeVisible(mix);
    addAndMakeVisible(delay);
    addAndMakeVisible(feedback);
    addAndMakeVisible(rate);
    addAndMakeVisible(depth);
    
    addAndMakeVisible(power);
    power.getButton().onClick = [this] { togglePowerLED(); };
    
    addAndMakeVisible(led);
}

ProcrastinatorAudioProcessorEditor::~ProcrastinatorAudioProcessorEditor()
{
}

//==============================================================================
void ProcrastinatorAudioProcessorEditor::paint (juce::Graphics& g)
{
    juce::Colour bgColour = juce::Colours::darkslateblue;
    g.fillAll (bgColour);
    
    juce::Colour borderColour = juce::Colours::black;
    g.setColour (borderColour);
    juce::Rectangle pluginBorder = juce::Rectangle<float>(0.0, 0.0, (float)getWidth(), (float)getHeight());
    g.drawRoundedRectangle(pluginBorder, 9.0f, 4);
}

void ProcrastinatorAudioProcessorEditor::resized()
{
    int dialWidth = getWidth() / 3;
    int dialHeight = getHeight() / 5;
    
    rate.setBounds(0, 0, dialWidth, dialHeight);
    depth.setBounds(getWidth() - dialWidth, 0, dialWidth, dialHeight);
    
    int bottomRowY = rate.getBottom();
    mix.setBounds(0, bottomRowY, dialWidth, dialHeight);
    delay.setBounds(mix.getRight(), bottomRowY, dialWidth, dialHeight);
    feedback.setBounds(delay.getRight(), bottomRowY, dialWidth, dialHeight);
    
    int bottomMargin = 20;
    int powerSwitchWidth = getWidth() * 0.75;
    int powerSwitchHeight = getHeight() * 0.35;
    int powerX = getWidth() / 2 - powerSwitchWidth / 2;
    int powerY = getHeight() - powerSwitchHeight - bottomMargin;
    power.setBounds(powerX, powerY, powerSwitchWidth, powerSwitchHeight);
    
    int ledRadius = 10.0f;
    led.setBounds(getWidth() / 2 - ledRadius / 2, 25, ledRadius, ledRadius);
}

void ProcrastinatorAudioProcessorEditor::togglePowerLED(){
    // change colour and call repaint PowerLED
    if (power.getButton().getToggleState() == true){
        led.setLEDColour(juce::Colours::red);
    }
    else{
        led.setLEDColour(juce::Colour(182,182,182).brighter(0.4f));
    }
    led.repaint();
}
