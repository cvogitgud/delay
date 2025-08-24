/*
  ==============================================================================

    Delay.h
    Created: 21 Aug 2025 3:35:32pm
    Author:  Chris

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

typedef struct {
    int channel;
    int delayIndex; // index within delayBuffer
} ChannelState;

class Delay {
public:
    Delay();
    void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);
    void process(juce::AudioBuffer<float>& buffer);
    float updateDelayBuffer(float input, int channel);
    
    void setDelayLength(const int delayTime_ms);
    void setMix(const float mix);
    void setFeedback(const float feedback);
    void setRate(const int rate);
    void setDepth(const int depth);
    
    void updateParameters();
    
    void reset();
    
private:
    double lastSampleRate = 44100.0f;
    
    std::vector<ChannelState> channelStates;
    
    juce::AudioBuffer<float> delayBuffer;
    int centerDelayLength;
    int maxDelayLength;
    
    float mix = 0.5f;
    float feedback = 0.5f;
    
    juce::dsp::Oscillator<float> lfo;
    float rate = 0.0f;
    float depth = 0.0f; // in ms
    
    int convertMStoSample(const int time);
    float limitOutput(float value);
};
