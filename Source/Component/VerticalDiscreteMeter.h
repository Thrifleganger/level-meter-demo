#pragma once

#include <JuceHeader.h>

namespace Gui
{
	class Bulb : public Component
	{
	public:
		Bulb(const Colour& c) : colour(c) {}
		void paint(Graphics& g) override
		{
			if (isOn)
			{
				g.setColour(colour);
				
			} else
			{
				g.setColour(Colours::black);
			}
			const auto delta = 4.f;
			const auto bounds = getLocalBounds().toFloat().reduced(delta);
			const auto side = jmin(bounds.getWidth(), bounds.getHeight());
			const auto bulbFillBounds = Rectangle<float>{ bounds.getX(), bounds.getY(), side, side };
			g.fillEllipse(bulbFillBounds);
			g.setColour(Colours::black);
			g.drawEllipse(bulbFillBounds, 1.f);
			if (isOn)
			{
				g.setGradientFill(ColourGradient{ colour.withAlpha(0.3f), bulbFillBounds.getCentre(), colour.withLightness(1.5f).withAlpha(0.f), {}, true });
				g.fillEllipse(bulbFillBounds.expanded(delta));
			}
					
		}
		void setState(const bool state) { isOn = state; }
	private:
		bool isOn = false;
		Colour colour{};
	};
	
	class VerticalDiscreteMeter : public Component, public Timer
	{
	public:
		VerticalDiscreteMeter(std::function<float()>&& valueFunction) : valueSupplier(std::move(valueFunction))
		{
			startTimerHz(24);			
		}

		void paint(Graphics& g) override
		{
			const auto value = jmap(valueSupplier(), -60.f, 6.f, 0.f, 1.f);
			for (auto i = 0; i < totalNumberOfBulbs; i++)
			{
				if (value >= static_cast<float>(i + 1) / totalNumberOfBulbs)
					bulbs[i]->setState(true);
				else
					bulbs[i]->setState(false);	
			}
		}

		void resized() override
		{
			const auto bounds = getLocalBounds().toFloat();
			ColourGradient gradient{ Colours::green, bounds.getBottomLeft(), Colours::red, bounds.getTopLeft(), false };
			gradient.addColour(0.5, Colours::yellow);

			const auto bulbHeight = getLocalBounds().getHeight() / totalNumberOfBulbs;
			auto bulbBounds = getLocalBounds();
			bulbs.clear();
			for (auto i = 0; i < totalNumberOfBulbs; i++)
			{
				auto bulb = std::make_unique<Bulb>(gradient.getColourAtPosition(static_cast<double>(i) / totalNumberOfBulbs));
				addAndMakeVisible(bulb.get());
				bulb->setBounds(bulbBounds.removeFromBottom(bulbHeight));
				bulbs.push_back(std::move(bulb));
			}			
		}

		void timerCallback() override
		{
			repaint();
		}
	
	private:
		std::function<float()> valueSupplier;		
		std::vector<std::unique_ptr<Bulb>> bulbs;
		const int totalNumberOfBulbs = 10;
	};
}
