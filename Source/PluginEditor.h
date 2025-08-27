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

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ProcrastinatorAudioProcessor& audioProcessor;
    
    Dial mix;
    Dial delay;
    Dial feedback;
    Dial rate;
    Dial depth;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProcrastinatorAudioProcessorEditor)
};
