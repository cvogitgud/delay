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
    int delayIndex; // index within delayBuffer
} ChannelState;

class Delay {
public:
    void prepareToPlay(double sampleRate, int samplesPerBlock, int numChannels);
    float processSample(int channel, float sample);
    
    void setDelayLength(const int delayTime_ms);
    void setMix(const float mix);
    void setFeedback(const float feedback);
    void setRate(const int rate);
    void setDepth(const int depth);
    
    void reset();
    
private:
    bool isPrepared {false};
    double lastSampleRate;
    
    std::vector<ChannelState> channelStates;
    
    juce::AudioBuffer<float> delayBuffer;
    int centerDelayLength;
    int maxDelayLength;
    
    float mix;
    float feedback;
    
    juce::dsp::Oscillator<float> lfo;
    int rate;
    int depth; // in ms
    
    int limitDelayLength(int delayLength);
    int convertMStoSample(const int time);
    float limitOutput(float value);
};
