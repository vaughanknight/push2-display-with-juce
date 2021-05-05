#pragma once
#include <JuceHeader.h>
#include <cctype>

class PadRenderer
{
public:
	
	void Render(juce::Image& image, int x, int y);
	PadRenderer();

private: 
	std::vector<int> _frameState;
};

