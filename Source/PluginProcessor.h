/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CircularBuffer.h"
#include "LowpassHighpassFilter.h"

//==============================================================================
/**
*/
class ExploreAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ExploreAudioProcessor();
    ~ExploreAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    
    CircularBuffer delayBuffer[2];
    juce::AudioProcessorValueTreeState params;
    juce::LinearSmoothedValue<float> delayTime {0};
    
    LowpassHighpassFilter lowpassFilter;

    //set bpm in case it couldn't be retrieved from DAW
    float bpm = 120.0f;
    
    
    std::atomic<float>* delayMsParameter = nullptr;
    std::atomic<float>* feedbackParameter = nullptr;
    std::atomic<float>* spreadParameter = nullptr;
    std::atomic<float>* invertParameter = nullptr;
    std::atomic<float>* mixParameter = nullptr;
    std::atomic<float>* syncParameter = nullptr;
    std::atomic<float>* lowpassParameter = nullptr;
    std::atomic<float>* timeParameter = nullptr;
    
    std::atomic<float>* cutOffParameter = nullptr;
    
    std::atomic<float>* delayModeParameter;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExploreAudioProcessor)
};
