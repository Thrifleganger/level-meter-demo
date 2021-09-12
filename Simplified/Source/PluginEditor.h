/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Component/HorizontalMeter.h"
#include "Component/VerticalGradientMeter.h"
#include "Component/VerticalDiscreteMeter.h"

//==============================================================================
/**
*/
class LevelMeterAudioProcessorEditor  : public juce::AudioProcessorEditor, public Timer
{
public:
    LevelMeterAudioProcessorEditor (LevelMeterAudioProcessor&);
    ~LevelMeterAudioProcessorEditor() override;

    void timerCallback() override;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    LevelMeterAudioProcessor& audioProcessor;

    Gui::HorizontalMeter horizontalMeterL, horizontalMeterR;
    Gui::VerticalGradientMeter verticalGradientMeterL, verticalGradientMeterR;
    Gui::VerticalDiscreteMeter verticalDiscreteMeterL, verticalDiscreteMeterR;
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LevelMeterAudioProcessorEditor)
};
