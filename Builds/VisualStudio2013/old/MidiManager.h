#pragma once
#include <Result.h>
#include <JuceHeader.h>
#include <stdio.h>

class MidiManager
	:
	public MidiInputCallback
{
public:
	static MidiManager* getInstance()
	{
		if (_instance == NULL)
		{
			_instance = new MidiManager();
		}
		return _instance;
	}
	void Init();

	using midiCallback_t = std::function<void(const MidiMessage&)>;
	void SetMidiInputCallback(const midiCallback_t& callback);
	void SetColour(int status, int button, int value);
	void SetColours(std::vector<Colour> colours);
	void SetColours(std::vector<int> padColours);

	int _getAbletonDevice(juce::StringArray& devices);
	int _getAbletonOutputDevice();
	int _getAbletonInputDevice();
	void SendMessages(MidiBuffer& buffer);
private:
	MidiManager() {} // Constructor? (the {} brackets) are needed here.
	static MidiManager* _instance;

	// We don't want copies of a singleton
	MidiManager(MidiManager const&) = delete;              // Don't Implement
	void operator=(MidiManager const&) = delete; // Don't implement

	// Inherited via MidiInputCallback
	virtual void handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message) override;

	bool _openMidiDevice();
	bool InitialiseInputMidi();
	bool InitialiseOutputMidi();

	std::unique_ptr<MidiInput> _midiInput;
	std::unique_ptr<MidiOutput> _midiOutput;

	midiCallback_t _midiCallback;
};

