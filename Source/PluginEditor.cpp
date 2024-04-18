/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "XyPad.h"
#include "ConfigurableSlider.h"
#include <random>

#define TEXT_HEIGHT 20
#define INDENT 10
#define SLIDERSIZE 200
#define WIDTH SLIDERSIZE*3 +INDENT*5
#define HEIGHT TEXT_HEIGHT*11 + INDENT + SLIDERSIZE

//==============================================================================
ExploreAudioProcessorEditor::ExploreAudioProcessorEditor (ExploreAudioProcessor& p,juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(vts) 
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
   
    
    addAndMakeVisible(xyPad);
    
    
    boringButton.setButtonText("Switch");
    boringButton.onClick = [this] { xyPad.boringButtonClicked(); }; // Call XyPad's boringButtonClicked function
    addAndMakeVisible(boringButton);
    
    randomButton.setButtonText("Random");
    randomButton.onClick = [this] {
        assignSlidersToAxesRandomly();
        updateSliderPositions();
        };
       
    
    addAndMakeVisible(randomButton);
    
    
   

    
    addAndMakeVisible(timeSlider);
    timeSlider.setTextValueSuffix(" ms");
    timeAttachment.reset(new SliderAttachment(valueTreeState, "time", timeSlider));
    
    
    
    addAndMakeVisible(feedbackSlider);
    feedbackAttachment.reset(new SliderAttachment(valueTreeState, "feedback", feedbackSlider));
    
    addAndMakeVisible(mixSlider);
    mixAttachment.reset(new SliderAttachment(valueTreeState, "mix", mixSlider));
    
    addAndMakeVisible(spreadSlider);
    spreadAttachment.reset(new SliderAttachment(valueTreeState, "spread", spreadSlider));
    
//    addAndMakeVisible(cutOffSlider);
//    spreadSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
//    cutOffSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow ,false, 80, TEXT_HEIGHT);
//    cutOffSlider.setNormalisableRange(juce::NormalisableRange<double>(20.0f, 20000.0f));
//    cutOffAttachment.reset(new SliderAttachment(valueTreeState, "cutoff", cutOffSlider));
//    
    allSliders.push_back(&timeSlider);
    allSliders.push_back(&feedbackSlider); 
    allSliders.push_back(&mixSlider);
    allSliders.push_back(&spreadSlider);
    
    assignSlidersToAxesRandomly();

    setSize (WIDTH, HEIGHT);
    setResizable(true, true);
}

ExploreAudioProcessorEditor::~ExploreAudioProcessorEditor()
{
}

//==============================================================================
void ExploreAudioProcessorEditor::paint (juce::Graphics& g)
{
    
    //change background color
    g.fillAll (juce::Colours::darkgrey);

    
    //change text color, size and centre it
    g.setColour (juce::Colours::whitesmoke);
    g.setFont (35.0f);
    //g.drawFittedText ("Stereo Delay", getLocalBounds(), juce::Justification::centredTop, 1);
}
void ExploreAudioProcessorEditor::assignSlidersToAxesRandomly()
{
    std::random_device rd;
    std::mt19937 g(rd());

    // Shuffle allSliders vector randomly
    std::shuffle(allSliders.begin(), allSliders.end(), g);

    // Assign sliders to random axes
    for (auto* slider : allSliders) {
        Gui::XyPad::Axis axis;
        
        // Generate a random integer between 0 and 3 to determine the axis
        std::uniform_int_distribution<int> distribution(0, 3);
        int randomAxis = distribution(g);
        
        // Convert the random integer to an axis enum value
        switch (randomAxis) {
            case 0:
                axis = Gui::XyPad::Axis::X;
                break;
            case 1:
                axis = Gui::XyPad::Axis::Y;
                break;
            case 2:
                axis = Gui::XyPad::Axis::DiagonalTopLeftToBottomRight;
                break;
            case 3:
                axis = Gui::XyPad::Axis::DiagonalTopRightToBottomLeft;
                break;
            default:
                axis = Gui::XyPad::Axis::X; // Default to X axis if randomAxis is out of range
                break;
        }
        
        // Deregister and register the slider with the randomly chosen axis
        xyPad.deregisterSlider(slider);
        xyPad.registerSlider(slider, axis);

        // Ensure the slider is of type ConfigurableSlider
        if (auto* configurableSlider = dynamic_cast<Gui::ConfigurableSlider*>(slider)) {
            // Configure the slider color based on the assigned axis
            switch (axis) {
                case Gui::XyPad::Axis::X:
                    configurableSlider->setSliderColour(juce::Colours::lightseagreen);
                    break;
                case Gui::XyPad::Axis::Y:
                    configurableSlider->setSliderColour(juce::Colours::lightgoldenrodyellow);
                    break;
                case Gui::XyPad::Axis::DiagonalTopLeftToBottomRight:
                    configurableSlider->setSliderColour(juce::Colours::lightpink);
                    break;
                case Gui::XyPad::Axis::DiagonalTopRightToBottomLeft:
                    configurableSlider->setSliderColour(juce::Colours::lightcyan);
                    break;
                // Add cases for any additional axes
                default:
                    break;
            }
        }
        
    }

    // Repaint the xyPad to reflect the changes
    xyPad.repaint();
}
void ExploreAudioProcessorEditor::updateSliderPositions()
{
    const auto container = getLocalBounds();
    auto bounds = container;

    // Set bounds for xyPad on the left side
    xyPad.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.5)));

    // Calculate the height for each slider
    int sliderHeight = (container.getHeight() - 50) / 4; // Divide the height evenly among the four sliders

    // Set bounds for each slider on the right side
    for (auto* slider : allSliders) {
        slider->setBounds(bounds.removeFromTop(sliderHeight).reduced(5));
    }

    // Check if there's enough space for both buttons
    if (bounds.getWidth() > 210) // Ensure there's at least enough space for the buttons' width
    {
        // Calculate button dimensions and positions
        int buttonWidth = 100; // Set the width of the button
        int buttonHeight = 30; // Set the height of the button
        int buttonMargin = 10; // Margin between buttons
        
        // Position for the switchButton
        int switchButtonX = bounds.getX() + (bounds.getWidth() - 2 * buttonWidth - buttonMargin) / 2;
        int switchButtonY = bounds.getY() + 10; // Set the buttons below the sliders with a margin of 10 pixels
        
        // Position for the randomButton
        int randomButtonX = switchButtonX + buttonWidth + buttonMargin;
        int randomButtonY = switchButtonY;

        // Set the bounds for the switchButton using the calculated position and size
        boringButton.setBounds(switchButtonX, switchButtonY, buttonWidth, buttonHeight);

        // Set the bounds for the randomButton using the calculated position and size
        randomButton.setBounds(randomButtonX, randomButtonY, buttonWidth, buttonHeight);
    }
    else
    {
        // Hide both buttons if there's not enough space
        boringButton.setBounds(0, 0, 0, 0);
        randomButton.setBounds(0, 0, 0, 0);
    }

    // Ensure that the bounds of the XyPad component are updated correctly
    xyPad.repaint();
}




void ExploreAudioProcessorEditor::resized()
{
    updateSliderPositions();
}

