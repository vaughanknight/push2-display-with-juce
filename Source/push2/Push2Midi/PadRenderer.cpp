#include "PadRenderer.h"
#include "Push2.h"

void PadRenderer::Render(juce::Image& image, int x, int y)
{
	auto pixelList = std::vector<int>();

	PadColours pc = PadColours();

	int i = 0;
	for (int dy = 7; dy >= 0; dy--)
	{
		for (int dx = 0; dx < 8; dx++)
		{
			
			auto c = image.getPixelAt( dx + x,  dy + y);
			auto padColour = pc.FindNearest(c);

			auto oldPixel = _frameState.at(i);

			if (oldPixel == padColour) // The pixel hasn't changed, load -1 into the render buffer 
			{
				pixelList.push_back(-1);
			}
			else // Pixel has changed, load it into the render buffer
			{
				pixelList.push_back(padColour);
				_frameState.at(i) = padColour;
			}
			i++;
		}
	}
	MidiManager::getInstance()->SetColours(pixelList);
}

PadRenderer::PadRenderer() 
{
	_frameState = std::vector<int>(64);
	for (int i = 0; i < 64; i++) _frameState.at(i) = -1;
}
