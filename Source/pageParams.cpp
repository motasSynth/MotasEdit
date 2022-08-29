
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
 * pageParams.cpp
 *
 *
 */

#include "../JuceLibraryCode/JuceHeader.h"

#include "Headers/pageParams.h"


 const StringArray globalDestination =
{"lfo1 waveform",
		"lfo1 frequency",
		"lfo1 tempo", // in firmware v1C / patch v8, acts the same as frequency, but not removed so as not to disturb!

		"lfo1 single-shot",
		"lfo1 sync/reset",
		"lfo1 freq mode",

		"lfo2 waveform",
		"lfo2 frequency",
		"lfo2 tempo",
		"lfo2 single-shot",
		"lfo2 sync/reset",
		"lfo2 freq mode",

		"lfo3 waveform",
		"lfo3 frequency",
		"lfo3 tempo",
		"lfo3 single-shot",
		"lfo3 sync/reset",
		"lfo3 freq mode",

		"lfo4 waveform",
		"lfo4 frequency",
		"lfo4 tempo",
		"lfo4 single-shot",
		"lfo4 sync/reset",
		"lfo4 freq mode",

		"eg1 delay",
		"eg1 attack",
		"eg1 decay",
		"eg1 sustain",
		"eg1 release",
		"eg1 keytrack",
		"eg1 reset/loop",
		"eg1 shape",
		"eg1 response",

		"eg2 delay",
		"eg2 attack",
		"eg2 decay",
		"eg2 sustain",
		"eg2 release",
		"eg2 keytrack",
		"eg2 reset/loop",
		"eg2 shape",
		"eg2 response",

		"eg3 delay",
		"eg3 attack",
		"eg3 decay",
		"eg3 sustain",
		"eg3 release",
		"eg3 keytrack",
		"eg3 reset/loop",
		"eg3 shape",
		"eg3 response",

		"eg4 delay",
		"eg4 attack",
		"eg4 decay",
		"eg4 sustain",
		"eg4 release",
		"eg4 keytrack",
		"eg4 reset/loop",
		"eg4 shape",
		"eg4 response"
};


const StringArray pageDestinations =
{
		"offset",
		"page option 1",
		"page option 2",

		"note depth",
		"note 2nd dest",
		"note 2nd depth",
		"note response",

		"velocity depth",
		"vel. 2nd dest",
		"vel. 2nd depth",
		"vel. response",

		"m1 depth",
		"m1 2nd dest",
		"m1 2nd depth",
		"m1 response",

		"m2 depth",
		"m2 2nd dest",
		"m2 2nd depth",
		"m2 response",

		"m3 depth",
		"m3 2nd dest",
		"m3 2nd depth",
		"m3 response",

		"m4 depth",
		"m4 2nd dest",
		"m4 2nd depth",
		"m4 response",

		"lfox choice",
		"lfox depth",

		"lfo waveform",
		"lfo frequency",
		"lfo depth",
		"lfo output mode",
		"lfo single-shot",
		"lfo sync/reset",
		"lfo freq mode",
		"lfo fmod lfox",
		"lfo fmod eg",

		"eg delay",
		"eg attack",
		"eg decay",
		"eg sustain",
		"eg release",
		"eg depth",
		"eg choice",
		"eg keytrack",
		"eg reset/loop",
		"eg shape",
		"eg response",


};



const StringArray patchSetupDestination =
{

		"port. mode",
		"port. time/rate" ,
"port. value",
"track note-off",
"pw sensitivity",
"lfo sync",



};


const StringArray pageSettings =
{
		"off",
	"global params",
	"master pitch",
	"osc-1:level",
	"osc-1:pitch",
	"osc-1:triangle",
	"osc-1:sawtooth",
	"osc-1:pulse mod.",
	"osc-1:pulse lev",

	"osc-2:phase mod", // 9
	"osc-2:level",
	"osc-2:pitch",
	"osc-2:triangle",
	"osc-2:sawtooth",
	"osc-2:square",
	"osc-2:sub-osc",

	"osc-3:phase mod", // 16
	"osc-3:level",
	"osc-3:pitch",
	"osc-3:triangle",
	"osc-3:sawtooth",
	"osc-3:pulse mod.",
	"osc-3:pulse lev",

	"mixer:noise", //23
	"mixer:level",
	"mixer:fb/ext",
	"lpf1:frequency",
	"lpf1:resonance",
	"lpf1:output",
	"lpf2:frequency",
	"lpf2:resonance",
	"lpf2:output",
	"hpf:frequency",
	"hpf:output",
	"output", //34
	"patch settings",
};



