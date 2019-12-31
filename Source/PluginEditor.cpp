/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MidiDebuggerAudioProcessorEditor::MidiDebuggerAudioProcessorEditor(MidiDebuggerAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{

    addAndMakeVisible(metronomeLabel);
    metronomeLabel.setText("metronome", dontSendNotification);
    addAndMakeVisible(metronomeButton);
    addAndMakeVisible(resetButton);
    resetButton.setButtonText("reset");

    addAndMakeVisible(midiMessagesBox);
    midiMessagesBox.setMultiLine(true);
    midiMessagesBox.setReturnKeyStartsNewLine(true);
    midiMessagesBox.setReadOnly(true);
    midiMessagesBox.setScrollbarsShown(true);
    midiMessagesBox.setCaretVisible(false);
    midiMessagesBox.setPopupMenuEnabled(true);
    midiMessagesBox.setColour(TextEditor::backgroundColourId, Colour(0x32ffffff));
    midiMessagesBox.setColour(TextEditor::outlineColourId, Colour(0x1c000000));
    midiMessagesBox.setColour(TextEditor::shadowColourId, Colour(0x16000000));
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 300);
}

MidiDebuggerAudioProcessorEditor::~MidiDebuggerAudioProcessorEditor()
{
}

//==============================================================================
void MidiDebuggerAudioProcessorEditor::paint(Graphics& g) {
    
    String bpm = String::formatted("BPM: %.2f", curBPM);
    String beat = String::formatted("beat: %d", curBeat);
    String time = String::formatted("time: %.2f", timeElapsed);
    String sample = String::formatted("sample: %d", samplesElapsed);

    g.setColour (Colours::white);
    g.setFont (15.0f);

    g.drawText (bpm,     15, 10, 200, 20, Justification::left, true);
    g.drawText (beat,   210, 10, 200, 20, Justification::left, true);
    g.drawText (time,    15, 30, 200, 20, Justification::left, true);
    g.drawText (sample, 210, 30, 200, 20, Justification::left, true);
}

void MidiDebuggerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    auto quarterHeight = getHeight() / 4;
    auto halfWidth = getWidth() / 2;

    metronomeLabel.setBounds(
        getLocalBounds().withX(35)
                        .withY(quarterHeight-20)
                        .withHeight(20));

    metronomeButton.setBounds(
        getLocalBounds().withX(10)
                        .withY(quarterHeight-20)
                        .withHeight(20));

    resetButton.setBounds(
        getLocalBounds().withX(halfWidth+5)
                        .withWidth(halfWidth-15)
                        .withY(quarterHeight-20)
                        .withHeight(20));
    
    midiMessagesBox.setBounds(
        getLocalBounds().withHeight(getHeight()-quarterHeight)
                        .withY(quarterHeight)
                        .reduced(10));
}

void MidiDebuggerAudioProcessorEditor::logMidiMessage (const MidiMessage& message)
{
    auto time = message.getTimeStamp();
    
    auto hours   = ((int) (time / 3600.0)) % 24;
    auto minutes = ((int) (time / 60.0)) % 60;
    auto seconds = ((int) time) % 60;
    auto millis  = ((int) (time * 1000.0)) % 1000;
    
    auto timecode = String::formatted("%02d:%02d:%02d.%03d",
                                      hours,
                                      minutes,
                                      seconds,
                                      millis);
    
    writeLog (timecode + "  -  " + getMidiMessageDescription(message));
}

void MidiDebuggerAudioProcessorEditor::setInfo(double bpm, int beat, double time, long long samples) {
    curBPM = bpm;
    curBeat = beat;
    timeElapsed = time;
    samplesElapsed = samples;
    this->repaint();
}

void MidiDebuggerAudioProcessorEditor::addListener(Button::Listener *listener) {
    resetButton.addListener(listener);
    metronomeButton.addListener(listener);
}


//==============================================================================

String MidiDebuggerAudioProcessorEditor::getMidiMessageDescription(const MidiMessage& m)
{
    if (m.isNoteOn())           return "Note on "          + MidiMessage::getMidiNoteName (m.getNoteNumber(), true, true, 3);
    if (m.isNoteOff())          return "Note off "         + MidiMessage::getMidiNoteName (m.getNoteNumber(), true, true, 3);
    if (m.isProgramChange())    return "Program change "   + String (m.getProgramChangeNumber());
    if (m.isPitchWheel())       return "Pitch wheel "      + String (m.getPitchWheelValue());
    if (m.isAftertouch())       return "After touch "      + MidiMessage::getMidiNoteName (m.getNoteNumber(), true, true, 3) +  ": " + String (m.getAfterTouchValue());
    if (m.isChannelPressure())  return "Channel pressure " + String (m.getChannelPressureValue());
    if (m.isAllNotesOff())      return "All notes off";
    if (m.isAllSoundOff())      return "All sound off";
    if (m.isMetaEvent())        return "Meta event";
    
    if (m.isController())
    {
        String name(MidiMessage::getControllerName(m.getControllerNumber()));
        
        if (name.isEmpty())
            name = "[" + String(m.getControllerNumber()) + "]";
        
        return "Controller " + name + ": " + String(m.getControllerValue());
    }
    
    return String::toHexString(m.getRawData(), m.getRawDataSize());
}

void MidiDebuggerAudioProcessorEditor::writeLog(const String& m)
{
    midiMessagesBox.moveCaretToEnd();
    midiMessagesBox.insertTextAtCaret(m + newLine);
}
