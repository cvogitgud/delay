/*
  ==============================================================================

    PowerSwitch.h
    Created: 26 Aug 2025 11:44:31am
    Author:  Chris

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class PowerSwitch  : public juce::Component
{
public:
    PowerSwitch(juce::AudioProcessorValueTreeState& treeState, juce::String parameterID);
    ~PowerSwitch() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    juce::TextButton& getButton();
    
private:
    juce::Colour switchColour = juce::Colours::black;
    juce::TextButton button;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachment;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PowerSwitch)
};
