# audio-plugins
A collection of audio plugins made with JUCE

## Basic Upmixer

![upmixer](https://user-images.githubusercontent.com/10064103/184654270-e9a42af9-e21a-4dbe-8e46-bea767aefc11.png)

This plugin takes in stereo and outputs left/right/centre. Where the centre channel is calculated as

```
centre = 0.5*(left + right)
```

The JUCE code is fairly straightforward and can be adapted to create higher order upmixers.

### `PluginProcessor.cpp`

* In `BasicUpmixerAudioProcessor()` we specify the number of the input and output channels

```cpp
.withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
.withOutput ("Output", juce::AudioChannelSet::createLCR(), true)
```

* `AudioChannelSet` has a number of preset channel configs

 ```cpp
 AudioChannelSet AudioChannelSet::mono()                { return AudioChannelSet ({ centre }); }
AudioChannelSet AudioChannelSet::stereo()              { return AudioChannelSet ({ left, right }); }
AudioChannelSet AudioChannelSet::createLCR()           { return AudioChannelSet ({ left, right, centre }); }
AudioChannelSet AudioChannelSet::createLRS()           { return AudioChannelSet ({ left, right, surround }); }
AudioChannelSet AudioChannelSet::createLCRS()          { return AudioChannelSet ({ left, right, centre, surround }); }
AudioChannelSet AudioChannelSet::create5point0()       { return AudioChannelSet ({ left, right, centre, leftSurround, rightSurround }); }
AudioChannelSet AudioChannelSet::create5point1()       { return AudioChannelSet ({ left, right, centre, LFE, leftSurround, rightSurround }); }
AudioChannelSet AudioChannelSet::create6point0()       { return AudioChannelSet ({ left, right, centre, leftSurround, rightSurround, centreSurround }); }
AudioChannelSet AudioChannelSet::create6point1()       { return AudioChannelSet ({ left, right, centre, LFE, leftSurround, rightSurround, centreSurround }); }
// and so on
```

* Modify `isBusesLayoutSupported` accordingly

```cpp
#ifndef JucePlugin_PreferredChannelConfigurations
bool BasicUpmixerAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const juce::AudioChannelSet& mainInput = layouts.getMainInputChannelSet();
    const juce::AudioChannelSet& mainOutput = layouts.getMainOutputChannelSet();

    return mainInput.size() == 2 && mainOutput.size() == 3;
}
#endif
```

* Set up the processing block

```cpp
    // Initialise write pointers
    auto outputL = buffer.getWritePointer(0);
    auto outputR = buffer.getWritePointer(1);
    auto outputC = buffer.getWritePointer(2);

    // Main processing loop
    for (int n = 0; n < numSamples; ++n)
    {
        float inputL = buffer.getSample(0, n);
        float inputR = buffer.getSample(1, n);

        outputL[n] = inputL;
        outputR[n] = inputR;
        outputC[n] = 0.5f * (inputL + inputR);

    }
```
