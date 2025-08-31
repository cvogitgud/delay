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
    this->onColour = colour;
    this->ledColour = colour;
}

PowerLED::~PowerLED()
{
}

void PowerLED::setLEDColour(juce::Colour colour){
    this->ledColour = colour;
}

void PowerLED::setRadius(float radius){
    this->radius = radius;
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
    auto width = getWidth();
    auto height = getHeight();
    
    g.setColour(juce::Colours::white);
    g.drawRect(0, 0, width, height);
    
    
    float alpha = 1.0f;
    float tempRadius = radius;
    int i = 1;
    while (tempRadius <= width){
        int centerX = width / 2 - tempRadius / 2;
        int centerY = height / 2 - tempRadius / 2;
        
        g.setColour(ledColour.withAlpha(alpha));
        g.fillEllipse(centerX, centerY, tempRadius, tempRadius);
        
        i += 1;
        alpha = 1.0f / juce::square(i);
        tempRadius = radius * i;
    }
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
