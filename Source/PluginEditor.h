#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Component/HorizontalMeter.h"
#include "Component/VerticalGradientMeter.h"
#include "Component/VerticalDiscreteMeter.h"
#include "Component/CircularMeter.h"

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
    
    Slider leftSlider, rightSlider, rmsPeriodSlider;
    AudioProcessorValueTreeState::SliderAttachment leftSliderAttachment, rightSliderAttachment, rmsPeriodAttachment;
    ToggleButton enableSmoothingButton;
    AudioProcessorValueTreeState::ButtonAttachment enableSmoothingAttachment;
	
    Label rmsLevelHeading1, rmsLevelHeading2;
    Label currentRmsLabel, maxRmsLabel;
    Label currentRmsValue, maxRmsValue;
    Label rmsPeriodLabel;
    float maxRmsLeft{}, maxRmsRight{};
    int framesElapsed = 0;    
	
    Gui::HorizontalMeter horizontalMeterL, horizontalMeterR;
    Gui::VerticalGradientMeter verticalGradientMeterL, verticalGradientMeterR;
    Gui::VerticalDiscreteMeter verticalDiscreteMeterL, verticalDiscreteMeterR;
    Gui::CircularMeter circularMeterL, circularMeterR;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LevelMeterAudioProcessorEditor)
};
