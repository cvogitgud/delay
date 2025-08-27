/*
  ==============================================================================

    PowerLED.h
    Created: 26 Aug 2025 11:44:20am
    Author:  Chris

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class PowerLED  : public juce::Component
{
public:
    PowerLED(juce::Colour colour);
    ~PowerLED() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void setLEDColour(juce::Colour colour);

private:
    float radius = 10.0f;
    juce::Colour ledColour;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PowerLED)
};
