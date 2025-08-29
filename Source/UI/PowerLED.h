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
    PowerLED(juce::Colour colour, float radius);
    ~PowerLED() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void toggleOn();
    void toggleOff();
    void setLEDColour(juce::Colour colour);
    
    bool hitTest(int x, int y) override;

private:
    float radius;
    juce::Colour ledColour;
    juce::Colour onColour;
    juce::Colour offColour = juce::Colours::black;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PowerLED)
};
