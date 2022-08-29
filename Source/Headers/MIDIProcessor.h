
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
 * MIDIComms.h
 *
 *
 */

#ifndef SOURCE_MIDI_PROCESSOR_H_

#define SOURCE_MIDI_PROCESSOR_H_
#pragma once
#include "ApplicationCommon.h"
#include "ParameterData.h"
#include "stdint.h"
#include "utilities.h"
#include "ApplicationCommon.h"
#include "MIDITransceiver.h"
class MotasPluginParameter;
class MIDIDeviceListBox;

#if BUILD_STANDALONE_EXECUTABLE == 0
	#include "PluginProcessor.h"
#endif



#if BUILD_STANDALONE_EXECUTABLE == 0
	class MotasEditPluginAudioProcessor;
#endif


class MIDITransceiver;




class HoldingWindowComponent;

class MIDIProcessor :  public MIDITransceiver, private Timer
{
public:

#if BUILD_STANDALONE_EXECUTABLE == 0
	MIDIProcessor(MotasEditPluginAudioProcessor* pluginProcessor);
#else

	MIDIProcessor(HoldingWindowComponent* holdingWindow);
	HoldingWindowComponent* holdingWindow;
#endif

	~MIDIProcessor();
	void populatePatch(uint8_t* buffer);
	//void sendPatch(uint8_t preset, uint8_t* patchBuffer);

	uint32_t getMIDISendRate();



#if BUILD_STANDALONE_EXECUTABLE == 0
	MotasPluginParameter* getDAWParameter(int page, int param);

#endif

	MemoryBlock& getEntirePluginState();
	void updateEntirePluginState(const void* data, int sizeInBytes);
	void updateMIDISettings();


	bool commandRequest(int commandNum, uint8_t* dataBuffer, int size);
	bool commandRequest(int commandNum, NRPNmessage_t& nrpn);
	bool commandRequest(int commandNum, uint8_t value);

	bool isBusySendingGuiCommand;
	bool isBusyReceivingGuiCommand;


	void  informDAW(NRPNmessage_t& msg);

	void indicatePatchDataChanged();



	enum
	{
		COMMAND_REQUEST_NOTHING,
		COMMAND_REQUEST_SEND_FIRMARE,
		COMMAND_REQUEST_GET_BULK,
		COMMAND_REQUEST_SEND_BULK,
		COMMAND_REQUEST_GET_INFO,
		COMMAND_REQUEST_STATUS,
		COMMAND_REQUEST_SEND_GLOBALS,
		COMMAND_REQUEST_GET_GLOBALS,

		COMMAND_REQUEST_PATCH,
		COMMAND_REQUEST_PATCH_BANK,
		COMMAND_REQUEST_PATTERN,
		COMMAND_REQUEST_PATTERN_BANK,
		COMMAND_REQUEST_SEQUENCE,
		COMMAND_REQUEST_SEQUENCE_BANK,

		COMMAND_REQUEST_SEND_ACTIVE_PATCH,

		COMMAND_REQUEST_SEND_GUI_NOTES_CONTROLLERS,

		COMMAND_REQUEST_SEND_PROGRAM_CHANGE,

		COMMAND_REQUEST_RESET_PARAMETER_PAGE,
		COMMAND_REQUEST_PARAMETER_CHANGED,
		COMMAND_REQUEST_GUI_UPDATE,

		COMMAND_REQUEST_PATCH_CHANGE,
		COMMAND_REQUEST_PATCH_NAME_CHANGE,
		COMMAND_REQUEST_TEST_MIDI,
	//	COMMAND_REQUEST_PARAMETER_CHANGED_MOTAS_ONLY,


	};

	enum
	{
		ACTION_FOR_GUI_NOTHING,
	//	ACTION_FOR_GUI_POPULATE_PATCH,
		ACTION_FOR_GUI_PATCH,
		ACTION_FOR_GUI_PATCH_BANK,
		ACTION_FOR_GUI_GLOBALS_TRANSFER,
		ACTION_FOR_GUI_INFO_TRANSFER,
		ACTION_FOR_GUI_BULK_TRANSFER,
		ACTION_FOR_GUI_SCREENSHOT,
		ACTION_FOR_GUI_SEQUENCE,
		ACTION_FOR_GUI_PATTERN,
		ACTION_FOR_GUI_PATTERN_BANK,
		ACTION_FOR_GUI_SEQUENCES_BANK,
		ACTION_FOR_GUI_FIRMWARE_VERSION_REPORTED

	};





	void resetParameterInPatch(NRPNmessage_t& msg);
private:




	bool retry = false;
	uint32_t timeLastRequestSent = 0;
	int lastActiveTab = -1;


	int debugThin ;
	uint32_t prevTimeStamp;

	//void sendToOutgoingFIFOToDAW(const MidiBuffer& buf);

	void updateGUIRequest();

	Utilities utilities;
	uint8_t activePatchPreset; // from 0 to 4


	bool dataFromGUI;


	int activeCommand;
	int activeCommandToGui;



	volatile bool patchDataEdited;
	volatile bool dataChanged;
	volatile bool guiNeedsUpdate;
	bool changesFromGUI;
	volatile bool guiBusyUpdating;
	void run() override;


	int threadFunction();
	CriticalSection criticalSection;
	bool liveScreenshotEnabled;
	bool autoPageChange;
	bool showOnlyMonitor;
	//int deviceCheckDivider;
	//bool isParameterDataFromDAW;

	volatile int randomNumber;
	uint8_t midiInBuffer[1024];
	uint8_t midiInDAWParametersBuffer[1024];
	uint8_t midiInFromMotasBuffer[fifoMidiInFromMotas_Size];
#if BUILD_STANDALONE_EXECUTABLE == 1
	uint8_t midiInFromOthersBuffer[fifoMidiInFromOthers_Size];
#endif
	uint8_t notesControllersFromGUIBuffer[fifoMidiFromGUI_Size];
	uint8_t paramChangesFromGUIBuffer[fifoParamChangesFromGUI_Size];
	uint8_t bitmapBuffer[64*64];
	uint8_t paramChangesFromMotasToDAWBuffer[fifoParamChangesFromMotasToDAW_Size];


	//void processNRPN(const uint8_t* data, NRPNmessage_t& NRPNMessage, NRPNinit_t &NRPNInitialised);
	void processSysExMIDIdata(uint8_t type,  Array<uint8_t>* midiStreamData);

	void processMIDIControllerChangeMessage(NRPNmessage_t* msg);



	void doTasks();

	//void updateMIDIReceiveSysExByte(uint8_t data);

	HoldingWindowComponent* getHoldingWindow();
	ApplicationCommon* getApplicationCommon();
	void doSlowerTasks();
	void timerCallback() override;

	void sysExDoProcessing();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MIDIProcessor)

};





#endif /* SOURCE_MIDICOMMSNoUI_H_ */
