/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ConfigurableSlider.h"
#include "XyPad.h"

//==============================================================================
/**
*/
class ExploreAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ExploreAudioProcessorEditor (ExploreAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~ExploreAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void assignSlidersToAxesRandomly();
    void resized() override;
    void updateSliderPositions();
     
    
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ExploreAudioProcessor& audioProcessor;
    
    Gui::XyPad xyPad;
    
    juce::TextButton boringButton;
    juce::TextButton randomButton;
    
    std::vector<Slider*> allSliders;
    
    
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    //labels
    juce::Label timeLabel;
    juce::Label spreadLabel;
    juce::Label feedbackLabel;
    juce::Label mixLabel;
    juce::Label invertLabel;
    juce::Label syncLabel;
 
    
    //controllers
    Gui::ConfigurableSlider timeSlider {*this, xyPad};
    Gui::ConfigurableSlider spreadSlider {*this, xyPad};
    Gui::ConfigurableSlider feedbackSlider {*this, xyPad};
    Gui::ConfigurableSlider mixSlider {*this, xyPad};
    Gui::ConfigurableSlider cutOffSlider {*this, xyPad};

    
    juce::ToggleButton invertSideButton;
    juce::ToggleButton syncButton;
    
    juce::ComboBox syncedDelayBox;
    juce::ComboBox delayModeButton;
    
    //attachments
    std::unique_ptr<SliderAttachment> timeAttachment;
    std::unique_ptr<SliderAttachment> spreadAttachment;
    std::unique_ptr<SliderAttachment> feedbackAttachment;
    std::unique_ptr<SliderAttachment> mixAttachment;
    
    std::unique_ptr<SliderAttachment> cutOffAttachment;
    
    
    std::unique_ptr<ButtonAttachment> invertSideAttachment;
    std::unique_ptr<ButtonAttachment> syncAttachment;
   
    
    std::unique_ptr<ComboBoxAttachment> delayModeAttachment;
    std::unique_ptr<ComboBoxAttachment> syncedDelayAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExploreAudioProcessorEditor)
};
