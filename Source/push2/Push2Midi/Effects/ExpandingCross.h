#pragma once
#include "../PadEffect.h"


class ExpandingCross : public PadEffect
{
public:
	void Render(Graphics* g);

	void DrawCross(juce::Graphics* g, float distance, int offset);

};
