/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MidiDebuggerAudioProcessor::MidiDebuggerAudioProcessor()
    #ifndef JucePlugin_PreferredChannelConfigurations
      : AudioProcessor (BusesProperties()
        #if ! JucePlugin_IsMidiEffect
          #if ! JucePlugin_IsSynth
            .withInput  ("Input",  AudioChannelSet::stereo(), true)
          #endif
          .withOutput ("Output", AudioChannelSet::stereo(), true)
        #endif
      )
    #endif
{
}

MidiDebuggerAudioProcessor::~MidiDebuggerAudioProcessor()
{
}

//==============================================================================
const String MidiDebuggerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MidiDebuggerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MidiDebuggerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MidiDebuggerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MidiDebuggerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MidiDebuggerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MidiDebuggerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MidiDebuggerAudioProcessor::setCurrentProgram (int index)
{
}

const String MidiDebuggerAudioProcessor::getProgramName (int index)
{
    return {};
}

void MidiDebuggerAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void MidiDebuggerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    this->sampleRate = sampleRate;
    this->blockSize = samplesPerBlock;
}

void MidiDebuggerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MidiDebuggerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void MidiDebuggerAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midi)
{
    bool update = false;
    MidiMessage msg;

    // Get incomming midi messages and log them

    int ignore;
    for (MidiBuffer::Iterator it (midi); it.getNextEvent (msg, ignore);)
    {
        messageLog.add(msg);
        update = true;
    }
    midi.clear();

    // Get current BPM and update metronome.
    
    AudioPlayHead *head = getPlayHead();
    if (head && !resetting) {
        head->getCurrentPosition(info);
        int samplesPerHalfBeat = (60.0 / info.bpm / 2.0) * sampleRate;
        if ((currentSample + blockSize) >= samplesPerHalfBeat) {
            int offset = jmax(0, jmin(samplesPerHalfBeat - currentSample, blockSize - 1));
            msg = beatFirstHalf
                ? MidiMessage::noteOff(16, 60)
                : MidiMessage::noteOn(16, 60, (uint8) 127);
            if (metronomeOn) midi.addEvent(msg, offset);
            if (!beatFirstHalf) currentBeat++;
            beatFirstHalf = !beatFirstHalf;
            update = true;
        }
        currentSample = (currentSample + blockSize) % samplesPerHalfBeat;
    }
    
    if (resetting) {
        doReset(midi);
        resetting = false;
        update = true;
    }

    if (update) triggerAsyncUpdate();
}

//==============================================================================
bool MidiDebuggerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* MidiDebuggerAudioProcessor::createEditor()
{
    MidiDebuggerAudioProcessorEditor *editor = new MidiDebuggerAudioProcessorEditor(*this);
    editor->addListener(this);
    return editor;
}

//==============================================================================
void MidiDebuggerAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MidiDebuggerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void MidiDebuggerAudioProcessor::handleAsyncUpdate()
{
    MidiDebuggerAudioProcessorEditor *editor =
        dynamic_cast<MidiDebuggerAudioProcessorEditor*>(getActiveEditor());

    if (editor) {
        for (MidiMessage *cur = messageLog.begin(); cur < messageLog.end(); cur++) {
            editor->logMidiMessage(*cur);
        }
        editor->setInfo(info.bpm,
                        floor(currentBeat % info.timeSigNumerator) + 1,
                        info.timeInSeconds,
                        info.timeInSamples);
    }
    
    messageLog.clear();
}

void MidiDebuggerAudioProcessor::buttonClicked(Button *b)
{
    ToggleButton *button = dynamic_cast<ToggleButton*>(b);
    if (button) {
        metronomeOn = button->getToggleState();
    } else {
        resetting = true;
    }
}

void MidiDebuggerAudioProcessor::doReset(MidiBuffer &midi)
{
    // clear all notes on all channels
    for (int i=0; i<16*128; i++) {
        midi.addEvent(MidiMessage::noteOff(i/128+1, i%128), 0);
    }

    currentSample = 0;
    currentBeat = 0;
    beatFirstHalf = true;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MidiDebuggerAudioProcessor();
}
