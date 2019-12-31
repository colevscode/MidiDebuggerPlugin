/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class MidiDebuggerAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    MidiDebuggerAudioProcessorEditor (MidiDebuggerAudioProcessor&);
    ~MidiDebuggerAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    void logMidiMessage(const MidiMessage &);
    void setInfo(double bpm, int beat, double time, long long samples);
    void addListener(Button::Listener*);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MidiDebuggerAudioProcessor& processor;
    
    Label metronomeLabel;
    ToggleButton metronomeButton;
    TextButton resetButton;
    TextEditor midiMessagesBox;

    double curBPM = 0.0;
    int curBeat = 0;
    double timeElapsed = 0.0;
    long long samplesElapsed = 0;

    static String getMidiMessageDescription(const MidiMessage &);
    void writeLog(const String &);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MidiDebuggerAudioProcessorEditor)
};
