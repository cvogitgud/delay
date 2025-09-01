/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ProcrastinatorAudioProcessor::ProcrastinatorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), treeState(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
    treeState.addParameterListener("DELAYTIME", this);
    treeState.addParameterListener("MIX", this);
    treeState.addParameterListener("FEEDBACK", this);
    treeState.addParameterListener("RATE", this);
    treeState.addParameterListener("DEPTH", this);
    treeState.addParameterListener("POWER", this);
}

ProcrastinatorAudioProcessor::~ProcrastinatorAudioProcessor()
{
    treeState.removeParameterListener("DELAYTIME", this);
    treeState.removeParameterListener("MIX", this);
    treeState.removeParameterListener("FEEDBACK", this);
    treeState.removeParameterListener("RATE", this);
    treeState.removeParameterListener("DEPTH", this);
    treeState.removeParameterListener("POWER", this);
}

//==============================================================================
const juce::String ProcrastinatorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ProcrastinatorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ProcrastinatorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ProcrastinatorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ProcrastinatorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ProcrastinatorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ProcrastinatorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ProcrastinatorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ProcrastinatorAudioProcessor::getProgramName (int index)
{
    return {};
}

void ProcrastinatorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ProcrastinatorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    auto numInputChannels = getTotalNumInputChannels();
    lastSampleRate = sampleRate;
    
    delayLine.prepareToPlay(sampleRate, samplesPerBlock, numInputChannels);
    updateParameters();
}

void ProcrastinatorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ProcrastinatorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ProcrastinatorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel){
        
        auto* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample){
            if (isOn){
                channelData[sample] = delayLine.processSample(channel, channelData[sample]);
            }
        }
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout ProcrastinatorAudioProcessor::createParameterLayout(){
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    auto delayTime_ms = std::make_unique<juce::AudioParameterInt>(juce::ParameterID("DELAYTIME", 1), "Delay", 0, 1000, 500);
    auto mix = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("MIX", 1), "Mix", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f);
    auto feedback = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("FEEDBACK", 1), "Feedback", juce::NormalisableRange<float>(0.0f, 0.95f), 0.0f);
    
    auto rate = std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("RATE", 1), "Rate", juce::NormalisableRange<float>(0.0f, 10.0f), 0.0f);
    auto depth = std::make_unique<juce::AudioParameterInt>(juce::ParameterID("DEPTH", 1), "Depth", 0, 10, 0);
    
    auto power = std::make_unique<juce::AudioParameterBool>(juce::ParameterID("POWER", 1), "Power", true);
    
    params.push_back(std::move(delayTime_ms));
    params.push_back(std::move(mix));
    params.push_back(std::move(feedback));
    params.push_back(std::move(rate));
    params.push_back(std::move(depth));
    params.push_back(std::move(power));
    
    return {params.begin(), params.end()};
}

void ProcrastinatorAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue){
    // should use newValue instead of getting from tree?
    updateParameters();
}

void ProcrastinatorAudioProcessor::updateParameters(){
    updateDelayLength();
    updateMix();
    updateFeedback();
    updateRate();
    updateDepth();
    updatePower();
}

void ProcrastinatorAudioProcessor::updateDelayLength(){
    int delayTime_ms = treeState.getRawParameterValue("DELAYTIME")->load();
    delayLine.setDelayLength(delayTime_ms);
}

void ProcrastinatorAudioProcessor::updateMix(){
    float mix = treeState.getRawParameterValue("MIX")->load();
    delayLine.setMix(mix);
}

void ProcrastinatorAudioProcessor::updateFeedback(){
    float feedback = treeState.getRawParameterValue("FEEDBACK")->load();
    delayLine.setFeedback(feedback);
}

void ProcrastinatorAudioProcessor::updateRate(){
    float rate = treeState.getRawParameterValue("RATE")->load();
    delayLine.setRate(rate);
}

void ProcrastinatorAudioProcessor::updateDepth(){
    int depth = treeState.getRawParameterValue("DEPTH")->load();
    delayLine.setDepth(depth);
}

void ProcrastinatorAudioProcessor::updatePower(){
    isOn = treeState.getRawParameterValue("POWER")->load();
}

//==============================================================================
bool ProcrastinatorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ProcrastinatorAudioProcessor::createEditor()
{
    return new ProcrastinatorAudioProcessorEditor (*this);
//    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void ProcrastinatorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ProcrastinatorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ProcrastinatorAudioProcessor();
}
