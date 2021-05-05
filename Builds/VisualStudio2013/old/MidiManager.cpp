#include "MidiManager.h"
#include <JuceHeader.h>
#include <stdio.h>

#include "Push2.h"
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

MidiManager* MidiManager::_instance = NULL;

void MidiManager::Init()
{
	this->_openMidiDevice();
}

void MidiManager::SetMidiInputCallback(const midiCallback_t& callback)
{
	_midiCallback = callback;
}

bool MidiManager::_openMidiDevice()
{
	if (!InitialiseInputMidi()) return false;
	if (!InitialiseOutputMidi()) return false;

	return true;
}

bool MidiManager::InitialiseInputMidi()
{
	auto inputDevices = MidiInput::getDevices();
	auto inputDeviceIndex = _getAbletonDevice(inputDevices);
	if (inputDeviceIndex == -1)
	{
		return false;
	}
	auto input = MidiInput::openDevice(inputDeviceIndex, this);
	
	if (!input)
	{
		return false;
	}
	_midiInput.reset(input);
	_midiInput->start();

	return true;
}

bool MidiManager::InitialiseOutputMidi()
{
	auto outputDevices = MidiOutput::getDevices();
	auto outputDeviceIndex = _getAbletonDevice(outputDevices);
	if (outputDeviceIndex == -1)
	{
		return false;
	}
	auto output = MidiOutput::openDevice(outputDeviceIndex);
	if (!output)
	{
		return false;
	}
	_midiOutput.reset(output);
	_midiOutput->startBackgroundThread();

	return true;
}

int MidiManager::_getAbletonDevice(juce::StringArray& devices)
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

int MidiManager::_getAbletonOutputDevice()
{
	// Look for an output device matching push 2
	auto outputDevices = MidiOutput::getDevices();
	auto outputDeviceIndex = _getAbletonDevice(outputDevices);
	return outputDeviceIndex;
}

int MidiManager::_getAbletonInputDevice()
{
	// Look for an input device matching push 2
	auto inputDevices = MidiInput::getDevices();
	auto inputDeviceIndex = _getAbletonDevice(inputDevices);
	return inputDeviceIndex;
}

void MidiManager::SendMessages(MidiBuffer& buffer)
{
	_midiOutput->sendBlockOfMessages(buffer, Time::getMillisecondCounter() + 100, 64);
}

void MidiManager::handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message)
{
	if (_midiCallback)
	{
		_midiCallback(message);
	}
}
void MidiManager::SetColours(std::vector<int> padColours)
{
	MidiBuffer buffer = MidiBuffer();

	
	int j = 0;
	for (int i = 0; i < padColours.size(); i++)
	{
		auto c = padColours.at(i);

		if (c >= 0)
		{
			MidiMessage message = MidiMessage(0x90, Push2Pad::P00 + i, padColours.at(i));
			buffer.addEvent(message, i);
			j++;
		}
	}

	if (!buffer.isEmpty())
	{
		_midiOutput->sendBlockOfMessages(buffer, Time::getMillisecondCounter(), 1000);
	}
}

void MidiManager::SetColours(std::vector<Colour> colours)
{
	PadColours pc;
	std::vector<int> padColours;

	for (int i = 0; i < colours.size(); i++)
	{
		Colour c = colours.at(i);
		padColours.push_back(pc.FindNearest(c));
	}
	
	SetColours(padColours);
}

void MidiManager::SetColour(int status, int button, int value)
{
	MidiMessage message = MidiMessage(status, button, value);
	MidiBuffer buffer = MidiBuffer(message);

	_midiOutput->sendBlockOfMessages(buffer, Time::getMillisecondCounter() + 10, 64);
}