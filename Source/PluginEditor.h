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
class bittyAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::TextEditor::Listener, public juce::Slider::Listener
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


    void sliderValueChanged (Slider *slider) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.

    bittyAudioProcessor* _p;


    ToggleButton removeDenormalsButton;


    Slider entropySlider;
    Slider entropyAmtSlider;

    TextEditor andMaskEditor;
    TextEditor orMaskEditor;
    TextEditor xorMaskEditor;
    TextEditor bitRemapEditor;

    std::array<TextEditor*, 4> editors = {&andMaskEditor, &orMaskEditor, &xorMaskEditor, &bitRemapEditor};

    Label andLabel, orLabel, xorLabel, bitremapLabel, removeDenormalsLabel, entropySliderLabel;

    std::array<Label*, 6> labels = {&andLabel, &orLabel, &xorLabel, &bitremapLabel, &removeDenormalsLabel, &entropySliderLabel};


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (bittyAudioProcessorEditor)
};
