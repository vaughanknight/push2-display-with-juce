#pragma once
#include <JuceHeader.h>

class AnimationObject
{
public:
	virtual void Render(Graphics g) = 0;
private:
	int px;
	int py;
};

