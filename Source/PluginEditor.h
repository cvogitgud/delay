/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/Dial.h"
#include "UI/PowerSwitch.h"
#include "UI/PowerLED.h"

//==============================================================================
/**
*/
class ProcrastinatorAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ProcrastinatorAudioProcessorEditor (ProcrastinatorAudioProcessor&);
    ~ProcrastinatorAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void togglePowerLED();

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ProcrastinatorAudioProcessor& audioProcessor;
    
    Dial mix;
    Dial delay;
    Dial feedback;
    Dial rate;
    Dial depth;
    
    PowerSwitch power;
    PowerLED led;
    
    juce::String pedalName = "Replicator";
    juce::Label pedalLabel {"PEDALLABEL", pedalName};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProcrastinatorAudioProcessorEditor)
};
