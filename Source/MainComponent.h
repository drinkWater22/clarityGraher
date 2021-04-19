#pragma once

#include <JuceHeader.h>
#include "dRowAudio/dRowAudio.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    // Algorithms
    float* createCache(int period); // Make sure allocate and free cache for SMA
    float simpleMovingAverage(int period, float dataPoint, float* cache);

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...
    drow::AudioOscilloscope grapher1;
    drow::AudioOscilloscope grapher2;
    float* cache1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
