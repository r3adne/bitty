/*
  ==============================================================================

    HysteresisProcessor.h
    Created: 5 Dec 2020 3:38:57pm
    Author:  Zachary Lewis-Towbes

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>


struct HysteresisProcessor : public juce::AudioProcessor
{
    HysteresisProcessor();
    HysteresisProcessor(HysteresisProcessor& other);
    HysteresisProcessor(HysteresisProcessor&& other);
    ~HysteresisProcessor() override;



    // resource management //
    void prepareToPlay(double, int) override;
    void releaseResources() override;

    // processing //
    void processBlock(AudioBuffer<float>&, MidiBuffer&) override;
    void processBlock(AudioBuffer<double>&, MidiBuffer&) override;
    void processBlockBypassed(AudioBuffer<float>&, MidiBuffer&) override;
    void processBlockBypassed(AudioBuffer<double>&, MidiBuffer&) override;
    bool supportsDoublePrecisionProcessing() const override;
    AudioProcessorParameter* getBypassParameter() const override;
    void reset() override;
    void setNonRealtime(bool) noexcept override;


    // buses //
    bool canAddBus(bool) const override;
    bool canRemoveBus(bool) const override;

    // attributes //
    double getTailLengthSeconds() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool supportsMPE() const override;
    bool isMidiEffect() const override;

    // editor //
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;



    // programs //
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int) override;
    const String getProgramName(int) override;
    void changeProgramName(int, const String&) override;

    // state //
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

protected:
    // buses //
    bool isBusesLayoutSupported(const BusesLayout&) const override;
    bool canApplyBusesLayout(const BusesLayout&) const override;
    bool applyBusLayouts(const BusesLayout&) override;
    bool canApplyBusCountChange(bool, bool, BusProperties&) override;









};
