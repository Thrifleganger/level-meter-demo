#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LevelMeterAudioProcessor::LevelMeterAudioProcessor()
	: AudioProcessor(BusesProperties()
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)),
	parameters(*this, nullptr, "LevelMeter", AudioProcessorValueTreeState::ParameterLayout{
		std::make_unique<AudioParameterFloat>("left", "Left", -60.f, 12.f, 0.f),
		std::make_unique<AudioParameterFloat>("right", "Right", -60.f, 12.f, 0.f),
		std::make_unique<AudioParameterInt>("rmsPeriod", "Period", 1, 500, 50),
		std::make_unique<AudioParameterBool>("smoothing", "Enable Smoothing", true)
	})
{
	parameters.addParameterListener("left", this);
	parameters.addParameterListener("right", this);
	parameters.addParameterListener("rmsPeriod", this);
	parameters.addParameterListener("smoothing", this);
}

LevelMeterAudioProcessor::~LevelMeterAudioProcessor()
{
	parameters.removeParameterListener("left", this);
	parameters.removeParameterListener("right", this);
	parameters.removeParameterListener("rmsPeriod", this);
	parameters.removeParameterListener("smoothing", this);
}

//==============================================================================
const juce::String LevelMeterAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool LevelMeterAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
	return false;
#endif
}

bool LevelMeterAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
	return false;
#endif
}

bool LevelMeterAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
	return false;
#endif
}

double LevelMeterAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int LevelMeterAudioProcessor::getNumPrograms()
{
	return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int LevelMeterAudioProcessor::getCurrentProgram()
{
	return 0;
}

void LevelMeterAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String LevelMeterAudioProcessor::getProgramName(int index)
{
	return {};
}

void LevelMeterAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void LevelMeterAudioProcessor::prepareToPlay(double sr, int samplesPerBlock)
{
	sampleRate = sr;
	const auto numberOfChannels = getTotalNumInputChannels();
	
	rmsLevels.clear();
	for (auto i = 0; i < numberOfChannels; i++)
	{
		LinearSmoothedValue<float> rms{ -100.f };
		rms.reset(sampleRate, 0.5);
		rmsLevels.emplace_back(std::move(rms));
	}

	rmsFifo.reset(numberOfChannels, static_cast<int>(sampleRate) + 1);
	rmsCalculationBuffer.clear();
	rmsCalculationBuffer.setSize(numberOfChannels, static_cast<int>(sampleRate) + 1);

	gainLeft.reset(sampleRate, 0.2);
	gainLeft.setCurrentAndTargetValue(Decibels::decibelsToGain(parameters.getRawParameterValue("left")->load()));
	
	gainRight.reset(sampleRate, 0.2);
	gainRight.setCurrentAndTargetValue(Decibels::decibelsToGain(parameters.getRawParameterValue("right")->load()));	

	rmsWindowSize =  static_cast<int> (sampleRate * parameters.getRawParameterValue("rmsPeriod")->load()) / 1000;
	isSmoothed = static_cast<bool> (parameters.getRawParameterValue("smoothing")->load());
}

void LevelMeterAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool LevelMeterAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	// Some plugin hosts, such as certain GarageBand versions, will only
	// load plugins that support stereo bus layouts.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

void LevelMeterAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	juce::ScopedNoDenormals noDenormals;
	const auto numSamples = buffer.getNumSamples();
	
	{
		const auto startGain = gainLeft.getCurrentValue();
		gainLeft.skip(numSamples);
		const auto endGain = gainLeft.getCurrentValue();
		buffer.applyGainRamp(0, 0, numSamples, startGain, endGain);
	}
	{
		const auto startGain = gainRight.getCurrentValue();
		gainRight.skip(numSamples);
		const auto endGain = gainRight.getCurrentValue();
		buffer.applyGainRamp(1, 0, numSamples, startGain, endGain);
	}

	for (auto& rmsLevel : rmsLevels)
		rmsLevel.skip(numSamples);
	
	rmsFifo.push(buffer);
}

//==============================================================================
bool LevelMeterAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* LevelMeterAudioProcessor::createEditor()
{
	return new LevelMeterAudioProcessorEditor(*this);
}

//==============================================================================
void LevelMeterAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void LevelMeterAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

void LevelMeterAudioProcessor::parameterChanged(const String& parameterID, float newValue)
{
	if (parameterID.equalsIgnoreCase("left"))
		gainLeft.setTargetValue(Decibels::decibelsToGain(newValue));
	if (parameterID.equalsIgnoreCase("right"))
		gainRight.setTargetValue(Decibels::decibelsToGain(newValue));
	if (parameterID.equalsIgnoreCase("rmsPeriod"))
		rmsWindowSize = static_cast<int>(sampleRate * newValue) / 1000;
	if (parameterID.equalsIgnoreCase("smoothing"))
		isSmoothed = static_cast<bool> (newValue);
}


std::vector<float> LevelMeterAudioProcessor::getRmsLevels()
{
	rmsFifo.pull(rmsCalculationBuffer, rmsWindowSize);
	std::vector<float> levels;
	for (auto channel = 0; channel < rmsCalculationBuffer.getNumChannels(); channel++)
	{
		processLevelValue(rmsLevels[channel], Decibels::gainToDecibels(rmsCalculationBuffer.getRMSLevel(channel, 0, rmsWindowSize)));
		levels.push_back(rmsLevels[channel].getCurrentValue());
	}
	return levels;
}

float LevelMeterAudioProcessor::getRmsLevel(const int channel)
{
	jassert(channel >= 0 && channel < rmsCalculationBuffer.getNumChannels());
	rmsFifo.pull(rmsCalculationBuffer.getWritePointer(channel), channel, rmsWindowSize);
	processLevelValue(rmsLevels[channel], Decibels::gainToDecibels(rmsCalculationBuffer.getRMSLevel(channel, 0, rmsWindowSize)));
	return rmsLevels[channel].getCurrentValue();
}

void LevelMeterAudioProcessor::processLevelValue(LinearSmoothedValue<float>& smoothedValue, const float value) const
{
	if (isSmoothed)
	{
		if (value < smoothedValue.getCurrentValue())
		{
			smoothedValue.setTargetValue(value);
			return;
		}			
	}
	smoothedValue.setCurrentAndTargetValue(value);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new LevelMeterAudioProcessor();
}
