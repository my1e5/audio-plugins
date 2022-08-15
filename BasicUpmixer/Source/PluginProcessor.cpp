/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BasicUpmixerAudioProcessor::BasicUpmixerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::createLCR(), true)
                     #endif
                       )
#endif
{
}

BasicUpmixerAudioProcessor::~BasicUpmixerAudioProcessor()
{
}

//==============================================================================
const juce::String BasicUpmixerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BasicUpmixerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BasicUpmixerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BasicUpmixerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BasicUpmixerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BasicUpmixerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BasicUpmixerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BasicUpmixerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BasicUpmixerAudioProcessor::getProgramName (int index)
{
    return {};
}

void BasicUpmixerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void BasicUpmixerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void BasicUpmixerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BasicUpmixerAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const juce::AudioChannelSet& mainInput = layouts.getMainInputChannelSet();
    const juce::AudioChannelSet& mainOutput = layouts.getMainOutputChannelSet();

    return mainInput.size() == 2 && mainOutput.size() == 3;
}
#endif

void BasicUpmixerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Find buffer length
    const int numSamples = buffer.getNumSamples();
    if (numSamples == 0) {
        return; // Exit if no samples
    }

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Initialise write pointers
    auto outputL = buffer.getWritePointer(0);
    auto outputR = buffer.getWritePointer(1);
    auto outputC = buffer.getWritePointer(2);

    // Main processing loop
    for (int n = 0; n < numSamples; ++n)
    {
        float inputL = buffer.getSample(0, n);
        float inputR = buffer.getSample(1, n);

        outputL[n] = inputL;
        outputR[n] = inputR;
        outputC[n] = 0.5f * (inputL + inputR);

    }
}

//==============================================================================
bool BasicUpmixerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BasicUpmixerAudioProcessor::createEditor()
{
    return new BasicUpmixerAudioProcessorEditor (*this);
}

//==============================================================================
void BasicUpmixerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void BasicUpmixerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BasicUpmixerAudioProcessor();
}
