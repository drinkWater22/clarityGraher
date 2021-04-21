#include "MainComponent.h"

#define SMA_AVERAGE 20

//==============================================================================
MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (1200, 800);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }

    addAndMakeVisible(grapher1);
    grapher1.setTraceColour(juce::Colours::white);
    grapher1.setBackgroundColour(juce::Colours::black);
    grapher1.setVerticalZoom(2.0);
    grapher1.setHorizontalZoom(0.01);

    addAndMakeVisible(grapher2);
    grapher2.setTraceColour(juce::Colours::white);
    grapher2.setBackgroundColour(juce::Colours::black);
    grapher2.setVerticalZoom(2.0);
    grapher2.setHorizontalZoom(0.01);

    cache1 = createCache(SMA_AVERAGE);

}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
	auto* inBuffer = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);
	grapher1.processBlock(inBuffer, bufferToFill.numSamples);

	for(auto channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)	
	{
		auto* inBuffer = bufferToFill.buffer->getReadPointer(channel, bufferToFill.startSample);
		auto* outBuffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);
		for(auto sample = 0; sample < bufferToFill.numSamples; ++sample)
		{
			outBuffer[sample] = simpleMovingAverage(SMA_AVERAGE, inBuffer[sample], cache1);
		}
	}

	auto* inBuffer2 = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);
	grapher2.processBlock(inBuffer2, bufferToFill.numSamples);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    free(cache1);
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto area = getLocalBounds();
    grapher1.setBounds(area.removeFromTop(area.getHeight() / 2));
    grapher2.setBounds(area);
}

float* MainComponent::createCache(int period)
{
	return (float*)malloc(sizeof(float) * period);
}

float MainComponent::simpleMovingAverage(int period, float dataPoint, float* cache)
{
	static int count = 0;
	float accumulator = 0;
	if( count >= SMA_AVERAGE )
	{
		for(int i = 0; i < period-1; i++)
		{
			cache[i] = cache[i+1];	
		}
		cache[period-1] = dataPoint;
		
		for(int i = 0; i < period; i++)	
		{
			accumulator += *(cache + i);
		}
		return accumulator/period;
	}
	else
	{
		*(cache + count) = dataPoint;
		count++;
		return dataPoint;
	}
}
