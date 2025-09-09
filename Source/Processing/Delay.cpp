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
    
    mix.reset(sampleRate, 0.02);
    feedback.reset(sampleRate, 0.02);
    rate.reset(sampleRate, 0.02);
    
    channelStates.resize(numChannels);
    for (int channel = 0; channel < numChannels; ++channel){
        channelStates[channel].channel = channel;
        channelStates[channel].delayIndex = 0;
        channelStates[channel].lfo.prepare(spec);
        channelStates[channel].lfo.initialise([](float x) { return sin(x); });
        channelStates[channel].lfo.setFrequency(rate.getNextValue());
    }
    
    centerDelayLength = (int)(sampleRate / 2);
    delayLength.reset(sampleRate, 0.02);
    maxDelayLength = (int)sampleRate;

    delayBuffer.setSize(numChannels, maxDelayLength);
    delayBuffer.clear();
    
    this->isPrepared = true;
}

float Delay::processSample(int channel, float input){
    
    jassert (isPrepared);
    
    ChannelState* channelState = &channelStates[channel];
    
    float delayOutput = readFromBuffer(channelState);
    writeToBuffer(channelState, input);
    
    float lfoValue = channelState->lfo.processSample(0.0f);
    float modulationLength = lfoValue * convertMStoSample(depth);
    
    float targetLength = limitDelayLength(centerDelayLength + modulationLength);
    delayLength.setTargetValue(targetLength);
    
    channelState->delayIndex++;
    if (channelState->delayIndex >= delayLength.getNextValue()){
        channelState->delayIndex -= delayLength.getNextValue();
    }
    
    // full dry signal <= 50% wet, true dry/wet ratio > 50%
    float output = 0.0f;
    if (mix.getNextValue() <= 0.5){
        output = input + mix.getNextValue() * delayOutput;
    }
    else {
        output = (1.0f - mix.getNextValue()) * input + mix.getNextValue() * delayOutput;
    }
    return limitOutput(output);
}

float Delay::readFromBuffer(ChannelState* channelState){
//    size_t i = static_cast<size_t> (channelState->delayIndex);
//    float belowSample = delayBuffer.getSample(channelState->channel, i);
//    float aboveSample = delayBuffer.getSample(channelState->channel, i+1);
//    float delayOutput = lerp(belowSample, aboveSample, channelState->delayIndex - 1);
    return delayBuffer.getSample(channelState->channel, channelState->delayIndex);
}

void Delay::writeToBuffer(ChannelState* channelState, float input){
    float delayOutput = readFromBuffer(channelState);
    float delayInput = input + delayOutput * feedback.getNextValue();
    delayBuffer.setSample(channelState->channel, channelState->delayIndex, delayInput);
}

void Delay::setDelayLength(const int delayTime_ms){
    
    jassert(isPrepared);
    jassert(delayTime_ms > 0);
    
    centerDelayLength = convertMStoSample(delayTime_ms);
    
    if (centerDelayLength > maxDelayLength){
        centerDelayLength = maxDelayLength;
    }
}

void Delay::setMix(const float mix){
    
    jassert(isPrepared);
    
    this->mix.setTargetValue(mix);
}

void Delay::setFeedback(const float feedback){
    
    jassert(isPrepared);
    
    this->feedback.setTargetValue(feedback);
}

void Delay::setRate(const float rate){
    
    jassert(isPrepared);
    
    this->rate.setTargetValue(rate);
    
    for (int channel = 0; channel < channelStates.size(); channel++){
        channelStates[channel].lfo.setFrequency(rate);
    }
    
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
        channelStates[channel].lfo.reset();
    }
    
}

//-----------------------------------------------------------------------------
// Utility
//-----------------------------------------------------------------------------
float Delay::convertMStoSample(const float time){
    return 0.001f * time * lastSampleRate;
}

float Delay::lerp(float a, float b, float f)
{
    return a * (1.0 - f) + (b * f);
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

