#include "PluginProcessor.h"
#include "PluginEditor.h"

LevelMeterAudioProcessorEditor::LevelMeterAudioProcessorEditor(LevelMeterAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
    leftSliderAttachment(p.getApvts(), "left", leftSlider),
    rightSliderAttachment(p.getApvts(), "right", rightSlider),
    rmsPeriodAttachment(p.getApvts(), "rmsPeriod", rmsPeriodSlider),
    enableSmoothingAttachment(p.getApvts(), "smoothing", enableSmoothingButton),
    verticalGradientMeterL([&]() { return audioProcessor.getRmsLevel(0); }),
    verticalGradientMeterR([&]() { return audioProcessor.getRmsLevel(1); }),
    verticalDiscreteMeterL([&]() { return audioProcessor.getRmsLevel(0); }),
    verticalDiscreteMeterR([&]() { return audioProcessor.getRmsLevel(1); }),
    circularMeterL([&]() { return audioProcessor.getRmsLevel(0); }, Colours::violet),
    circularMeterR([&]() { return audioProcessor.getRmsLevel(1); }, Colours::cyan)    
{
    addAndMakeVisible(rmsLevelHeading1);
    addAndMakeVisible(rmsLevelHeading2);
    addAndMakeVisible(currentRmsLabel);
    addAndMakeVisible(maxRmsLabel);
    addAndMakeVisible(currentRmsValue);
    addAndMakeVisible(maxRmsValue);
    addAndMakeVisible(rmsPeriodLabel);

    addAndMakeVisible(horizontalMeterL);
    addAndMakeVisible(horizontalMeterR);

    addAndMakeVisible(verticalGradientMeterL);
    addAndMakeVisible(verticalGradientMeterR);

    addAndMakeVisible(verticalDiscreteMeterL);
    addAndMakeVisible(verticalDiscreteMeterR);

    addAndMakeVisible(circularMeterL);
    addAndMakeVisible(circularMeterR);

    addAndMakeVisible(leftSlider);
    addAndMakeVisible(rightSlider);
    addAndMakeVisible(rmsPeriodSlider);
    addAndMakeVisible(enableSmoothingButton);

    rmsLevelHeading1.setText("dBFS", dontSendNotification);
    rmsLevelHeading1.setFont(Font{}.withStyle(Font::FontStyleFlags::bold));
    rmsLevelHeading2.setText("Left \t Right", dontSendNotification);
    rmsLevelHeading2.setFont(Font{}.withStyle(Font::FontStyleFlags::bold));
    currentRmsLabel.setText("Current RMS:", dontSendNotification);
    maxRmsLabel.setText("Max RMS:", dontSendNotification);
    rmsPeriodLabel.setText("RMS Period", dontSendNotification);
    rmsPeriodLabel.setJustificationType(Justification::right);

    leftSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    leftSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    leftSlider.setPopupDisplayEnabled(true, false, this);
    leftSlider.setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::violet.withBrightness(0.6f));
    leftSlider.setColour(Slider::ColourIds::thumbColourId, Colours::violet);
    leftSlider.setTextValueSuffix(" dB : Left Channel");

    rightSlider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);   
    rightSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    rightSlider.setPopupDisplayEnabled(true, false, this);
    rightSlider.setColour(Slider::ColourIds::rotarySliderFillColourId, Colours::cyan.withBrightness(0.6f));
    rightSlider.setColour(Slider::ColourIds::thumbColourId, Colours::cyan);
    rightSlider.setTextValueSuffix(" dB : Right Channel");

    rmsPeriodSlider.setSliderStyle(Slider::LinearHorizontal);
    rmsPeriodSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    rmsPeriodSlider.setPopupDisplayEnabled(true, false, this);
    rmsPeriodSlider.setTextValueSuffix(" ms");

    enableSmoothingButton.setButtonText("Enable smoothing");    

    setSize (400, 650);
    setResizable(true, true);
    startTimerHz(24);
}

LevelMeterAudioProcessorEditor::~LevelMeterAudioProcessorEditor()
{
    stopTimer();
}

void LevelMeterAudioProcessorEditor::timerCallback()
{
    if (++framesElapsed > 100)
    {
        framesElapsed = 0;
        maxRmsLeft = -100.f;
        maxRmsRight = -100.f;
    }

    const auto leftGain = audioProcessor.getRmsLevel(0);
    const auto rightGain = audioProcessor.getRmsLevel(1);
    if (leftGain > maxRmsLeft)
        maxRmsLeft = leftGain;
    if (rightGain > maxRmsRight)
        maxRmsRight = rightGain;
    currentRmsValue.setText(String{ leftGain, 2 } + "   " + String{ rightGain, 2 }, sendNotification);
    maxRmsValue.setText(String{ maxRmsLeft, 2 } + "   " + String{ maxRmsRight, 2 }, sendNotification);

    horizontalMeterL.setLevel(leftGain);
    horizontalMeterL.repaint();

    horizontalMeterR.setLevel(rightGain);
    horizontalMeterR.repaint();
}

//==============================================================================
void LevelMeterAudioProcessorEditor::paint (juce::Graphics& g)
{

    g.setGradientFill(ColourGradient{ Colours::darkgrey, getLocalBounds().toFloat().getCentre(), Colours::darkgrey.darker(0.7f), {}, true });
    g.fillRect(getLocalBounds());

    g.setColour(Colours::black);
    g.fillEllipse(circularMeterL.getBounds().toFloat());
}

void LevelMeterAudioProcessorEditor::resized()
{
    const auto container = getBounds().reduced(20);
    auto bounds = container;

    auto labelBounds = bounds.removeFromTop(container.proportionOfHeight(0.12f));
    auto controlBounds = labelBounds.removeFromRight(container.proportionOfWidth(0.35f));

    const auto labelHeight = labelBounds.proportionOfHeight(0.33f);

    auto labelRow1 = labelBounds.removeFromTop(labelHeight);
    rmsLevelHeading1.setBounds(labelRow1.removeFromLeft(labelRow1.proportionOfWidth(0.5f)));
    rmsLevelHeading2.setBounds(labelRow1);

    auto labelRow2 = labelBounds.removeFromTop(labelHeight);
    maxRmsLabel.setBounds(labelRow2.removeFromLeft(labelRow2.proportionOfWidth(0.5f)));
    maxRmsValue.setBounds(labelRow2);

    auto labelRow3 = labelBounds;
    currentRmsLabel.setBounds(labelRow3.removeFromLeft(labelRow3.proportionOfWidth(0.5f)));
    currentRmsValue.setBounds(labelRow3);

    rmsPeriodLabel.setBounds(controlBounds.removeFromTop(labelHeight));
    rmsPeriodSlider.setBounds(controlBounds.removeFromTop(labelHeight));
    enableSmoothingButton.setBounds(controlBounds);

    auto horizontalMeterBounds = bounds.removeFromTop(container.proportionOfHeight(0.1f)).reduced(5);
    horizontalMeterL.setBounds(horizontalMeterBounds.removeFromTop(horizontalMeterBounds.proportionOfHeight(0.5f)).reduced(5));
    horizontalMeterR.setBounds(horizontalMeterBounds.reduced(5));

    auto verticalMeterBounds = bounds.removeFromTop(container.proportionOfHeight(0.35f)).reduced(container.proportionOfWidth(0.1f), 5);
    auto verticalMeter1 = verticalMeterBounds.removeFromLeft(verticalMeterBounds.proportionOfWidth(0.5f))
        .withSizeKeepingCentre(55, 200);
    verticalGradientMeterL.setBounds(verticalMeter1.removeFromLeft(25));
    verticalMeter1.removeFromLeft(5);
    verticalGradientMeterR.setBounds(verticalMeter1);

    auto verticalMeter2 = verticalMeterBounds.withSizeKeepingCentre(55, verticalMeterBounds.getHeight());
    verticalDiscreteMeterL.setBounds(verticalMeter2.removeFromLeft(25));
    verticalMeter2.removeFromLeft(5);
    verticalDiscreteMeterR.setBounds(verticalMeter2);

    auto circularMeterBounds = bounds.removeFromTop(container.proportionOfHeight(0.3f)).reduced(5);
    const auto diameter = jmin(circularMeterBounds.getWidth(), circularMeterBounds.getHeight());
    circularMeterBounds = circularMeterBounds.withSizeKeepingCentre(diameter, diameter);
    circularMeterL.setBounds(circularMeterBounds);
    circularMeterR.setBounds(circularMeterBounds);

    auto sliderBounds = bounds.reduced(5);
    sliderBounds = sliderBounds.withSizeKeepingCentre(sliderBounds.proportionOfWidth(0.6f), sliderBounds.getHeight());
    leftSlider.setBounds(sliderBounds.removeFromLeft(sliderBounds.proportionOfWidth(0.5f)));
    rightSlider.setBounds(sliderBounds);
}
