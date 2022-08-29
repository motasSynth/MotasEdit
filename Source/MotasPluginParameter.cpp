
/*
Copyright
2023
J. M. Hayes
Motas Electronics Limited

This file is part of MotasEdit.

MotasEdit is free software: you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation,
 either version 3 of the License, or (at your option) any later version.

MotasEdit is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with MotasEdit.
 If not, see <https://www.gnu.org/licenses/>.


*/



/*
 * MotasPluginParameter.cpp
 *
 */

#include "./Headers/MotasPluginParameter.h"
#include "Headers/PluginProcessor.h"


#include "Headers/utilities.h"
#include "Headers/MIDIProcessor.h"
MotasPluginParameter::MotasPluginParameter(MotasEditPluginAudioProcessor* processor,
		const String& parameterID, const String& name,
		int parameterPage, int subParam, int min, int max, int32_t defaultValue,
		std::function< String(int value, int maximumStringLength)>  lambdaFunction)

:  AudioParameterInt(parameterID, name, min, max, defaultValue, String(),
		 lambdaFunction, nullptr),
   processor(processor),  paramPage(parameterPage),subParam(subParam), source(0)
{





	//int defaultValue = 2048;

	//AudioParameterInt(parameterID, name, minValue, maxValue, defaultValue, String(), nullptr, nullptr);
}





MotasPluginParameter::~MotasPluginParameter()
{
	// TODO Auto-generated destructor stub
}



void MotasPluginParameter::getNRPNBasic(uint8_t* buffer, int newValue)
{
	if (!isPitch())
	{
		newValue += (128 * 128) / 2; // offset to positive values
	}

	buffer[0] = (uint8_t)paramPage;
	buffer[1] = (uint8_t) subParam;
	buffer[2] = (uint8_t) ((newValue / 128) & 0x7F);
	buffer[3] = (uint8_t)((uint32_t) newValue % 128);
}



void MotasPluginParameter::valueChanged(int newValue)
{
	// value has changed so add to the MIDI FIFO
	if (source == MotasPluginParameter::SOURCE_FROM_DAW)
	{
		uint8_t mm[4];
		getNRPNBasic(mm, newValue);
		processor->fifoMidiParameters.addToFifo(mm, 4);
		processor->midiProcessor->notify();
	} else
	{
		// reset the source!
		source = SOURCE_FROM_DAW;
	}

	DBG("MotasPluginParameter::valueChanged: " + String(newValue));
}



/**
 *
 * Rather cunning this, when we want to change a DAW parameter from internal
 * to MotasEdit (either from GUI control or from Motas synth MIDI input)
 * we call this function
 * . Then, especially when change came from Motas we can avoid sending the
 * data out of MIDI port again by knowing the source! So for internal (i.e. not called by the DAW,
 * which will call the overrided function 'valueChanged') use call 'setValueInternal'
 */

void MotasPluginParameter::setValueInternal(uint8_t msb, uint8_t lsb)
{
	source = SOURCE_INTERNAL;
	float val;
	if (isModulation())
	{
		val = 0.5f + (((int) msb * 128 + (int) lsb) - (128*128 / 2)) / (4095.0f * 2.0f);
	}
	else if (isPitch())
	{
		val = (((int) msb * 128 + (int) lsb) ) / 12000.0f;
	} else if (isLFOWaveform())
	{
		val = (((int) msb * 128 + (int) lsb) ) / 50.0f;
	}
	else if (paramPage == 1 && subParam == 1)
	{
		val = 0.5f + (((int) msb * 128 + (int) lsb) - (128*128 / 2)) / (4095.0f * 2.0f);
	} else
	{
		val = (((int) msb * 128 + (int) lsb) - (128*128 / 2)) / 4095.0f;
	}


	// depending on the parameter we will need to scale appropriately


	DBG("DAWparameterChanged, from motas: : " + String(val));

	/*
QUESTION
	Is it safe to call setValueNotifyingHost() (and beginChangeGesture()/endChangeGesture()) from within the processBlock()?

	I’ve been doing this with no issues for some time. Works perfectly in Pro Tools,
	for example. But, I have a Logic user reporting issues with distortion and noise.

	Am I doing something ill advised, or is this a Logic idiosyncrasy?

ANSWER
	Nope, that definitely isn’t safe, I’m afraid.

	Just because you haven’t heard of any problems doesn’t mean it’s safe.
	You’ve no idea what the host will do when you invoke that call, but it’s very likely that
	some of them will allocate memory or post a message, which will occasionally cause problems
	depending on what else the host is doing at a particular moment.

	The only safe way to send events from your process method is to have your own
	lock-free fifo to post events which you later pick up in another thread (or on a timer) and do the work there.
	*/

	setValueNotifyingHost(val);

	source = SOURCE_INTERNAL;

}


bool MotasPluginParameter::isDiscrete()
{
	bool isDiscrete = false;
	if (isLFOFreq())
		isDiscrete = true;
	return isDiscrete;
}

bool MotasPluginParameter::isLFOFreq()
{
	if (paramPage == 0 && (subParam >= 4 && subParam <= 8))
		return true;
	else
		return false;
}
bool MotasPluginParameter::isLFOWaveform()
{
	if (paramPage == 0 && (subParam >= 80 && subParam <= 83))
		return true;
	else
		return false;
}

bool MotasPluginParameter::isOrientationInverted()
{
	/*if (isEGRate())
		return true;
	else
		return false;*/
	return false;
}

bool MotasPluginParameter::isEGRate()
{
	if (paramPage == 0 &&
			((subParam >= 20 && subParam <= 31)
			|| (subParam >= 44 && subParam <= 47)
			))
		return true;
	else
		return false;

}

bool MotasPluginParameter::isPitch()
{
	if (subParam == 1 && (paramPage == 3 || paramPage == 10 || paramPage == 17))
		return true;
	else
		return false;
}

bool MotasPluginParameter::isModulation()
{
	if (paramPage > 0 && (subParam == 5 || subParam == 20 || subParam == 26))
		return true;
	else
		return false;
}


