
/*
Copyright 2022
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
  ==============================================================================

    ApplicationCommon.h


  ==============================================================================
*/



#ifndef APPLICATION_COMMON_H
#define APPLICATION_COMMON_H

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "motasDefines.h"
#include "crc.h"
class MIDIProcessor;
// Manually set the #define below to help out Eclipse! Rather than entirely relying on JUCE setup
#define BUILD_STANDALONE_EXECUTABLE 1



/*
 *
 * SO if you ask for new ideas, I might have one (three, actually) :

Create a stripped down version of the synth editor, a VST, not a VSTi, targeted toward using the Motas as an
insert fx. It would be with a lot less buttons obviously.
That's not something we see a lot and I think it's a shame it's not more common, because when you are working,
let's say, on a drum part, you want to add some Motas spice to it, then to load the big vsti in the middle of
the fx chain is weird. I'd rather have a little ui that
just allow me to store the analog goodness without overthinking too much the process !

Something that could be nice also was if the Motas plugin had not only the ability to communicate with the synth
directly,
but also an option to let a stream of midi in and a stream of midi out. Why you might ask ? Because in Reaper
for example, what one can do is use a little plugin called
 Reainsert that can nicely compensate for latency. It's another way of rooting midi. The editor doesn't talk
 direclty to the synth, it rather lets midi come in and go out,
 and then the daw can send the datas to the synth.

Basically I have the CTRLR system in mind ; CTRLR which is a free software on which one can create custom vst
 editors for instruments and fx.
We have a Thru option there, and other routing options that are super useful. I understand it might be a bit
complicated to implement at this point. It's just a thought.
 but if you follow my logic, it's a smart way to have perfectly in sync recorded instruments from the get go.
  In Reaper, using Reainsert, fiddling to get audio takes aligned
 onto the grid is a thing of the past as far as I'm concerned when editors I use allow me to handle midi
 communication with the gear via the Daw and not the editor itself.


COMPLETED
Last idea would be a mode to keep the monitor page always active on the synth itself. If used remotely, we
 don't need the infos of the screen
but a meter is a very interesting info to have while working with feedback, excessive drive or high resonances
 */


/*
 *
 * Build under JUCE 5.4.6 OR 5.4.7 locks up under windows until e.g. window is
 * re-sized. MIDI stops going on in and out!!!
 * Works ok with JUCE 5.4.5. This is either with standalone or plugin version.
 *
 */



/*
 *
 Enabling "link time optimisation" in projucer seems to fail linking entirely on Linux!
 so make sure link time optimisation is off.
 */

/*
Building on Linux JUCE 5.4.5 Jan 2020
v1.06


#JUCE 5.4.7
#On Win10 Run Producer (v 5.4.7).

#Producer global settings:
#Path to JUCE:
C:\Compilers\JUCE

#Set path to VST2
C:\Compilers\vstsdk365_12_11_2015_build_67
# Note that only the sub-folders
/pluginterfaces
# and
/public.sdk
# need to be present.

#Saving Producer file will create .sln file with the required setup.

****************
****************
****************

TO COMPILE ON WINDOWS:

****************
****************
****************
ENSURE STATIC BUILD IS SET IN PRODUCER FILE!

run x64 Native tools command prompt:
(type x64.. in the serach bar for WIn10)

reg.exe query "HKLM\SOFTWARE\Microsoft\MSBuild\ToolsVersions\4.0" /v MSBuildToolsPath
cd C:\Windows\Microsoft.NET\Framework64\v4.0.30319


PLUGIN VERSION
#On windows 10 to build plugin. First clean the paths.
msbuild.exe -m:2 /target:clean /p:Configuration=Release E:\Motas\Juce\MotasEditPlugin\ProducerFiles\Plugin\Builds\VisualStudio2017\MotasEditPlugin.sln

# then do the build!
msbuild.exe -m:2 /p:Configuration=Release E:\Motas\Juce\MotasEditPlugin\ProducerFiles\Plugin\Builds\VisualStudio2017\MotasEditPlugin.sln


EXE VERSION
#On windows 10 to build plugin. First clean the paths.
msbuild.exe -m:2 /target:clean /p:Configuration=Release Z:\Motas\Juce\MotasEditPlugin\ProducerFiles\Exe\Builds\VisualStudio2017\MotasEdit.sln

# then do the build, on multiple cores!
msbuild.exe -m:2 /m /p:Configuration=Release Z:\Motas\Juce\MotasEditPlugin\ProducerFiles\Exe\Builds\VisualStudio2017\MotasEdit.sln



// March 2021 on new machine:
 * install Visual Studio 2017 Community edition
~2GB download

JUCE 5.4.5.

VST stuff:
vstsdk365_12_11_2015_build_67


ENNSURE STATIC BUILD IS SET IN PRODUCER FILE!



run x64 Native tools command prompt:
RUN AS ADMINISTRATOR (if fails due to some random problem)
(type x64.. in the serach bar for WIn10)

reg.exe query "HKLM\SOFTWARE\Microsoft\MSBuild\ToolsVersions\4.0" /v MSBuildToolsPath
cd C:\Windows\Microsoft.NET\Framework64\v4.0.30319


PLUGIN VERSION
#On windows 10 to build plugin. First clean the paths.
msbuild.exe -m:2 /target:clean /p:Configuration=Release Z:\Motas\Juce\MotasEditPlugin\ProducerFiles\Plugin\Builds\VisualStudio2017\MotasEditPlugin.sln

# then do the build!
msbuild.exe -m:2 /p:Configuration=Release Z:\Motas\Juce\MotasEditPlugin\ProducerFiles\Plugin\Builds\VisualStudio2017\MotasEditPlugin.sln


EXE VERSION
#On windows 10 to build plugin. First clean the paths.
msbuild.exe -m:2 /target:clean /p:Configuration=Release Z:\Motas\Juce\MotasEditPlugin\ProducerFiles\Exe\Builds\VisualStudio2017\MotasEdit.sln

# then do the build!
msbuild.exe -m:2 /p:Configuration=Release Z:\Motas\Juce\MotasEditPlugin\ProducerFiles\Exe\Builds\VisualStudio2017\MotasEdit.sln

 *



****************
****************
****************


BUILD on macOS Catalina v10.15.2
Mac mini (Late 2012)
2.6GHz quad-Core Intel Core i7
16GB 1600MHz DDR3
1) copy MotasEditPlugin onto FAT Flash drive
2) Boot macOS machine (log in as Gareth first to decrypt disk)
3) Switch to Jon user
4) Copy MotasEditPlugin to 'Documents'
5) In 'Downloads' run Producer (in JUCE folder) and then launch XCode from there
6) (for plugin select VST/AU build choice on LHS) then choose Product-> "Archive" build
7) once built  press "distribute content", choose "Build products" and save into "MotasEdit_Builds_fromArchiveFunction"
which is in "Documents"
8) Use macOS to compress these products, along with release notes etc and then copy onto FLASH



****************
****************
****************



*/

/*
 *
 * To build on windows with VS 2017:
 * August 20 2018. On WIN10 PRO virtual machine
 * Using JUCE 5.3.2 (in C:\Compilers\JUCE)
 * msbuild.exe -m:2 /target:clean /p:Configuration=Release E:\Motas\Juce\MotasEditPlugin\ProducerFiles\Plugin\Builds\VisualStudio2017\MotasEditPlugin.sln
 *
 * To build Exe version:
 * C:\Program Files (x86)\Microsoft Visual Studio\2017\Community>msbuild.exe
 * -m:2  /p:Configuration=Release
 * E:Motas\Juce\MotasEditPlugin\ProducerFiles\Exe\Builds\VisualStudio2017\MotasEdit.sln
 */

/*
 *
 *
 * in ABLETON if plugin is built in DEBUG mode UI possibly locks up every so often!
 * Make sure built in release mode.
 *
 */


/*Nov 2020 Ubuntu 20.04
 *
 * sudo apt-get install freeglut3-dev g++ libasound2-dev libcurl4-openssl-dev            \
                     libfreetype6-dev libjack-jackd2-dev libx11-dev libxcomposite-dev \
                     libxcursor-dev libxinerama-dev libxrandr-dev mesa-common-dev

 */

/*
 *
 * To compile for Linux 32 bit (for e.g. Sony laptop) do
 * sudo apt install g++-multilib
 * sudo apt-get install libfreetype6-dev:i386
 * sudo apt-get install libwebkit2gtk-4.0-dev:i386
 * sudo apt-get install libxinerama-dev:i386
 * sudo apt-get install libxinerama-dev:i386
 * sudo apt-get install libxext-dev:i386
 *  can't get libcurl to sort out...
 * BOLLOCKS TO THIS, just compile on the target machine...
 */

/*
 *
 * Nov 2020
 * Build issues on Ubuntu 20.04 for standaline exe
 * Need to NOT set -O3 otherwise exe segfaults!
 * Size is 12MB unless also now set Link-Time optimisation = Enabled.
 * Also add Extra Linker Flags "-no-pie" to make final object an exe file!
 *
 *
 */


/*
 *
 * March 2021
 * download and install:
 * Build Tools for Visual Studio 2017 (version 15.9)
 * on win10
 *
 *
 */
/*
 *
 *
 * Changelog
 */
// v1.0.4 first public release.

// v1.0.5
// fixed bug in that load of single sequence did not fully load all bytes, causing pattern rejection on Motas.
// TO-DO add save of settings in plugin separately from 'plugin save' so e.g. colour is always remembered even if
// DAW does not load the settings (i.e. save to usual User directory for these settings)
// MIDI input on Windows Exe does not seem to allow simultaneous Motas and MPKMini for some reason... on virtual box or real WIN10 machine


// v1.0.6 / 1.07
	// added button for fixed monitor display on Motas
	// added handling of unipolar options for EG and modulators
	// added patch bank #4
	// rebuilt using later JUCE 5.4.5.
	// added check of firmware version during firmware update. Warning if version to update to is older.
	// added tooltip to patch combo box
	// added control of unipolar and pitch track LFO, pitch mode, EG shape.
	// convert from Patch version < v3
	// added send of data to DAW as well as direct to Motas


// v1.07 released on May 23 2020 for MacOS, commit  bf37e53cd768c3d6329d3939793293dceb38d5f4
// for firmware v0110


// v1.08 started 27 July 2020
	// added support for the latest 5 added LFO waveshapes
	// Added up/down/left/right to modify sliders. Added page up/down/home/end to navigate main pages.
	// Updated min Motas-6 version
	// Updated Bank numbering to start from 1


// v1.09 started Nov 2020
// added editing of parameter CC mapping options
// improved default file location (was file saved, should be folder)
// Added VCO2/3 infra- and ultra-sonic options
// Added MIDI channel option (previously was fixed at channel 1 for NRPN, channel 2 for SysEx)
// Fixed incorrect label on LPF1 resonance option



//v1.10 13 March 2020
// Fixed issue with multiple instances of plugin not detecting MIDI devices
// caused by having static variables in various places!
// Seems that creation of multiple DAW plugins creates new objects but not everything new from scratch.
// Added new trigger options for EGs to GUI

// v1.0.11 Apr 11 2021.
// Added compatibility with new patch version that now uses EG values for ADR stored 0..4095

// May 3 2021. Adding control of advanced modulation on a new tab
// This build on Linux with optimise -03 segfaults.... switched to optimise for sizer -s (as per the setting for the exe) and is ok!

// v1.0.12 February 2022
// added 2 new options for algorithms (as per firmware 0602011B)
// added new flexible MIDI data routing options
// Changed UI for new LFOx and LFO clk sync  (as per firmware 0602011C)
// Changed name of 2nd tab.


#define PATCH_IDENTIFIER ((uint32_t) 0x3561706D)
#define PATTERN_IDENTIFIER ((uint32_t) 0xF3559321)
#define SEQUENCE_IDENTIFIER ((uint32_t)  0xFE099312)

#define INCLUDE_TEST_FUNCTIONS 0 // set to 1 to add button to test lots of MIDI note on/off




#if BUILD_STANDALONE_EXECUTABLE == 0



// Set to 1 on 14 April 2020
// was turned off previously for some reason.
// This should allow plugin to send MIDI data out directly to DAW (not to Motas-6)
// This would allow (along with MIDI in from DAW) full in/out control via the DAW itself

	#define SEND_MIDI_TO_DAW_PROCESS_BLOCK 1

	#define PROCESS_MIDI_INCOMING_FROM_DAW_PROCESS_BLOCK 1
#else

	#define SEND_MIDI_TO_DAW_PROCESS_BLOCK 0
	#define PROCESS_MIDI_INCOMING_FROM_DAW_PROCESS_BLOCK 0
#endif

class ApplicationCommon
{
	public:
	ApplicationCommon();
	~ApplicationCommon();

	void saveAppPropertyString(int propertyEnum, String value);
	void saveAppPropertyInteger(int propertyEnum, int value);
	void saveAppPropertyBool(int propertyEnum, bool value);
#if BUILD_STANDALONE_EXECUTABLE == 1
	bool loadPresets();
	bool savePresets();
#endif
	String loadAppPropertyString(int propertyEnum);
	int loadAppPropertyInteger(int propertyEnum);
	bool loadAppPropertyBool(int propertyEnum);

	void saveSettings();
	enum
	{
		APP_PROPERTY_HIDE_NON_MOTAS_DEVICES,
		APP_PROPERTY_PASS_SYSEX,
		APP_PROPERTY_PLUGIN_GUI_SIZE,
		APP_PROPERTY_GUI_SIZE_X,
		APP_PROPERTY_GUI_SIZE_Y,
		APP_PROPERTY_BG_COLOUR,
		APP_PROPERTY_SCREENSHOT_COLOUR,
		APP_PROPERTY_DEFAULT_FILE_LOCATION,
		APP_PROPERTY_SHOW_TOOLTIPS,
		APP_PROPERTY_MIDI_THRU_ENABLED,
	//	APP_PROPERTY_MIDI_UPDATE_INTERVAL,
		APP_PROPERTY_MIDI_SEND_RATE,
		APP_PROPERTY_AUTO_CONNECT_MOTAS,
		APP_PROPERTY_LIVE_SCREENSHOT,
		APP_PROPERTY_ENABLE_AUTO_PAGE_CHANGE,
		APP_PROPERTY_PATCH_PRESET_NUMBER,
		APP_PROPERTY_ENABLE_SHOW_MONITOR_ONLY,


		APP_PROPERTY_MIDI_BASIC_CHANNEL,
		APP_PROPERTY_MIDI_ROUTING_BITFIELD,

	};


	bool isPatchValid(uint8_t* patch);
	void updatePatch(int index, uint8_t* buffer);
	void updateCurrentPatch(uint8_t* buffer);
	void storeEditedPatch(int index);
	void restoreEditedPatch(int index);

#if BUILD_STANDALONE_EXECUTABLE == 0
	std::unique_ptr<XmlElement> getPluginSettingsXML();

	void restorePluginState(XmlElement* xml);

#endif

	MemoryBlock& getPluginStateBinary();
	//void updateEntirePluginState(const void* data, int sizeInBytes);



	Array<uint8_t*> patchPresetsArray;
	Array<uint8_t*> patchPresetsEditedArray;


	void setMIDIProcessor(MIDIProcessor* m);



	String getPatchName(int index);
	String getCurrentPatchName();

	int getPatchNumber();
	void setPatchNumber(int newPatchNum);

	void changeCurrentPatchName(const String& newName);
	void changePatchName(int patchNum, const String& newName);
	void changePatchName(int patchNum, const uint8_t* newName);
	void changeActivePatchName(uint8_t* newName);
	bool isFirstTimeStartedGui();
	uint8_t* getActivePatch();
	tAllParams* getGUIPatch();
	tAllParams* getOriginalPatch();
	void setGUIPatch();
	bool isActivePatchEdited();

private:
	String fileNameAndPath;
	crcCalculator crc;
	String getPatchName(uint8_t* patch);

	bool aPatchHasChanged;
	bool originalMatchesEditedPatch;
	tAllParams guiPatch;

	uint8_t* activePatch; // pointer to a patch
	int activePatchNumber;
	bool firstTimeStartup;


	MIDIProcessor* midiProcessor;

	MemoryBlock memoryBlock;

#if BUILD_STANDALONE_EXECUTABLE == 1
	PropertiesFile::Options options;
	ApplicationProperties appProperties;
#else
	PropertySet appProperties;
#endif
	PropertySet* properties;

	bool prevMatchState;


};


#endif
