/*
  ==============================================================================

    Delay.cpp
    Created: 21 Aug 2025 3:35:32pm
    Author:  Chris

  ==============================================================================
*/

#include "Delay.h"

Delay::Delay(){
    
}

void Delay::prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels){
    lastSampleRate = sampleRate;
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.numChannels = numChannels;
    spec.maximumBlockSize = samplesPerBlock;
    
    channelStates.resize(numChannels);
    for (int channel = 0; channel < numChannels; ++channel){
        channelStates[channel].channel = channel;
        channelStates[channel].delayIndex = 0;
    }
    
    centerDelayLength = (int)(sampleRate / 2);
    maxDelayLength = (int)sampleRate;

    delayBuffer.setSize(numChannels, maxDelayLength);
    delayBuffer.clear();
    
    lfo.prepare(spec);
    lfo.initialise([](float x) { return sin(x); });
    lfo.setFrequency(rate);
}

float Delay::updateDelayBuffer(float input, int channel){
    ChannelState* channelState = &channelStates[channel];
    
    float delayOutput = delayBuffer.getSample(channelState->channel, channelState->delayIndex);
    float delayInput = input + delayOutput * feedback;
    delayBuffer.setSample(channelState->channel, channelState->delayIndex, delayInput);
    
    float lfoValue = lfo.processSample(0.0f);
    int modulationLength = (int)(lfoValue * convertMStoSample(depth));
    
    int delayLength = centerDelayLength + modulationLength;
    
    if (delayLength < 0){
        delayLength = 0;
    }
    if (delayLength >= maxDelayLength){
        delayLength = maxDelayLength;
    }
    
    channelState->delayIndex++;
    if (channelState->delayIndex >= delayLength){
        channelState->delayIndex = 0;
    }
    
    float output = (1.0f - mix) * input + mix * delayOutput;
    return limitOutput(output);
}

int Delay::convertMStoSample(const int time){
    return (unsigned int) (0.001f * time * lastSampleRate);
}

void Delay::setDelayLength(const int delayTime_ms){
    centerDelayLength = convertMStoSample(delayTime_ms);
    
    if (centerDelayLength > maxDelayLength){
        centerDelayLength = maxDelayLength;
    }
}

void Delay::setMix(const float mix){
    this->mix = mix;
}

void Delay::setFeedback(const float feedback){
    this->feedback = feedback;
}

void Delay::setRate(const int rate){
    this->rate = rate;
    lfo.setFrequency(rate);
}

void Delay::setDepth(const int depth){
    this->depth = depth;
}

void Delay::updateParameters(){
    
}

void Delay::reset(){
    // reset values
    // reset lfo
    lfo.reset();
}

float Delay::limitOutput(float value){
    
    float output = 0.0f;
    
    if (value > 1.0f){
        output = 1.0f;
    }
    else if (value < -1.0f){
        output = -1.0f;
    }
    else {
        output = value;
    }
    
    return output;
}
