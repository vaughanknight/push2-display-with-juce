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
#include "Push2.h"

extern "C" {
	#include "../modules/Doom/doom.h"

	void dg_Create();

}
#include <stdio.h>
#include "../Builds/VisualStudio2013/PadRenderer.h"

//------------------------------------------------------------------------------

namespace
{
	bool SMatchSubStringNoCase(const std::string& haystack, const std::string& needle)
	{
		auto it = std::search(
			haystack.begin(), haystack.end(),
			needle.begin(), needle.end(),
			[](char ch1, char ch2)                // case insensitive
			{
				return std::toupper(ch1) == std::toupper(ch2);
			});
		return it != haystack.end();
	}
}

NBase::Result Demo::Init()
{
	Demo::instance_ = this;

	const String threadName = String("Hello");

	auto doomThread = new DoomThread("Doom Thread");
	doomThread->startThread();

	//explicit Thread(const String & threadName, size_t threadStackSize = 0);
	

	//std::thread doomThread([this] { this->RunDoom(); });

	// First we initialise the low level push2 object
	NBase::Result result = push2Display_.Init();
	RETURN_IF_FAILED_MESSAGE(result, "Failed to init push2");

	// Then we initialise the juce to push bridge
	result = bridge_.Init(push2Display_);
	RETURN_IF_FAILED_MESSAGE(result, "Failed to init bridge");

	// Initialises the midi input
	result = openMidiDevice();
	RETURN_IF_FAILED_MESSAGE(result, "Failed to open midi device");

	// Reset elapsed time
	elapsed_ = 0;
	offset_ = 0;
	widthOffset_ = 0;

	// Start the timer to draw the animation
	startTimerHz(60);

	return NBase::Result::NoError;
}

//------------------------------------------------------------------------------

NBase::Result Demo::openMidiDevice()
{
	// Look for an input device matching push 2

	auto inputDevices = MidiInput::getDevices();

	auto inputDeviceIndex = GetAbletonDevice(inputDevices);
	if (inputDeviceIndex == -1)
	{
		return NBase::Result("Failed to find input midi device for push2");
	}
	auto input = MidiInput::openDevice(inputDeviceIndex, this);

	auto outputDevices = MidiOutput::getDevices();

	auto outputDeviceIndex = GetAbletonDevice(outputDevices);

	if (outputDeviceIndex == -1)
	{
		return NBase::Result("Failed to find output midi device for push2");
	}
	auto output = MidiOutput::openDevice(outputDeviceIndex);

	// Try opening the device
	/*
	auto output = MidiOutput::openDevice(deviceIndex);*/

	if (!input)
	{
		return NBase::Result("Failed to open input device");
	}

	// Store and starts listening to the device
	midiInput_.reset(input);
	midiOutput_.reset(output);
	midiOutput_->startBackgroundThread();

	midiInput_->start();


	return NBase::Result::NoError;
}

int Demo::GetAbletonDevice(juce::StringArray& devices)
{
	int deviceIndex = -1;
	int index = 0;
	for (auto& device : devices)
	{
		if (SMatchSubStringNoCase(device.toStdString(), "ableton push 2"))
		{
			deviceIndex = index;
			break;
		}
		index++;
	}
	return deviceIndex;
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

void Demo::OnPadPressed(Push2Pad pad)
{
	MidiMessage  message = MidiMessage(0x90, pad, pad % 8 + 30);
	MidiBuffer buffer = MidiBuffer(message);
	midiOutput_->sendBlockOfMessages(buffer, Time::getMillisecondCounter() + 100, 64);

	//uint32 bytes[9] = {
	//	0xF0, // SYSEX
	//	0x00, 0x21, 0x1D, // Ableton ID
	//	0x01, // Device ID
	//	0x01, // Model ID 
	//	0x04, // Get RGB Colour Pallette Entry
	//	0x7D, // Pallette index (0-127)
	//	0xF7 }; // End Command 

	//MidiMessage getColourMessage = MidiMessage(bytes, 9);
	//MidiBuffer getColourBuffer = MidiBuffer(getColourMessage);
	//midiOutput_->sendBlockOfMessages(getColourBuffer, Time::getMillisecondCounter() + 100, 64);

	
}

void Demo::SetColour(int status, int button, int value)
{
	MidiMessage message = MidiMessage(status, button, value);
	MidiBuffer buffer = MidiBuffer(message);

	midiOutput_->sendBlockOfMessages(buffer, Time::getMillisecondCounter() + 100, 64);
}

//------------------------------------------------------------------------------

void Demo::SetMidiInputCallback(const midicb_t& callback)
{
	midiCallback_ = callback;
}


//------------------------------------------------------------------------------

void Demo::handleIncomingMidiMessage(MidiInput* /*source*/, const MidiMessage& message)
{
	// if a callback has been set, forward the incoming message
	if (midiCallback_)
	{
		midiCallback_(message);
	}
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

void Demo::drawFrame()
{
	// Request a juce::Graphics from the bridge
	auto& g = bridge_.GetGraphic();
	//float hue = (std::sin(elapsed_ / 4.0f) + 1.0f) / 2.0f;
	float hue2 = (std::sin((1.6 + elapsed_) / 1.0f) + 1.0f) / 2.0f;
	//// Clear previous frame
	g.fillAll(juce::Colour(hue2, 1.0f, 0.5f, 1.0f));

	auto doomPixels = std::vector<juce::Colour>();
	juce::Image image = juce::Image(juce::Image::PixelFormat::RGB, doomRenderWidth_, doomRenderHeight_, true);
	
	for (int i = 0; i < doomRenderHeight_ ; i++)
	{
		for (int j = 0; j < doomRenderWidth_; j++)
		{
			int pi = (j + i* doomRenderWidth_)*4;
			uint8_t* data = ((uint8_t*)doomRenderData_);

			uint8 blue = data[pi + 0];
			uint8 green = data[pi + 1];
			uint8 red = data[pi + 2];
			/*
			g.setColour(juce::Colour(red, green , blue, 1.0f));*/
			image.setPixelAt(j, i, juce::Colour(red, green, blue, 1.0f));
			//g.setPixel(j, i);
			
			//g.setPixel(i, j);
			/*doomPixels.push_back(juce::Colour(red, green, blue));*/
		}
	}
	g.drawImage(image, 320, 0, 640, 160, 0, 0, doomRenderWidth_, doomRenderHeight_-64);

	int hudTop = doomRenderHeight_ - 64;

	auto concrete = ImageCache::getFromFile(juce::File(juce::String("c://Projects/GitHub/push2-display-with-juce/Resources/concrete.png")));
	g.drawImage(concrete, 0, 0, 320, 160, 0, 0, 317, 160);
	g.drawImage(image, 15, 12, 211, 64, 0, hudTop, 211, 64);
	g.drawImage(image, 226, 12, 75, 64, 211, hudTop, 75, 64);
	g.drawImage(image, 24, 78, 142, 64, 211+148, hudTop, 142, 64);
	g.drawImage(image, 156, 78, 142, 64, 211 + 148 + 142, hudTop, 142, 64);
	
	/*288
		hudTop
		69
		64*/
	auto rect = juce::Rectangle<int>(288, hudTop+2, 69, 64);

	auto doomGuy = image.getClippedImage(rect);
	auto tinyDoomGuy = doomGuy.rescaled(8, 8);

	PadRenderer padRenderer = PadRenderer(*this);

	padRenderer.Render(tinyDoomGuy, 0, 0);

	//// Create a path for the animated wave
	//const auto height = ableton::Push2DisplayBitmap::kHeight;
	//const auto width = ableton::Push2DisplayBitmap::kWidth;

	//Path wavePath;

	//const float waveStep = 10.0f;
	//const float waveY = height * 0.44f;
	//int i = 0;
	//if (doomRenderData != NULL)
	//{
	//	int i = 5;
	//}
	//for (float x = waveStep * 0.5f; x < width; x += waveStep)
	//{
	//	const float y1 = waveY + height * 0.10f * std::sin(i * (0.38f + offset_) + elapsed_);
	//	const float y2 = waveY + height * 0.20f * std::sin(i * (0.20f + offset_) + elapsed_ * 2.0f);


	//	wavePath.addLineSegment(Line<float>(x, y1, x, y2), 2.0f * (1.0f + widthOffset_));
	//	wavePath.addEllipse(x - waveStep * 0.3f, y1 - waveStep * 0.3f, waveStep * 0.6f, waveStep * 0.6f);
	//	wavePath.addEllipse(x - waveStep * 0.3f, y2 - waveStep * 0.3f, waveStep * 0.6f, waveStep * 0.6f);

	//	++i;
	//}

	//auto marioWalkPad = ImageCache::getFromFile(juce::File(juce::String("c://Projects/GitHub/push2-display-with-juce/Resources/mariowalk.png")));
	////auto marioWalkScreen = ImageCache::getFromFile(juce::File(juce::String("c://Projects/GitHub/push2-display-with-juce/Resources/mariowalk_transparent.png")));

	//PadRenderer padRenderer = PadRenderer(*this);

	//int frame = (int)(elapsed_ * 8.0 ) % 6;
	//padRenderer.Render(marioWalkPad, frame * 8, 0);
	////
	////juce::Colour c = juce::Colour(0.5f, 0, 0.5f);
	////g.setColour(juce::Colour(hue, 1.0f, 0.5f, 1.0f));
	//g.setColour(juce::Colour(0, 0, 0));

	//// Draw the path
	////g.setColour(juce::Colour::greyLevel(0.5f));

	//g.fillPath(wavePath);

	//// Blit the logo on top
	//auto logo = ImageCache::getFromFile(juce::File(juce::String("c://Projects/GitHub/push2-display-with-juce/Resources/AzureLogo.png")));

	//g.setImageResamplingQuality(juce::Graphics::lowResamplingQuality);
	////g.drawImageAt(logo, 0, 0);
	//const int IMAGE_SPACING = 80;


	//for (int i = -IMAGE_SPACING; i < width +  IMAGE_SPACING; i+= IMAGE_SPACING)
	//{
	//	for (int j = 0; j < height; j+= IMAGE_SPACING)
	//	{
	//		int walkOffset = (((int)(elapsed_ * 80.0 * (1+offset_))) % IMAGE_SPACING) - j/2;
	//		int spriteFrame = ((int)(elapsed_ * 8.0)) % 6 ;
	//		g.drawImage(marioWalkScreen, i + walkOffset, 8+j, 64, 64, spriteFrame * 8  , 0, 8, 8);
	//	}
	//}
	// Tells the bridge we're done with drawing and the frame can be sent to the display
	bridge_.Flip();
}


DoomThread::DoomThread(const String& threadName) : Thread(threadName)
{
		
}

void DoomThread::run()
{
	dg_Create();
	D_DoomMain();
}

void Push2_SetRenderData(int width, int height, void* data)
{
	Demo::Instance()->SetRenderData( width,  height, data);
}

Demo* Demo::instance_ = NULL;


