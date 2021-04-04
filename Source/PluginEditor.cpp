/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/


#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
bittyAudioProcessorEditor::bittyAudioProcessorEditor (bittyAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    setResizable(true, true);
    setResizeLimits(400, 300, 4000, 3000);



    String bitremaptext;


    for (int i = 0; i < N_BITS; ++i)
    {
        int bitremapfori = static_cast<int>(audioProcessor.ed.getbitremap()[i]);
        if (bitremapfori < 9)
        {
            bitremaptext += bitremapfori;
        }
        else
        {
            bitremaptext += String::toHexString(bitremapfori);
        }
    }

    xorMaskEditor.setText(audioProcessor.ed.getxormask());
    orMaskEditor.setText(audioProcessor.ed.getormask());
    andMaskEditor.setText(audioProcessor.ed.getandmask());
    bitRemapEditor.setText(bitremaptext);

#if N_BITS == 16
    xorMaskEditor.setTextToShowWhenEmpty("0000000000000000", juce::Colours::grey);
    orMaskEditor.setTextToShowWhenEmpty("0000000000000000", juce::Colours::grey);
    andMaskEditor.setTextToShowWhenEmpty("1111111111111111", juce::Colours::grey);
    bitRemapEditor.setTextToShowWhenEmpty("0123456789ABCDEF", juce::Colours::grey);
#endif
#if N_BITS == 8
    xorMaskEditor.setTextToShowWhenEmpty("00000000", juce::Colours::grey);
    orMaskEditor.setTextToShowWhenEmpty("00000000", juce::Colours::grey);
    andMaskEditor.setTextToShowWhenEmpty("11111111", juce::Colours::grey);
    bitRemapEditor.setTextToShowWhenEmpty("01234567", juce::Colours::grey);
#endif
#if N_BITS != 8 && N_BITS != 16
    jassertfalse
#endif


    for (TextEditor* a : editors)
    {
        a->addListener(this);
        a->setMultiLine(false);
        TextEditor::InputFilter* infilt = new TextEditor::LengthAndCharacterRestriction(N_BITS, "01");
        a->setInputFilter(infilt, true);
        infilt = nullptr;
        addAndMakeVisible(a);
    }

#if N_BITS == 16
    TextEditor::InputFilter* infilt = new TextEditor::LengthAndCharacterRestriction(N_BITS, "0123456789ABCDEFabcdef");
#endif
#if N_BITS == 8
    TextEditor::InputFilter* infilt = new TextEditor::LengthAndCharacterRestriction(N_BITS, "01234567");
#endif
    bitRemapEditor.setInputFilter(infilt, true);
    infilt = nullptr;


    xorLabel.setText("xor mask", dontSendNotification);
    andLabel.setText("and mask", dontSendNotification);
    orLabel.setText("or mask", dontSendNotification);
    entropySliderLabel.setText("entropy", dontSendNotification);
    bitremapLabel.setText("bit remapping", dontSendNotification);

    xorLabel.attachToComponent(&xorMaskEditor, true);
    andLabel.attachToComponent(&andMaskEditor, true);
    orLabel.attachToComponent(&orMaskEditor, true);
    entropySliderLabel.attachToComponent(&entropySlider, true);
//    bitremapLabel.attachToComponent(&bitRemapEditor, true);



    entropySlider.setRange({0.0, 1.0}, 0.0000001);
    entropySlider.addListener(this);
    entropySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    entropySlider.setTextBoxIsEditable(true);
    entropySlider.setDoubleClickReturnValue(true, 0.0);
    entropySlider.setValue(0.0);

    entropyAmtSlider.setRange({-10.f, 10.f}, 0.0001);
    entropyAmtSlider.addListener(this);
    entropyAmtSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    entropyAmtSlider.setTextBoxIsEditable(true);
    entropyAmtSlider.setDoubleClickReturnValue(true, 1.0);
    entropyAmtSlider.setValue(1.0);

    addAndMakeVisible(entropyAmtSlider);
    addAndMakeVisible(entropySlider);

    for(Label* a : labels)
    {
        addAndMakeVisible(a);
    }

    _p = &p;
}

bittyAudioProcessorEditor::~bittyAudioProcessorEditor()
{
}


void bittyAudioProcessorEditor::textEditorReturnKeyPressed(TextEditor& t)
{
    String s = t.getText();
    if (&t == &xorMaskEditor)
    {
        s = s.paddedRight('0', N_BITS);
        _p->ed.setxormask(s);
    }
    else if (&t == &andMaskEditor)
    {
        s = s.paddedRight('1', N_BITS);
        _p->ed.setandmask(s);
    }
    else if (&t == &orMaskEditor)
    {
        s = s.paddedRight('0', N_BITS);
        _p->ed.setormask(s);
    }
    else if (&t == &bitRemapEditor)
    {
        if (s.length() != N_BITS)
        {
#if N_BITS == 16
            s.append(String("0123456789ABCDEF").substring(s.length()), N_BITS);
#endif
#if N_BITS == 8
            s.append(String("01234567").substring(s.length()), N_BITS);
#endif
            
        }

        std::array<uint8, N_BITS> arr;

        for (int i = 0; i < N_BITS; ++i)
        {
            if (s.substring(0, 1).contains("0123456789"))
            {
                arr[i] = s.substring(0, 1).getIntValue();
            }
            else
            {
                arr[i] = s.substring(0, 1).getHexValue32();
            }

            s = s.substring(1);
        }

        _p->ed.setEntireBitRemap(arr);
    }



}

char bittyAudioProcessorEditor::parseMaskString(String s)
{
    char a = 0;
    for (int i = 0; i < N_BITS; ++i)
    {
        a += (s.substring(0,1).getIntValue()) * (1 << i);
        s = s.substring(1);
    }
    return a;
}

//==============================================================================
void bittyAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
//    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void bittyAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    Rectangle<int> thesebounds = getLocalBounds().reduced(10, 10);
    Rectangle<int> remaparea = thesebounds.removeFromTop(thesebounds.proportionOfHeight(0.33)).reduced(5, 5);
    Rectangle<int> maskarea = thesebounds.reduced(5,5);

    bitremapLabel.setBounds(remaparea.removeFromTop(20));
    remaparea.removeFromTop(10);
    bitRemapEditor.setBounds(remaparea);

    int areaper = maskarea.proportionOfHeight(0.25);
    andMaskEditor.setBounds(maskarea.removeFromTop(areaper).reduced(0, 10).removeFromRight(300));
    orMaskEditor.setBounds(maskarea.removeFromTop(areaper).reduced(0, 10).removeFromRight(300));
    xorMaskEditor.setBounds(maskarea.removeFromTop(areaper).reduced(0, 10).removeFromRight(300));

    auto a = maskarea.removeFromTop(areaper).reduced(0, 10);
    entropySlider.setBounds(a.removeFromRight(150));
    entropyAmtSlider.setBounds(a);


}


void bittyAudioProcessorEditor::sliderValueChanged(Slider *s)
{
    if (s == &entropySlider)
    {
        _p->ed.setEntropyVal(entropySlider.getValue());
    }
    if (s == &entropyAmtSlider)
    {
        _p->ed.setEntropyAmt(entropyAmtSlider.getValue());
    }
}
