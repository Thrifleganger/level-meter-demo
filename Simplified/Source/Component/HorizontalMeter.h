#pragma once

#include <JuceHeader.h>

namespace Gui
{
	class HorizontalMeter : public Component
	{
	public:
		void paint(Graphics& g) override
		{
			auto bounds = getLocalBounds().toFloat();

			g.setColour(Colours::white.withBrightness(0.4f));
			g.fillRoundedRectangle(bounds, 5.f);

			g.setColour(Colours::white);
			// map level from -60.f / +6.f  to  0 / width
			const auto scaledX = 
				jmap(level, -60.f, +6.f, 0.f, static_cast<float>(getWidth()));
			g.fillRoundedRectangle(bounds.removeFromLeft(scaledX), 5.f);
		}

		void setLevel(const float value) { level = value; }
	private:
		float level = -60.f;
	};
}
