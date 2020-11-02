#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "../JuceLibraryCode/BinaryData.h"
#include <cctype>
#include "Push2Demo.h"

class PadRenderer
{
public:
	
	void Render(juce::Image& image, int x, int y);
	PadRenderer(Demo& demo);

private:
	Demo& _demo;
};

