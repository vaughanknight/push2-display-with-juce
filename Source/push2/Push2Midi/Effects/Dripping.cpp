#include "Dripping.h"
#include <math.h>

Dripping::Dripping()
{
	_lifespan = std::chrono::seconds(2);
}

void Dripping::Render(Graphics* g)
{
	auto age = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::steady_clock::now() - _created);

	auto timeInSeconds = age.count() / 1000.0f;

	auto distance = -(cos(timeInSeconds)-1) * 5.0f;

	float l = _lifespan.count();
	float a = age.count();

	auto colourIntensity = (cos(timeInSeconds) + 1) / 2.0f;
	
	_colour = Colour((uint8)(255* colourIntensity), 0, 0, 1.0f);
	int offset = 0;
	g->setColour(_colour);
	g->setPixel(_position.x, _position.y + distance);
}
