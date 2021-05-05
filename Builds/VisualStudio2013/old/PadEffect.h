#pragma once
#include <JuceHeader.h>
#include <chrono>

class PadEffect
{
public:
	PadEffect();

	void Update(long delta);
	virtual void Render(Graphics* g) = 0;

	bool IsEnabled()
	{
		return _enabled;
	}

	void SetPosition(float x, float y);

	
protected:
	// These are all protected (not private) on purpose 
	std::chrono::steady_clock::time_point _created;
	bool _enabled = true;
	juce::Point<float> _position;
	float _diameter = 1.0f;
	Colour _colour;
	std::chrono::milliseconds _lifespan = std::chrono::milliseconds(1000);
};

