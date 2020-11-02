#include "PadRenderer.h"
#include "Push2.h"
#include "Push2Demo.h"

void PadRenderer::Render(juce::Image& image, int x, int y)
{
	std::vector<juce::Colour> pixelList = std::vector<juce::Colour>();

	PadColours pc = PadColours();


	for (int dy = 7; dy >= 0; dy--)
	{
		for (int dx = 0; dx < 8; dx++)
		{
			auto c = image.getPixelAt( dx + x,  dy + y);

			pixelList.push_back(c);
		}
	}

	for (int px = Push2Pad::P00, i = 0; i < pixelList.size(); i++, px++)
	{ 
		auto c = pixelList.at(i);
		auto toPad = pc.FindNearest(c);

		_demo.SetColour(0x90, px, toPad);
	}
}

PadRenderer::PadRenderer(Demo& demo) : _demo(demo)
{

}
