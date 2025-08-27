/*
  ==============================================================================

    PowerLED.cpp
    Created: 26 Aug 2025 11:44:20am
    Author:  Chris

  ==============================================================================
*/


#include <JuceHeader.h>
#include "PowerLED.h"

//==============================================================================
PowerLED::PowerLED(juce::Colour colour)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    this->ledColour = colour;
}

PowerLED::~PowerLED()
{
}

void PowerLED::setLEDColour(juce::Colour colour){
    this->ledColour = colour;
}

void PowerLED::paint (juce::Graphics& g)
{
    g.setColour(ledColour);
    g.fillEllipse(0, 0, radius, radius);
}

void PowerLED::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
