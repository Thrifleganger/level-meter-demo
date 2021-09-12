/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LevelMeterAudioProcessorEditor::LevelMeterAudioProcessorEditor (LevelMeterAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    verticalGradientMeterL([&]() { return audioProcessor.getRmsValue(0); }),
    verticalGradientMeterR([&]() { return audioProcessor.getRmsValue(1); }),
    verticalDiscreteMeterL([&]() { return audioProcessor.getRmsValue(0); }),
    verticalDiscreteMeterR([&]() { return audioProcessor.getRmsValue(1); })
{
    addAndMakeVisible(horizontalMeterL);
    addAndMakeVisible(horizontalMeterR);

    addAndMakeVisible(verticalGradientMeterL);
    addAndMakeVisible(verticalGradientMeterR);

    addAndMakeVisible(verticalDiscreteMeterL);
    addAndMakeVisible(verticalDiscreteMeterR);

    setSize (400, 500);

    startTimerHz(24);
}

LevelMeterAudioProcessorEditor::~LevelMeterAudioProcessorEditor()
{
}

void LevelMeterAudioProcessorEditor::timerCallback()
{
    horizontalMeterL.setLevel(audioProcessor.getRmsValue(0));
    horizontalMeterR.setLevel(audioProcessor.getRmsValue(1));

    horizontalMeterL.repaint();
    horizontalMeterR.repaint();
}

//==============================================================================
void LevelMeterAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(Colours::darkgrey);
}

void LevelMeterAudioProcessorEditor::resized()
{
    horizontalMeterL.setBounds(100, 100, 200, 15);
    horizontalMeterR.setBounds(100, 120, 200, 15);

    verticalGradientMeterL.setBounds(100, 200, 25, 200);
    verticalGradientMeterR.setBounds(130, 200, 25, 200);

    verticalDiscreteMeterL.setBounds(200, 200, 25, 200);
    verticalDiscreteMeterR.setBounds(230, 200, 25, 200);
}
