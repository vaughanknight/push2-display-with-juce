#pragma once
#include <JuceHeader.h>
#include <chrono>
#include "PadEffect.h"

class PadEffects : public Timer
{
public:
	PadEffects::PadEffects();

	void AddEffect(PadEffect *effect);

private:
	void _update();
	void _updateTimers();
	void _render();
	void _init();

	std::chrono::steady_clock::duration _deltaDuration;
	long _deltaMilliseconds;
	std::chrono::steady_clock::time_point _timeLastFrame;
	std::chrono::steady_clock::duration _durationElapsed;
	std::chrono::steady_clock::time_point _timeStart;

	Image  _padImage;
	Graphics* _padGraphics;

	std::vector<PadEffect*> _effects;

	// Inherited via Timer
	virtual void timerCallback() override;
};