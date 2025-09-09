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
#define DEFAULT_RATE 0.0
#define DEFAULT_DEPTH 0.0

typedef struct {
    int channel;
    float delayIndex;
    juce::dsp::Oscillator<float> lfo;
} ChannelState;

class Delay {
public:
    void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);
    float processSample(int channel, float sample);
    
    void setDelayLength(const int delayTime_ms);
    void setMix(const float mix);
    void setFeedback(const float feedback);
    void setRate(const float rate);
    void setDepth(const int depth);
    
    void clearDelayLine();
    
    void reset();
    
private:
    bool isPrepared { false };
    double lastSampleRate;
    
    std::vector<ChannelState> channelStates;
    
    juce::AudioBuffer<float> delayBuffer;
    int centerDelayLength;
    juce::SmoothedValue<float> delayLength;
    int maxDelayLength;
    
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> mix { DEFAULT_MIX };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> feedback { DEFAULT_FEEDBACK };
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> rate { 0.01f };
    int depth = DEFAULT_DEPTH; // in ms
    
    float readFromBuffer(ChannelState* channelState);
    void writeToBuffer(ChannelState* channelState, float input);
    
    //-----------------------------------------------------------------------------
    // Utility
    //-----------------------------------------------------------------------------
    float convertMStoSample(const float time);
    float lerp(float a, float b, float f);
    int limitDelayLength(int delayLength);
    float limitOutput(float value);
};


