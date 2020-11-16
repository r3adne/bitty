/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <iostream>
//==============================================================================
bittyAudioProcessor::bittyAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{

}

bittyAudioProcessor::~bittyAudioProcessor()
{
}

//==============================================================================
const juce::String bittyAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool bittyAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool bittyAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool bittyAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double bittyAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int bittyAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int bittyAudioProcessor::getCurrentProgram()
{
    return 0;
}

void bittyAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String bittyAudioProcessor::getProgramName (int index)
{
    return {};
}

void bittyAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void bittyAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void bittyAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool bittyAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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

void bittyAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    ed.processSamplesContextReplacing(buffer);

}

//==============================================================================
bool bittyAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* bittyAudioProcessor::createEditor()
{
    return new bittyAudioProcessorEditor (*this);


}

//==============================================================================
void bittyAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.


    ValueTree vt("settings");


    vt.setProperty("version", "0.0.0", nullptr);
    vt.setProperty("license", "GNU Affero General Public License v3.0", nullptr);

    vt.setProperty("xormask", ed.getxormask(), nullptr);
    vt.setProperty("ormask", ed.getormask(), nullptr);
    vt.setProperty("andmask", ed.getandmask(), nullptr);

    String remapvalsString = "";

    for (int i = 0; i < N_BITS; ++i)
    {
        remapvalsString.append(String(static_cast<char>(ed.getbitremap()[i])), 1);
    }

    vt.setProperty("remapvals", remapvalsString, nullptr);


    std::unique_ptr<XmlElement> a = vt.createXml();


    copyXmlToBinary(*a, destData);

}

void bittyAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.


    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));



    if (xml.get() == nullptr) return;

    if (! xml->hasTagName("settings")) return;

    ValueTree vt("settings");
    vt = ValueTree::fromXml(*xml);


    String test = vt.toXmlString();


    ed.setxormask(vt.getProperty("xormask"));
    ed.setormask(vt.getProperty("ormask"));
    ed.setandmask(vt.getProperty("andmask"));

    std::array<uint8, N_BITS> bits;

    String s = vt.getProperty("remapvals");

    for (int i = 0; i < N_BITS; ++i)
    {
        bits[i] = static_cast<uint8>(s.substring(i,i+1).getIntValue());
    }

    ed.setEntireBitRemap(bits);


}



//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new bittyAudioProcessor();
}
