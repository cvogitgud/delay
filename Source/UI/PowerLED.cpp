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
PowerLED::PowerLED(juce::Colour colour, float radius)
{
    this->onColour = colour;
    this->ledColour = colour;
    this->radius = radius;
}

PowerLED::~PowerLED()
{
}

void PowerLED::setLEDColour(juce::Colour colour){
    this->ledColour = colour;
}

void PowerLED::toggleOn(){
    this->ledColour = onColour;
    this->repaint();
}

void PowerLED::toggleOff(){
    this->ledColour = offColour;
    this->repaint();
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

bool PowerLED::hitTest(int x, int y){
    bool inXBounds = (0 <= x && x <= radius);
    bool inYBounds = (0 <= y && y <= radius);
    return inXBounds && inYBounds;
}
