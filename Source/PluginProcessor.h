/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/**
*/
class MidiDebuggerAudioProcessor  : public AudioProcessor, public AsyncUpdater, public Button::Listener
{
public:
    //==============================================================================
    MidiDebuggerAudioProcessor();
    ~MidiDebuggerAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiDebuggerAudioProcessor)
    Array<MidiMessage> messageLog;
    
    AudioPlayHead::CurrentPositionInfo info; // The last updated position
    double sampleRate;                       // samples per sec
    int blockSize;                           // samples per block

    int currentSample = 0;
    int currentBeat = 0;
    bool beatFirstHalf = true;
    bool metronomeOn = false;
    bool resetting = false;
    
    void handleAsyncUpdate() override;
    void buttonClicked(Button*) override;
    void doReset(MidiBuffer&);
};
