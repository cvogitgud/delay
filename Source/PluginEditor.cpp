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
mix(audioProcessor.treeState, "MIX", "Mix"), delay(audioProcessor.treeState, "DELAYTIME", "Delay"), feedback(audioProcessor.treeState, "FEEDBACK", "Feedback"), rate(audioProcessor.treeState, "RATE", "Rate"), depth(audioProcessor.treeState, "DEPTH", "Depth"), power(audioProcessor.treeState, "POWER"), led(juce::Colours::red, 10.0f)
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
    
    led.setLEDColour(juce::Colours::red);
    addAndMakeVisible(led);
}

ProcrastinatorAudioProcessorEditor::~ProcrastinatorAudioProcessorEditor()
{
}

//==============================================================================
void ProcrastinatorAudioProcessorEditor::paint (juce::Graphics& g)
{
    // BG
    g.fillAll (juce::Colours::darkslateblue);
    
//    // Border for Dials
//    float dialBorderWidth = getWidth() - 20;
//    float dialBorderHeight = getHeight() / 2 - 30;
//    float dialBorderX = getWidth() / 2 - dialBorderWidth / 2;
//    float dialBorderY = -1.0;
//    juce::Rectangle dialBorder = juce::Rectangle<float>(dialBorderX, dialBorderY, dialBorderWidth, dialBorderHeight);
//
//    g.setColour(juce::Colours::whitesmoke);
//    g.drawRoundedRectangle(dialBorder, 2.0f, 2.0f);
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
    
    int ledRadius = 15.0f;
    int ledX = getWidth() / 2 - ledRadius / 2;
    int ledY = 25;
    led.setBounds(ledX, ledY, ledRadius, ledRadius);
}

void ProcrastinatorAudioProcessorEditor::togglePowerLED(){
    // change colour and call repaint PowerLED
    if (power.getButton().getToggleState() == true){
        led.toggleOn();
    }
    else{
        led.toggleOff();
    }
}
