#pragma once
#include <cctype>
#include "JuceHeader.h"
#include "Effects/Effects.h"

#include "MidiManager.h"
#include "PadEffects.h"
#include "PadRenderer.h"

typedef enum  Push2Pad
{
	P00 = 0x24, P01, P02, P03, P04, P05, P06, P07,
	P10, P11, P12, P13, P14, P15, P16, P17,
	P20, P21, P22, P23, P24, P25, P26, P27,
	P30, P31, P32, P33, P34, P35, P36, P37,
	P40, P41, P42, P43, P44, P45, P46, P47,
	P50, P51, P52, P53, P54, P55, P56, P57,
	P60, P61, P62, P63, P64, P65, P66, P67,
	P70, P71, P72, P73, P74, P75, P76, P77
};

enum Push2Status
{
	ButtonRelease = 0x80,
	ButtonDown = 0x90,
	ControlChange = 0xB0
};

enum Push2Control
{
	ButtonPlay = 0x55
};

class Push2
{
public:
	bool IsPad(int inputId);
};

class PadColours
{
private:
	std::vector<juce::Colour> colours_;
public:
	PadColours();
	uint8 FindNearest(juce::Colour& color);

	// These were manually checked and tested
	// after finding many sources on the internet
	// there wasn't a single list that accurately met
	// my LED's colours.  The Push2 can report
	// back the LED colours, but colour they are trying
	// to be, and what colour the LED's reproduce is different
	// Painfully checked against all 128 in the standard 
	// and tweaked.  Only as accurate as my eyes
	juce::uint8 colourBytes[128][3] =
	{
		0,0,0,
		255,36,36,
		242,21,20,
		255,153,0,
		242,58,12,
		237,249,90,
		128,55,0,
		237,249,90,
		237,249,90,
		86,191,19,
		44,132,3,
		36,107,36,
		25,255,48,
		21,149,115,
		23,107,80,
		0,255,255,
		0,116,252,
		39,79,204,
		0,68,140,
		100,74,217,
		77,63,160,
		135,0,255,
		102,0,153,
		230,87,227,
		179,49,105,
		255,0,80,
		255,0,196,
		248,115,109,
		153,86,40,
		135,103,0,
		144,130,31,
		60,200,10,
		0,160,40,
		24,83,178,
		98,75,173,
		115,58,103,
		248,188,175,
		255,155,118,
		255,191,95,
		217,175,113,
		255,244,128,
		191,186,105,
		188,204,136,
		174,255,153,
		124,221,159,
		137,180,125,
		128,243,255,
		122,206,252,
		104,161,211,
		133,143,194,
		187,170,242,
		205,187,228,
		239,139,176,
		133,157,140,
		107,117,110,
		132,144,155,
		106,112,117,
		136,133,157,
		108,106,117,
		157,133,156,
		116,106,116,
		156,157,133,
		116,117,106,
		157,132,132,
		117,106,106,
		180,60,60,
		60,20,11,
		140,20,10,
		80,11,5,
		128,46,0,
		60,30,10,
		150,30,5,
		50,30,10,
		77,73,31,
		36,24,10,
		64,30,7,
		26,21,1,
		150,150,40,
		26,26,0,
		120,120,7,
		44,45,3,
		69,106,1,
		20,40,0,
		40,128,17,
		3,40,10,
		10,96,20,
		3,40,5,
		34,68,10,
		25,36,10,
		16,96,64,
		3,32,16,
		0,105,105,
		0,26,26,
		0,35,77,
		0,33,37,
		0,110,130,
		0,22,28,
		0,37,77,
		0,12,26,
		35,8,100,
		11,0,39,
		50,0,96,
		30,0,40,
		0,60,102,
		13,17,38,
		77,29,104,
		26,16,50,
		51,0,77,
		28,0,38,
		96,26,53,
		26,0,24,
		96,26,53,
		35,12,21,
		176,26,70,
		51,8,20,
		126,8,81,
		40,15,24,
		0,0,0,
		89,89,89,
		26,26,26,
		255,255,255,
		89,89,89,
		255,255,255,
		89,89,89,
		26,26,26,
		0,0,255,
		0,255,0,
		255,0,0
	};

};

