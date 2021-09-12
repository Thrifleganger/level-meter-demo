#pragma once

#include <JuceHeader.h>

namespace Gui
{
	class VerticalGradientMeter : public Component, public Timer
	{
	public:
		VerticalGradientMeter(std::function<float()>&& valueFunction) : valueSupplier(std::move(valueFunction))
		{
			startTimerHz(24);
			grill = ImageCache::getFromMemory(BinaryData::MeterGrill_png, BinaryData::MeterGrill_pngSize);
		}
		
		void paint(Graphics& g) override
		{
			auto bounds = getLocalBounds().toFloat().reduced(3.f);			

			g.setColour(Colours::black);
			g.fillRect(bounds);

			g.setGradientFill(gradient);
			const auto scaledY = jmap(valueSupplier(), -60.f, 6.f, 0.f, static_cast<float>(getHeight()));
			g.fillRect(bounds.removeFromBottom(scaledY));
		}

		void resized() override
		{
			const auto bounds = getLocalBounds().toFloat();
			gradient = ColourGradient{ Colours::green, bounds.getBottomLeft(), Colours::red, bounds.getTopLeft(), false };
			gradient.addColour(0.5, Colours::yellow);
		}
		
		void paintOverChildren(::juce::Graphics& g) override
		{
			g.drawImage(grill, getLocalBounds().toFloat());
		}

		void timerCallback() override
		{
			repaint();
		}
	private:
		std::function<float()> valueSupplier;
		ColourGradient gradient{};
		Image grill;
	};
}
