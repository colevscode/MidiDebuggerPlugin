# MidiDebuggerPlugin
A JUCE based VST plugin that logs midi messages and provides a metronome.

This serves as both a debug utility and a demo of:
* Calculating host BPM and current beat
* Generating midi metronome output
* Updating the UX asynchronously using AsyncUpdater
* Responding to UX events like button clicks 

Below I've broken out each demo bullet as a separate HOWTO.

## How to calculate host BPM and current beat counter

First save the `sampleRate` and `suggestedBlockSize` from `prepareToPlay`. 
We'll use those values to calculate and update the beat count.

Then, in `processBlock`:

```cpp
    AudioPlayHead *head = getPlayHead();
    if (head) {
        head->getCurrentPosition(info);
        
        // Use the current bpm to calculate samplesPerBeat.
        int samplesPerBeat = (60.0 / info.bpm) * sampleRate;
        
        // If sample count exceeds the threashold, increment the beat
        if ((currentSample + blockSize) >= samplesPerBeat) {
            currentBeat = (currentBeat+1) / info.timeSigNumerator;
        }
        
        // update the current sample
        currentSample = (currentSample + blockSize) % samplesPerBeat;
    }    
```

Notes: 
* This doens't respect the current beat 
* Since we don't have inbound midi samples, we're calculating current sample directly from the blockSize. 
  This creates a continuous beat   
