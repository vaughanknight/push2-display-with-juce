// Copyright (c) 2017 Ableton AG, Berlin
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include "Push2Demo.h"
#include <cctype>
#include <algorithm>
#include "Push2/Push2.h"

extern "C" {
#include "../modules/Doom/doom.h"
#include "../modules/Doom/doomkeys.h"

	void dg_Create();
	void addOoomKeyToQueue(int pressed, unsigned char keyCode);
}
#include <stdio.h>
#include "Push2/Push2.h"

NBase::Result Demo::Init()
{
	Demo::instance_ = this;

	const String threadName = String("Hello");
	//_padEffects = new PadEffects();

	auto doomThread = new DoomThread("Doom Thread");
	doomThread->startThread();
	this->colourShift_ = new int[3];

	// First we initialise the low level push2 object
	NBase::Result result = push2Display_.Init();
	RETURN_IF_FAILED_MESSAGE(result, "Failed to init push2");

	// Then we initialise the juce to push bridge
	result = bridge_.Init(push2Display_);
	RETURN_IF_FAILED_MESSAGE(result, "Failed to init bridge");

	// Initialises the midi input
	result = openMidiDevice(); // VK
	RETURN_IF_FAILED_MESSAGE(result, "Failed to open midi device");

	// Reset elapsed time
	elapsed_ = 0;
	offset_ = 0;
	widthOffset_ = 0;

	// Start the timer to draw the animation
	startTimerHz(60);
	_padEffects = new PadEffects();
	_padRenderer = new PadRenderer();

	return NBase::Result::NoError;
}

//------------------------------------------------------------------------------

NBase::Result Demo::openMidiDevice()
{
/*
	return NBase::Result::NoError;*/
	auto midi = MidiManager::getInstance();
	midi->Init();
	
	return NBase::Result::NoError;

}

void Demo::SetRenderData(int width, int height, void* data)
{
	auto size = width * height * 4;

	if (doomRenderData_ == 0)
	{
		doomRenderWidth_ = width;
		doomRenderHeight_ = height;
		doomRenderData_ = reinterpret_cast<uint32_t*>(malloc(size));
	}

	memcpy(doomRenderData_, data, size);
}

int Demo::PadToDoomKey(Push2Pad pad)
{
	
	int status = 0;
	switch (pad)
	{
	case Push2Pad::P50:
	{
		status = KEY_LEFTARROW;
		break;
	}
	case Push2Pad::P51:
	{
		status = KEY_DOWNARROW;
		break;
	}
	case Push2Pad::P52:
	{
		status = KEY_RIGHTARROW;
		break;
	}
	case Push2Pad::P60:
	{
		status = KEY_STRAFE_L;
		break;
	}
	case Push2Pad::P61:
	{
		status = KEY_UPARROW;
		break;
	}
	case Push2Pad::P62:
	{
		status = KEY_STRAFE_R;
		break;
	}
	case Push2Pad::P57:
	{
		status = KEY_FIRE;
		break;
	}
	case Push2Pad::P55:
	{
		status = KEY_USE;
		break;
	}
	case Push2Pad::P07:
	{
		status = KEY_ENTER;
		break;
	}
	}
	return status;
}

void Demo::OnPadReleased(Push2Pad pad)
{
	addOoomKeyToQueue(0, PadToDoomKey(pad));
}

void Demo::OnControlChange(uint8 control, uint8 value)
{
	switch (control)
	{
	case 0x4C:
		{
			if (value == 1)
			{
				this->colourShift_[0]++;
			}
			else if (value == 127)
			{
				this->colourShift_[0]--;
			}
			break;
		}
		case 0x4D:
		{
			if (value == 1)
			{
				this->colourShift_[1]++;
			}
			else if (value == 127)
			{
				this->colourShift_[1]--;
			}
			break;
		}
		case 0x4E:
		{
			if (value == 1)
			{
				this->colourShift_[2]++;
			}
			else if (value == 127)
			{
				this->colourShift_[2]--;
			}
			break;
		}
	}
}

void Demo::OnPadPressed(Push2Pad pad)
{
	addOoomKeyToQueue(1, PadToDoomKey(pad));
	this->lastKeyPressed_ = pad;
	colourShift_[0] = colourShift_[1] = colourShift_[2] = 0;

	int padNumber = pad - Push2Pad::P00;

	int px = padNumber % 8;
	int py = padNumber / 8;

	auto effect = new ExpandingCircle();
	effect->SetPosition(px, 7-py);

	_padEffects->AddEffect(effect);
}

//------------------------------------------------------------------------------

void Demo::timerCallback()
{
	elapsed_ += 0.02f;
	drawFrame();
}

void Demo::ShiftOffset(float delta)
{
	offset_ += delta;
}

void Demo::WidthOffset(float delta)
{
	widthOffset_ += delta;
}

//------------------------------------------------------------------------------

int Demo::clamp(int value, int lower, int upper)
{
	int status = std::min(value, upper);
	status = std::max(value, lower);
	return status;
}

void Demo::drawFrame()
{
	// Request a juce::Graphics from the bridge
	auto& g = bridge_.GetGraphic();

	// Render Doom To It
	//RenderDoom(g);

	/*auto screen = ImageCache::getFromFile(juce::File(juce::String("c://Projects/GitHub/push2-display-with-juce/Resources/voting.png")));
	auto pad = ImageCache::getFromFile(juce::File(juce::String("c://Projects/GitHub/push2-display-with-juce/Resources/votes.png")));

	g.drawImage(screen, 0, 0, 960, 160, 0, 0, 960, 160);
	_padRenderer->Render(pad, 0, 0);*/

	// Flip the off screen buffer to the display 
	bridge_.Flip();
}

void Demo::ColourTweakerRender(juce::Graphics& g)
{
	auto pc = PadColours();

	if (lastKeyPressed_ >= Push2Pad::P00 && lastKeyPressed_ <= Push2Pad::P77)
	{
		auto rgb = pc.colourBytes[lastKeyPressed_ - Push2Pad::P00 + 64];
		uint8 red = clamp((int)rgb[0] + (int)colourShift_[0], 0, 255);
		uint8 green = clamp((int)rgb[1] + (int)colourShift_[1], 0, 255);
		uint8 blue = clamp((int)rgb[2] + (int)colourShift_[2], 0, 255);

		auto c = juce::Colour(red, green, blue, 1.0f);
		g.fillAll(c);

		g.setColour(juce::Colour((uint8)0, 0, 0, 1.0f));
		g.setFont(80);
		juce::String s = juce::String("rgb(");
		s.append(juce::String(red), 3);
		s.append(juce::String(","), 1);
		s.append(juce::String(green), 3);
		s.append(juce::String(","), 1);
		s.append(juce::String(blue), 3);
		s.append(juce::String(")"), 1);

		g.drawText(juce::String(s), juce::Rectangle<int>(0, 0, 960, 100), juce::Justification::left);
	}
}

void Demo::RenderDoom(juce::Graphics& g)
{
	auto doomPixels = std::vector<juce::Colour>();
	juce::Image image = juce::Image(juce::Image::PixelFormat::RGB, doomRenderWidth_, doomRenderHeight_, true);

	for (int i = 0; i < doomRenderHeight_; i++)
	{
		for (int j = 0; j < doomRenderWidth_; j++)
		{
			int pi = (j + i * doomRenderWidth_) * 4;
			uint8_t* data = ((uint8_t*)doomRenderData_);

			uint8 blue = data[pi + 0];
			uint8 green = data[pi + 1];
			uint8 red = data[pi + 2];

			image.setPixelAt(j, i, juce::Colour(red, green, blue, 1.0f));
		}
	}
	g.drawImage(image, 320, 0, 640, 160, 0, 0, doomRenderWidth_, doomRenderHeight_ - 64);

	int hudTop = doomRenderHeight_ - 64;

	auto concrete = ImageCache::getFromFile(juce::File(juce::String("c://Projects/GitHub/push2-display-with-juce/Resources/concrete.png")));
	g.drawImage(concrete, 0, 0, 320, 160, 0, 0, 317, 160);
	g.drawImage(image, 15, 12, 211, 64, 0, hudTop, 211, 64);
	g.drawImage(image, 226, 12, 75, 64, 211, hudTop, 75, 64);
	g.drawImage(image, 24, 78, 142, 64, 211 + 148, hudTop, 142, 64);
	g.drawImage(image, 156, 78, 142, 64, 211 + 148 + 142, hudTop, 142, 64);


	/*288
	hudTop
	69
	64*/
	//auto rect = juce::Rectangle<int>(288, hudTop + 2, 69, 64);
	//auto rect = juce::Rectangle<int>(0, 0, 640, hudTop);

	//auto doomGuy = image.getClippedImage(rect);

	//
	//auto tinyDoomGuy = doomGuy.rescaled(8, 8, Graphics::ResamplingQuality::highResamplingQuality);
	//tinyDoomGuy
	//auto padGraphics = new juce::Graphics(tinyDoomGuy);
	//padGraphics->drawImage(controlOverlay, 0, 0, 8, 8, 0, 0, 8, 8);
	/*auto controlOverlay = ImageCache::getFromFile(juce::File(juce::String("c://Projects/GitHub/push2-display-with-juce/Resources/controls.png")));
	juce::Graphics* padGraphics = new juce::Graphics(controlOverlay);
	delete padGraphics;*/
	//auto eggplant = ImageCache::getFromFile(juce::File(juce::String("c://Projects/GitHub/push2-display-with-juce/Resources/eggplant.png")));
	//eggplant = eggplant.rescaled(8, 8);

	/*auto padRender = PadRenderer(*this);
	padRender.Render(eggplant, 0, 0);*/
}

DoomThread::DoomThread(const String& threadName) : Thread(threadName)
{

}

void DoomThread::run()
{
	/*dg_Create();
	D_DoomMain();*/
}

void Push2_SetRenderData(int width, int height, void* data)
{
	Demo::Instance()->SetRenderData(width, height, data);
}

Demo* Demo::instance_ = NULL;


