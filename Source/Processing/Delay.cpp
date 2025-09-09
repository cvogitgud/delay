/*
  ==============================================================================

    Delay.cpp
    Created: 21 Aug 2025 3:35:32pm
    Author:  Chris

  ==============================================================================
*/

#include "Delay.h"

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
        channelStates[channel].lfo.prepare(spec);
        channelStates[channel].lfo.initialise([](float x) { return sin(x); });
        channelStates[channel].lfo.setFrequency(rate.getNextValue());
    }
    
    maxDelayLength = (int)sampleRate;

    delayBuffer.setSize(numChannels, maxDelayLength);
    delayBuffer.clear();
    
    isPrepared = true;
    
    reset();
}

void Delay::reset(){
    
    mix = DEFAULT_MIX;
    feedback = DEFAULT_FEEDBACK;
    rate = DEFAULT_RATE;
    depth = DEFAULT_DEPTH;
    
    dryGain.reset(lastSampleRate, 0.02);
    wetGain.reset(lastSampleRate, 0.02);
    feedback.reset(lastSampleRate, 0.02);
    rate.reset(lastSampleRate, 0.02);
    
    centerDelayLength = (int) lastSampleRate / 2;
    delayLength.reset(lastSampleRate, 0.02);
    delayBuffer.clear();
    
    for (int channel = 0; channel < channelStates.size(); ++channel){
        channelStates[channel].delayIndex = 0;
        channelStates[channel].lfo.reset();
    }
    
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
    
    // Balanced Dry/Wet Mixing Rule
    dryGain.setTargetValue(2.0f * juce::jmin(0.5f, 1.0f - mix));
    wetGain.setTargetValue(2.0f * juce::jmin(0.5f, mix));
    float output = dryGain.getNextValue() * input + wetGain.getNextValue() * delayOutput;
    
    return limitOutput(output);
}

float Delay::readFromBuffer(ChannelState* channelState){

//    return interpolateSample(channelState);
    return delayBuffer.getSample(channelState->channel, channelState->delayIndex);
}

float Delay::interpolateSample(ChannelState* channelState){
    size_t index1 = static_cast<size_t> (std::floor (channelState->delayIndex));
    size_t index2 = index1 + 1;
    
    if (index2 >= maxDelayLength){
        index1 %= maxDelayLength;
        index2 %= maxDelayLength;
    }
    
    auto value1 = delayBuffer.getSample (channelState->channel, index1);
    auto value2 = delayBuffer.getSample (channelState->channel, index2);

    return value1 + (channelState->delayIndex - 1) * (value2 - value1);
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

void Delay::setMix(const float newValue){
    
    jassert(isPrepared);
    
    this->mix = newValue;
}

void Delay::setFeedback(const float newValue){
    
    jassert(isPrepared);
    
    this->feedback.setTargetValue(newValue);
}

void Delay::setRate(const float newValue){
    
    jassert(isPrepared);
    
    this->rate.setTargetValue(newValue);
    
    for (int channel = 0; channel < channelStates.size(); channel++){
        channelStates[channel].lfo.setFrequency(rate.getNextValue());
    }
    
}

void Delay::setDepth(const int depth){
    
    jassert(isPrepared);
    
    this->depth = depth;
}

void Delay::clearDelayLine(){
    delayBuffer.clear();
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

