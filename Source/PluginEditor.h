/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once


#include <array>

#include <JuceHeader.h>

#include "PluginProcessor.h"


//==============================================================================
/**
*/
class bittyAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::TextEditor::Listener
{

    bittyAudioProcessor& audioProcessor;
public:
    bittyAudioProcessorEditor (bittyAudioProcessor&);
    ~bittyAudioProcessorEditor() override;

    void textEditorReturnKeyPressed(TextEditor&) override;

    char parseMaskString(String s);
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.

    bittyAudioProcessor* _p;


    ToggleButton removeDenormalsButton;


    TextEditor andMaskEditor;
    TextEditor orMaskEditor;
    TextEditor xorMaskEditor;
    TextEditor bitRemapEditor;

    std::array<TextEditor*, 4> editors = {&andMaskEditor, &orMaskEditor, &xorMaskEditor, &bitRemapEditor};

    Label andLabel, orLabel, xorLabel, bitremapLabel, removeDenormalsLabel;

    std::array<Label*, 5> labels = {&andLabel, &orLabel, &xorLabel, &bitremapLabel, &removeDenormalsLabel};


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (bittyAudioProcessorEditor)
};
