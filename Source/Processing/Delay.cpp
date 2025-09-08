/*
  ==============================================================================

    Delay.cpp
    Created: 21 Aug 2025 3:35:32pm
    Author:  Chris

  ==============================================================================
*/

#include "Delay.h"

void Delay::prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels){
    this->lastSampleRate = sampleRate;
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.numChannels = numChannels;
    spec.maximumBlockSize = samplesPerBlock;
    
    channelStates.resize(numChannels);
    for (int channel = 0; channel < numChannels; ++channel){
        channelStates[channel].channel = channel;
        channelStates[channel].delayIndex = 0;
    }
    
    this->centerDelayLength = (int)(sampleRate / 2);
    this->maxDelayLength = (int)sampleRate;

    delayBuffer.setSize(numChannels, maxDelayLength);
    delayBuffer.clear();
    
    lfo.prepare(spec);
    lfo.initialise([](float x) { return sin(x); });
    
    lfo.setFrequency(rate);
    
    this->isPrepared = true;
}

float Delay::processSample(int channel, float input){
    
    jassert (isPrepared);
    
    ChannelState* channelState = &channelStates[channel];
    
    float delayOutput = delayBuffer.getSample(channelState->channel, channelState->delayIndex);
    float delayInput = input + delayOutput * feedback;
    delayBuffer.setSample(channelState->channel, channelState->delayIndex, delayInput);
    
    float lfoValue = lfo.processSample(0.0f);
    int modulationLength = (int)(lfoValue * convertMStoSample(depth));
    
    int delayLength = limitDelayLength(centerDelayLength + modulationLength);
    
    channelState->delayIndex++;
    if (channelState->delayIndex >= delayLength){
        channelState->delayIndex -= delayLength;
    }
    
    float output = 0.0f;
    if (mix <= 0.5){
        output = input + mix * delayOutput;
    }
    else {
        output = (1.0f - mix) * input + mix * delayOutput;
    }
    return limitOutput(output);
}

void Delay::setDelayLength(const int delayTime_ms){
    
    jassert(isPrepared);
    
    centerDelayLength = convertMStoSample(delayTime_ms);
    
    if (centerDelayLength > maxDelayLength){
        centerDelayLength = maxDelayLength;
    }
}

void Delay::setMix(const float mix){
    
    jassert(isPrepared);
    
    this->mix = mix;
}

void Delay::setFeedback(const float feedback){
    
    jassert(isPrepared);
    
    this->feedback = feedback;
}

void Delay::setRate(const float rate){
    
    jassert(isPrepared);
    
    this->rate = rate;
    lfo.setFrequency(rate);
}

void Delay::setDepth(const int depth){
    
    jassert(isPrepared);
    
    this->depth = depth;
}

void Delay::clearDelayLine(){
    delayBuffer.clear();
}

void Delay::reset(){
    
    jassert(isPrepared);
    
    this->mix = DEFAULT_MIX;
    this->feedback = DEFAULT_FEEDBACK;
    this->rate = DEFAULT_RATE;
    this->depth = DEFAULT_DEPTH;
    this->mix = DEFAULT_MIX;
    
    this->centerDelayLength = lastSampleRate / 2;
    delayBuffer.clear();
    
    for (int channel = 0; channel < channelStates.size(); ++channel){
        channelStates[channel].delayIndex = 0;
    }
    
    lfo.reset();
}

int Delay::limitDelayLength(int delayLength){
    
    int result = delayLength;
    if (delayLength < 0){
        result = 0;
    }
    if (delayLength >= maxDelayLength){
        result = maxDelayLength;
    }
    return result;
}

int Delay::convertMStoSample(const int time){
    return (unsigned int) (0.001f * time * lastSampleRate);
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
