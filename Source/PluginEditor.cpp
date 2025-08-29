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
    
    pedalLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    pedalLabel.setJustificationType(juce::Justification::centred);
    pedalLabel.setFont(juce::FontOptions(30.0f));
    pedalLabel.setText(pedalName, juce::dontSendNotification);
    addAndMakeVisible(pedalLabel);
}

ProcrastinatorAudioProcessorEditor::~ProcrastinatorAudioProcessorEditor()
{
}

//==============================================================================
void ProcrastinatorAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkslateblue);
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
    
    int labelWidth = getWidth();
    int labelHeight = powerY - mix.getBottom();
    int labelX = getWidth() / 2 - labelWidth / 2;
    int labelY = mix.getBottom();
    pedalLabel.setBounds(labelX, labelY, labelWidth, labelHeight);
}

void ProcrastinatorAudioProcessorEditor::togglePowerLED(){
    if (power.getButton().getToggleState() == true){
        led.toggleOn();
    }
    else{
        led.toggleOff();
    }
}
