/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "LowpassHighpassFilter.h"


//==============================================================================
ExploreAudioProcessor::ExploreAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
    params(*this, nullptr, "Parameters",
       {
        std::make_unique<juce::AudioParameterInt>(ParameterID("time",1), "Time", 0, 2000, 200),
        std::make_unique<juce::AudioParameterInt>(ParameterID("feedback",1), "Feedback", 0, 100, 50),
        std::make_unique<juce::AudioParameterInt>(ParameterID("spread",1), "Intensity", 0, 100, 50),
        std::make_unique<juce::AudioParameterInt>(ParameterID("mix",1), "Mix", 0, 100, 50),
        std::make_unique<juce::AudioParameterBool>(ParameterID("invert",1), "Invert Side", false),
        std::make_unique<juce::AudioParameterChoice>(ParameterID("delayMode",1), "Delay Mode", juce::StringArray{"normal","pingpong"}, 1),
        std::make_unique<juce::AudioParameterBool>(ParameterID("sync",1), "Sync", false),
        std::make_unique<juce::AudioParameterBool>(ParameterID("lowpass",1), "Lowpass", false),
        std::make_unique<juce::AudioParameterChoice>(ParameterID("timing",1), "Delay Time", juce::StringArray{"1/4", "1/8", "1/16", "1/32"}, 0),
        // Add the new parameter for cutoff frequency
        std::make_unique<juce::AudioParameterFloat>(ParameterID("cutoff",1), "Cutoff", juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.25f), 2000.0f),

})
{
delayMsParameter = params.getRawParameterValue("time");
feedbackParameter = params.getRawParameterValue("feedback");
spreadParameter = params.getRawParameterValue("spread");
invertParameter = params.getRawParameterValue("invert");
mixParameter = params.getRawParameterValue("mix");
delayModeParameter = params.getRawParameterValue("delayMode");
syncParameter = params.getRawParameterValue("sync");
lowpassParameter = params.getRawParameterValue("lowpass");
timeParameter = params.getRawParameterValue("timing");
cutOffParameter = params.getRawParameterValue("cutoff");

}

ExploreAudioProcessor::~ExploreAudioProcessor()
{
}

//==============================================================================
const juce::String ExploreAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ExploreAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ExploreAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ExploreAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ExploreAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ExploreAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ExploreAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ExploreAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ExploreAudioProcessor::getProgramName (int index)
{
    return {};
}

void ExploreAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ExploreAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    for (int i = 0; i < 2; i++) {
        delayBuffer[i].init(sampleRate * 2 +1);
    }
    delayTime.reset(sampleRate, 0.3f);
}

void ExploreAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ExploreAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void ExploreAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    auto sampleRate = getSampleRate();
    auto numSamples = buffer.getNumSamples();
    
    auto tmp_bpm = getPlayHead()->getPosition()->getBpm();
        
       
        if (tmp_bpm.hasValue()) {
            //update bpm with retrieved value
            bpm = *tmp_bpm;
//            DBG("Got BPM: " << bpm);
        }
        else {
            // show that bpm couldn't be retrieved
            DBG("BPM couldn't be retrieved"<<bpm);
        };
    double quarterNoteDelay = 60000.0/bpm;
   // double eighthNoteDelay = quarterNoteDelay/2.0;
   // double sixteenthNoteDelay = quarterNoteDelay/2.0;
    //double thirtySecondNoteDelay = sixteenthNoteDelay/2.0;
    
   
    
//    DBG(*syncParameter);
//    DBG(*timeParameter);
    
//    float delay = *delayMsParameter;
    
  
 
    
//    DBG(delay);
    
    auto time = ( !*syncParameter ) ? static_cast<float>(*delayMsParameter) : quarterNoteDelay * std::pow(2.0, -1* *timeParameter );
    delayTime.setTargetValue(time);
//    DBG(delayTime.getNextValue());
    
    
    /*auto time = delay;
    if (*syncParameter == 1){
        if(*timeParameter == 0){
            time = quarterNoteDelay;
        }else if (*timeParameter == 1){
            time = eighthNoteDelay;
        }else if (*timeParameter == 2){
            time = sixteenthNoteDelay;
        }else if (*timeParameter == 3){
            time = thirtySecondNoteDelay;
        }
        
    }*/

//    DBG(time);
   
    
    auto wet = sqrt( *mixParameter / 100.0 );
    auto dry = sqrt( 1.0 - (*mixParameter / 100.0) );
    auto fb = static_cast<float>(*feedbackParameter);
    
    

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        

        // ..do something to the data...
    };
    auto* leftChannelData = buffer.getWritePointer(0);
    auto* rightChannelData = buffer.getWritePointer(1);
    
    auto invertSide = *invertParameter > 0.5f;
    auto spreadLeft = invertSide ? (1 - (*spreadParameter / 100)) : 1;
    auto spreadRight = invertSide ? 1 : (1 - (*spreadParameter / 100));
    
  
    for (int i = 0; i < numSamples; ++i) {
        // Update and fetch the current smoothed delay time in samples
        // Note: This assumes 'time' is the delay time in milliseconds you wish to smooth transition to
        float currentSmoothedDelayTimeInSamples = delayTime.getNextValue() * sampleRate / 1000.0f;
        
        // read input signal
        const float inputL = leftChannelData[i];
        const float inputR = rightChannelData[i];
        
        
        // read delayed signal
        float delayL = delayBuffer[0].read(currentSmoothedDelayTimeInSamples);
        float delayR = delayBuffer[1].read(currentSmoothedDelayTimeInSamples);

        // create input for delay line + write feedback
       
        //mode = normal
        if(*delayModeParameter == 0){
            
            double dnL = inputL  + (fb/ 100) * delayL;
            double dnR = inputR  + (fb / 100) * delayR;
            
            delayBuffer[0].write(dnL);
            delayBuffer[1].write(dnR);
        
        //mode = pingpong
        }else if(*delayModeParameter == 1){
            
            double dnL = inputL * spreadLeft  + (fb / 100) * delayL;
            double dnR = inputR * spreadRight + (fb / 100) * delayR;
            
            
            delayBuffer[0].write(dnR);
            delayBuffer[1].write(dnL);
        }

        // write output mix
        leftChannelData[i] = dry * inputL + wet * delayL;
        rightChannelData[i] = dry * inputR + wet * delayR;

    }
    if(*lowpassParameter)
    {
        // Apply lowpass filter
        lowpassFilter.setHighpass(false); // Set to lowpass mode
        lowpassFilter.setCutoffFrequency(*cutOffParameter); // Get cutoff frequency from parameter
        lowpassFilter.setSamplingRate(getSampleRate());
        lowpassFilter.processBlock(buffer, midiMessages);
    }
}

//==============================================================================
bool ExploreAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ExploreAudioProcessor::createEditor()
{
    return new ExploreAudioProcessorEditor (*this, params);
   
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void ExploreAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = params.copyState();
            std::unique_ptr<juce::XmlElement> xml (state.createXml());
            copyXmlToBinary (*xml, destData);
}

void ExploreAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
     
        if (xmlState.get() != nullptr)
            if (xmlState->hasTagName (params.state.getType()))
                    params.replaceState (juce::ValueTree::fromXml (*xmlState));
        
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ExploreAudioProcessor();
}
