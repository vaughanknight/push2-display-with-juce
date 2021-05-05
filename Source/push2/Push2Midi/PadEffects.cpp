#include <chrono>
#include "PadRenderer.h"
#include "PadEffects.h"
#include <stdio.h>

PadEffects::PadEffects()
{
	_init();
	this->startTimerHz(60);
}

void PadEffects::_init()
{
	_timeStart = std::chrono::steady_clock::now();
	_durationElapsed = std::chrono::steady_clock::duration(0);
	_deltaDuration = std::chrono::steady_clock::duration(0);
	_deltaMilliseconds = 0;
	_padImage = Image(Image::PixelFormat::RGB, 8, 8, true);
	_padGraphics = new Graphics(_padImage);
	_padRenderer = new PadRenderer();
}

void PadEffects::timerCallback()
{
	_updateTimers();
	_update();
	_render();
}

void PadEffects::AddEffect(PadEffect *effect)
{
	_effects.push_back(effect);
}

void PadEffects::_update()
{
	for (int i = 0; i < _effects.size(); i++)
	{
		auto padEffect = _effects.at(i);
		padEffect->Update(_deltaMilliseconds);
		if (!padEffect->IsEnabled())
		{
			_effects.erase(_effects.begin() + i);
			i--;
			delete padEffect;
		}
	}
}

void PadEffects::_updateTimers()
{
	auto now = std::chrono::steady_clock::now();
	_deltaDuration = now - _timeLastFrame;
	_deltaMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(_deltaDuration).count();
	_durationElapsed = now - _timeStart;
	_timeLastFrame = now;
}

void PadEffects::_render()
{
	_padImage.clear(juce::Rectangle<int>(0, 0, 8, 8), juce::Colour::fromRGB(0, 0, 0));
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			auto p = _padImage.getPixelAt(i, j);
		}
	}

	for (int i = 0; i < _effects.size(); i++)
	{
		auto padEffect = _effects.at(i);
		padEffect->Render(_padGraphics);
	}

	_padRenderer->Render(_padImage, 0, 0);
}