#include "ExpandingCross.h"

void ExpandingCross::Render(Graphics* g)
{


	auto age = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::steady_clock::now() - _created);

	int distance = age.count() * 120.0f / (1000.0f * 5.0f);
	auto agePercent = (age.count() / 1000.0f) / 1.5f;

	_colour = Colour((uint8)128, 0, 128, 1.0f);
	int offset = 0;

	DrawCross(g, distance, 0);

	if (distance > 1)
	{
		_colour = Colour((uint8)255, 0, 0, 1.0f);
		DrawCross(g, distance, 1);
	}
	if (distance > 2)
	{
		_colour = Colour((uint8)255, 128, 0, 1.0f);
		DrawCross(g, distance, 2);
	}
	if (distance > 3)
	{
		_colour = Colour((uint8)255,255, 128, 1.0f);
		DrawCross(g, distance, 3);
	}
}

void ExpandingCross::DrawCross(juce::Graphics* g, float distance, int offset)
{
	g->setColour(_colour);
	g->setPixel(_position.x, _position.y + distance - offset);
	g->setPixel(_position.x + distance - offset, _position.y + distance - offset);
	g->setPixel(_position.x + distance - offset, _position.y);
	g->setPixel(_position.x + distance - offset, _position.y - distance + offset);
	g->setPixel(_position.x, _position.y - distance + offset);
	g->setPixel(_position.x - distance + offset, _position.y - distance + offset);
	g->setPixel(_position.x - distance + offset, _position.y);
	g->setPixel(_position.x - distance + offset, _position.y + distance - offset);
}
