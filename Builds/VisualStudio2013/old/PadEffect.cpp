#include "PadEffect.h"
#include <time.h>

PadEffect::PadEffect()
{
	_created = std::chrono::steady_clock::now();
	_colour = Colour((uint8)255, 0, 0, 1.0f);
}

void PadEffect::Update(long delta)
{
	if (std::chrono::steady_clock::now() - _created > _lifespan)
	{
		_enabled = false;
	}
}

void PadEffect::SetPosition(float x, float y)
{
	_position.x = x;
	_position.y = y;
}
