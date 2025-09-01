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
    this->isOn = true;
    this->ledColour = onColour;
    this->repaint();
}

void PowerLED::toggleOff(){
    this->isOn = false;
    this->ledColour = offColour;
    this->repaint();
}

void PowerLED::paint (juce::Graphics& g)
{
    this->width = getWidth();
    this->height = getHeight();
    
    g.setColour(juce::Colours::white);
    g.drawRect(0.0f, 0.0f, this->width, this->height);
    
    if (this->isOn){
        createGlow(g);
    }
    
    // LED border
    g.setColour(juce::Colours::black);
    drawCenteredCircle(g, this->radius + 1.25f);
    
    // LED
    g.setColour(ledColour);
    drawCenteredCircle(g, this->radius);
}

void PowerLED::createGlow(juce::Graphics& g){
    // Start from edge, work inwards
    float alpha = 0.0f;
    float tempRadius = this->width;
    
    // optimize: first one technically draws nothing -> adjust starting alpha and radius
    // optimize: don't need to draw past the groove -> stop at groove radius
    while (tempRadius > radius){
        g.setColour(ledColour.withAlpha(alpha));
        drawCenteredCircle(g, tempRadius);
        tempRadius *= 0.85f;
        alpha = 1 / juce::square(tempRadius / this->radius);
    }
}

void PowerLED::drawCenteredCircle(juce::Graphics& g, float radius){
    float centerX = centerHorizontal(this->width, radius);
    float centerY = centerVertical(this->height, radius);
    
    g.fillEllipse(centerX, centerY, radius, radius);
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

float PowerLED::centerHorizontal(float containerWidth, float width){
    return containerWidth / 2 - width / 2;
}

float PowerLED::centerVertical(float containerHeight, float height){
    return containerHeight / 2 - height / 2;
}
