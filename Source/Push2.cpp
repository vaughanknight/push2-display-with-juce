#include "Push2.h"

bool Push2::IsPad(int inputId)
{
	return inputId >= Push2Pad::P00 && inputId <= Push2Pad::P77;
}

PadColours::PadColours()
{{
	colours_ = std::vector<juce::Colour>();

	for (int i = 0; i < 128 ; i++)
	{
		auto c = juce::Colour(colourBytes[i][0], colourBytes[i][1], colourBytes[i][2]);

		colours_.push_back(c);
	}
}}

uint8 PadColours::FindNearest(juce::Colour& colour)
{
	auto closest = 1000.0;
	uint8 index = -1;

	for (int i = 0; i < colours_.size(); i++)
	{
		auto c = colours_.at(i);
		auto dr = square((colour.getRed()- c.getRed()) );
		auto dg = square((colour.getGreen() - c.getGreen()) );
		auto db = square((colour.getBlue() - c.getBlue()) );

		auto distance = sqrt(dr + dg + db);
		
		if (distance < closest)
		{
			closest = distance;
			index = i;
		}

		if (0 == distance)
		{
			break;
		}
	}

	return index;
}
