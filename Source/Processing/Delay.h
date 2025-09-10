/*
  ==============================================================================

    Delay.h
    Created: 21 Aug 2025 3:35:32pm
    Author:  Chris

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#define DEFAULT_MIX 0.5
#define DEFAULT_FEEDBACK 0.5
#define DEFAULT_RATE 0.01f
#define DEFAULT_DEPTH 0.0

template<typename SampleType>
class Delay {
public:
    void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);
    void reset();
    SampleType processSample(int channel, SampleType sample);
    
    void setDelayLength(const SampleType delayTime_ms);
    void setMix(const float mix);
    void setFeedback(const float feedback);
    void setRate(const float rate);
    void setDepth(const SampleType depth);
    
    void clearDelayLine();
private:
    
    typedef struct {
        int channel;
        SampleType delayIndex;
        juce::dsp::Oscillator<float> lfo;
    } ChannelState;
    
    bool isPrepared { false };
    double lastSampleRate;
    
    std::vector<ChannelState> channelStates;
    
    juce::AudioBuffer<float> delayBuffer;
    SampleType centerDelayLength;
    juce::SmoothedValue<SampleType> delayLength;
    int maxDelayinSamples;
    
    SampleType interpolateSample(ChannelState* channelState);
    
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> dryGain, wetGain;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> feedback { DEFAULT_FEEDBACK };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> rate { 0.01f };
    
    float mix = DEFAULT_MIX;
    SampleType depth = DEFAULT_DEPTH; // in ms
    
    SampleType readFromBuffer(ChannelState* channelState);
    void writeToBuffer(ChannelState* channelState, SampleType input);
    
    //-----------------------------------------------------------------------------
    // Utility
    //-----------------------------------------------------------------------------
    SampleType convertMStoSample(const SampleType time);
    SampleType lerp(SampleType a, SampleType b, SampleType f);
    SampleType limitDelayLength(SampleType delayLength);
    SampleType limitOutput(SampleType value);
};


