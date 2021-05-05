#include "ExpandingCircle.h"

void ExpandingCircle::Render(Graphics* g)
{
	auto diameter = 30.0f;

	auto age = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::steady_clock::now() - _created);

	diameter = age.count() * 200.0f / (1000.0f * 5.0f);
	auto agePercent = (age.count() / 1000.0f) / 1.5f;

	auto radius = diameter / 2.0f;
	float x = _position.x - radius + 0.5f;
	float y = _position.y - radius + 0.5f;
	_colour = Colour((uint8)128, 0, 128, 1.0f);

	g->setColour(_colour);
	g->drawEllipse(x, y, diameter, diameter, 1);

	if (diameter > 3)
	{
		_colour = Colour((uint8)255, 0, 0, 1.0f);
		g->setColour(_colour);
		g->drawEllipse(x + 1, y + 1, diameter - 2, diameter - 2, 1);
	}
	if (diameter > 5)
	{
		_colour = Colour((uint8)255, 128, 0, 1.0f);
		g->setColour(_colour);
		g->drawEllipse(x + 2, y + 2, diameter - 4, diameter - 4, 1);
	}
	if (diameter > 7)
	{
		_colour = Colour((uint8)255, 255, 128, 1.0f);
		g->setColour(_colour);
		g->drawEllipse(x + 3, y + 3, diameter - 6, diameter - 6, 1);
	}
}
