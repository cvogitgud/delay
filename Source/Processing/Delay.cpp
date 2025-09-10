/*
  ==============================================================================

    Delay.cpp
    Created: 21 Aug 2025 3:35:32pm
    Author:  Chris

  ==============================================================================
*/

#include "Delay.h"

template<typename SampleType>
void Delay<SampleType>::prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels){
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
    
    maxDelayinSamples = sampleRate;

    delayBuffer.setSize(numChannels, maxDelayinSamples + 2);
    delayBuffer.clear();
    
    isPrepared = true;
    
    reset();
}

template<typename SampleType>
void Delay<SampleType>::reset(){
    
    mix = DEFAULT_MIX;
    feedback = DEFAULT_FEEDBACK;
    rate = DEFAULT_RATE;
    depth = DEFAULT_DEPTH;
    
    dryGain.reset(lastSampleRate, 0.02);
    wetGain.reset(lastSampleRate, 0.02);
    feedback.reset(lastSampleRate, 0.02);
    rate.reset(lastSampleRate, 0.02);
    
    centerDelayLength = lastSampleRate / 2;
    delayLength.reset(lastSampleRate, 0.02);
    delayBuffer.clear();
    
    for (int channel = 0; channel < channelStates.size(); ++channel){
        channelStates[channel].delayIndex = 0;
        channelStates[channel].lfo.reset();
    }
    
}

template<typename SampleType>
SampleType Delay<SampleType>::processSample(int channel, SampleType input){
    
    jassert (isPrepared);
    
    ChannelState* channelState = &channelStates[channel];
    
    SampleType delayOutput = readFromBuffer(channelState);
    writeToBuffer(channelState, input);
    
    float lfoValue = channelState->lfo.processSample(0.0f);
    SampleType modulationLength = lfoValue * convertMStoSample(depth);

    delayLength.setTargetValue(limitDelayLength(centerDelayLength + modulationLength));
    
    channelState->delayIndex++;
    if (channelState->delayIndex >= delayLength.getNextValue()){
        channelState->delayIndex -= delayLength.getNextValue();
    }

    // Balanced Dry/Wet Mixing Rule
    dryGain.setTargetValue(2.0f * juce::jmin(0.5f, 1.0f - mix));
    wetGain.setTargetValue(2.0f * juce::jmin(0.5f, mix));
    SampleType output = dryGain.getNextValue() * input + wetGain.getNextValue() * delayOutput;
    
    return limitOutput(output);
}

template<typename SampleType>
SampleType Delay<SampleType>::readFromBuffer(ChannelState* channelState){

//    return interpolateSample(channelState);
    return delayBuffer.getSample(channelState->channel, channelState->delayIndex);
}

template<typename SampleType>
void Delay<SampleType>::writeToBuffer(ChannelState* channelState, SampleType input){
    SampleType delayOutput = readFromBuffer(channelState);
    SampleType delayInput = input + delayOutput * feedback.getNextValue();
    delayBuffer.setSample(channelState->channel, channelState->delayIndex, delayInput);
}

template<typename SampleType>
SampleType Delay<SampleType>::interpolateSample(ChannelState* channelState){
    size_t index1 = static_cast<size_t> (std::floor (channelState->delayIndex));
    size_t index2 = index1 + 1;
    
    if (index2 >= maxDelayinSamples){
        index1 %= maxDelayinSamples;
        index2 %= maxDelayinSamples;
    }
    
    auto value1 = delayBuffer.getSample (channelState->channel, index1);
    auto value2 = delayBuffer.getSample (channelState->channel, index2);

    return value1 + (channelState->delayIndex - 1) * (value2 - value1);
}

template<typename SampleType>
void Delay<SampleType>::setDelayLength(const SampleType delayTime_ms){
    
    jassert(isPrepared);
    jassert(delayTime_ms > 0);
    
    centerDelayLength = convertMStoSample(delayTime_ms);
    
    if (centerDelayLength > maxDelayinSamples){
        centerDelayLength = maxDelayinSamples;
    }
}

template<typename SampleType>
void Delay<SampleType>::setMix(const float newValue){
    
    jassert(isPrepared);
    
    this->mix = newValue;
}

template<typename SampleType>
void Delay<SampleType>::setFeedback(const float newValue){
    
    jassert(isPrepared);
    
    this->feedback.setTargetValue(newValue);
}

template<typename SampleType>
void Delay<SampleType>::setRate(const float newValue){
    
    jassert(isPrepared);
    
    this->rate.setTargetValue(newValue);
    
    for (int channel = 0; channel < channelStates.size(); channel++){
        channelStates[channel].lfo.setFrequency(rate.getNextValue());
    }
    
}

template<typename SampleType>
void Delay<SampleType>::setDepth(const SampleType depth){
    
    jassert(isPrepared);
    
    this->depth = depth;
}

template<typename SampleType>
void Delay<SampleType>::clearDelayLine(){
    delayBuffer.clear();
}

//-----------------------------------------------------------------------------
// Utility
//-----------------------------------------------------------------------------
template<typename SampleType>
SampleType Delay<SampleType>::convertMStoSample(const SampleType time){
    return (0.001f * time * lastSampleRate);
}

template<typename SampleType>
SampleType Delay<SampleType>::lerp(SampleType a, SampleType b, SampleType f)
{
    return a * (1.0 - f) + (b * f);
}

template<typename SampleType>
SampleType Delay<SampleType>::limitDelayLength(SampleType delayLength){

    SampleType result = delayLength;
    return juce::jlimit(1.0f, (float) maxDelayinSamples, (float) result);
}

template<typename SampleType>
SampleType Delay<SampleType>::limitOutput(SampleType value){
    
    SampleType output = 0.0f;
    
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

//=============================================================================
template class Delay<float>;
template class Delay<int>;
