

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
 * MIDIComms.cpp
 *
 *
 */
#include "./Headers/MIDIProcessor.h"

#include "./Headers/utilities.h"
#include "./Headers/crc.h"
#include "./Headers/MainComponent.h"
#include "./Headers/PatchPatternSequence.h"
#include "./Headers/HoldingWindowComponent.h"
#include "./Headers/Firmware.h"
#include "../binaryResources/MotasEditResources.h"
#include "Headers/MotasPluginParameter.h"
#include "Headers/PanelControl.h"
#include "Headers/MIDIComms.h"
#include "Headers/MIDIConnection.h"
#include "Headers/MIDITransceiver.h"
#include "Headers/advancedMod.h"

#if BUILD_STANDALONE_EXECUTABLE == 0
	#include "Headers/PluginProcessor.h"
#endif
/**
 *
 * Deals with the MIDI communication stuff
 */


// normally set to 1 to have separate worker thread
#define RUN_SEPARATE_THREAD 1


#if RUN_SEPARATE_THREAD == 1
	static const int timerUpdatePeriodMS = 40;
	static const int workerThreadPriority = 7;
#else
	static const int timerUpdatePeriodMS = 4;
#endif


#if BUILD_STANDALONE_EXECUTABLE == 0
	MIDIProcessor::MIDIProcessor(MotasEditPluginAudioProcessor* p):
		MIDITransceiver(p)

#else
	  MIDIProcessor::MIDIProcessor(HoldingWindowComponent* holdingWindow)
	: holdingWindow(holdingWindow)
#endif

{
		//deviceCheckDivider  = 10;
	activePatchCRC32 = 0;
	//activePatchName = "";
	changesFromGUI = false;
	dataChanged = false;
	patchDataEdited = false;
	dataFromGUI = false;

	activePatchPreset = 0;
	isBusySendingGuiCommand = false;
	isBusyReceivingGuiCommand = false;
	activeCommand = 0;
	activeCommandToGui = 0;
	guiNeedsUpdate = false;
	guiBusyUpdating = false;
	liveScreenshotEnabled = false;
	autoPageChange = false;
	showOnlyMonitor = false;
	//isParameterDataFromDAW = false;

	jassert(sizeof(tAllParams) == 4096);
	jassert(sizeof(singleParam) == 108);
	jassert(sizeof(tCommonLFO) == 8);
	jassert(sizeof(tEGCore) == 20);
	jassert(sizeof(tArpegSettings) == 8);
	jassert(sizeof(tArpegChordSettings) == 18);
	jassert(sizeof(tLFOAmount) == 8);
	jassert(sizeof(tStorageLFO) == 8);
	jassert(sizeof(tStorageMIDI) == 6);
	jassert(sizeof(tEGAmount) == 8);
	prevTimeStamp = 0;
	debugThin = 0;


	retry = false;
	timeLastRequestSent = 0;
	lastActiveTab = -1;



	DBG(String("create timer..") + String(" this pointer: ") +  String((uint64) this));
	Timer::startTimer(timerUpdatePeriodMS);
#if RUN_SEPARATE_THREAD == 1
	startThread(workerThreadPriority);
#endif

}



/**
 *
 * This function is called when the thread starts
 */
void MIDIProcessor::run()
{
	//static uint32_t counter = 0;


	// threadShouldExit() returns true when the stopThread() method has been
    // called, so we should check it often, and exit as soon as it gets flagged.
	while (!threadShouldExit())
	{
		int delay = threadFunction();
		wait(delay);


/*
		counter++;

		if ((counter % 10) == 0)
		{
			DBG("thread run..." + String((uint64_t) this));
			counter = 0;
		}*/
	}
}


ApplicationCommon* MIDIProcessor::getApplicationCommon()
{
	#if BUILD_STANDALONE_EXECUTABLE == 0
		return this->pluginProcessor->appCommonPtr;
	#else
		return this->holdingWindow->appCommon.get();
	#endif
}


HoldingWindowComponent* MIDIProcessor::getHoldingWindow()
{
	#if BUILD_STANDALONE_EXECUTABLE == 0
		return pluginProcessor->getActiveHoldingWindow();
	#else
		return holdingWindow;
	#endif
}


/**
 *
 *
 * Processes SysEx from Motas e.g. screenshot, patches etc
 * Called in worker thread.
 */
void MIDIProcessor::processSysExMIDIdata(uint8_t type,  Array<uint8_t>* midiStreamData)
{
	//String msg = "Process MIDI data";
	int32_t len = midiStreamData->size();
	if (isBusyReceivingGuiCommand || len < 16)
	{
		if (len < 16)
		{
			DBG("SysEx size too small: " + String(len));
		}
		midiStreamData->clear(); // clear it ready for new data in the future
		return;
	}
	activeCommandToGui = ACTION_FOR_GUI_NOTHING;
	bool guiUpdateNeeded = true;

    incomingDataArray.clear();
    for (int32_t i = 0; i < len; i++)
    {
    	incomingDataArray.add(midiStreamData->getUnchecked(i));
    }
    uint8_t* buffer = incomingDataArray.getRawDataPointer();

    midiStreamData->clear(); // clear it ready for new data in the future
    uint32_t convertedLength = utilities.convertRawMIDI(buffer, (uint32_t) len);
    uint32_t expectedLength = 0;

    switch (type)
    {
		case MIDI_SYSEX_STATUS_TRANSFER:
			expectedLength = 16;
			break;
        case MIDI_SYSEX_BITMAP_TRANSFER:
            expectedLength = 4096;
            break;
        case MIDI_SYSEX_BITMAP_COMPRESSED_TRANSFER:
        	expectedLength = 0;
        	break;
        case  MIDI_SYSEX_INFO_TRANSFER:
            expectedLength = 256;
            break;
    //  case MIDI_SYSEX_OSCILLOSCOPE_TRANSFER:
   //       expectedLength = OSCILLOSCOPE_TRACE_NUM_POINTS*2;
   //       break;
        case MIDI_SYSEX_REQUEST_BULK_FLASH_DATA_SEND:
            expectedLength = MASS_FLASH_TRANSFER_LENGTH_BYTES;
            break;
        case MIDI_SYSEX_PATCH_TRANSFER:
        	DBG("PATCH data received");
            expectedLength = PATCH_LENGTH_BYTES_UNPACKED;
            break;
        case MIDI_SYSEX_PATCH_BANK_TRANSFER:
            expectedLength  = BANK_OF_PATCHES_LENGTH * PATCH_LENGTH_BYTES_UNPACKED;
            break;
        case MIDI_SYSEX_PATTERN_TRANSFER:
            expectedLength  = PATTERN_LENGTH_BYTES_UNPACKED;
            break;
        case MIDI_SYSEX_PATTERN_BANK_TRANSFER:
            expectedLength  = BANK_OF_PATTERNS_LENGTH * PATTERN_LENGTH_BYTES_UNPACKED;
            break;
        case MIDI_SYSEX_SEQUENCE_TRANSFER:
            expectedLength  =  SEQUENCE_LENGTH_BYTES_UNPACKED;
            break;
        case MIDI_SYSEX_SEQUENCE_BANK_TRANSFER:
            expectedLength  = BANK_OF_SEQUENCES_LENGTH * SEQUENCE_LENGTH_BYTES_UNPACKED;
            break;
        case MIDI_SYSEX_GLOBALS_TRANSFER:
        	DBG("Setup global data received");
            expectedLength = SETUP_DATA_LENGTH_BYTES;
            break;
        default:
            return;
            break;
    }

    if (type != MIDI_SYSEX_BITMAP_COMPRESSED_TRANSFER)
    {
    	expectedLength += 4;// add on the CRC bytes
		if (convertedLength != expectedLength)
		{
			String msg = "Incorrect sysex length: "  + String(convertedLength) + " Expected: " + String(expectedLength);
			sendMessageToHw(msg, HoldingWindowComponent::MESSAGE_TYPE_FROM_MOTAS);
			DBG("ERROR unexpected sysex length: "  + String(convertedLength));
			return;
		}
    } else
    {
    	DBG("bitmap length: "  +  String(convertedLength));
    }
    // calculate CRC
    uint32_t CRC32  = Utilities::read32((int) convertedLength - 4, buffer);
    // offset of 8 to miss the CRC and identifier bytes
    uint32_t crcVal = crcCalculator::crcFastSTMF4(buffer, convertedLength - 4); // calculate CRC of all data (excluding first 8 bytes, and final 4)

    if (CRC32 == crcVal)
    {
    //	DBG("CRC ok, type : " + String(type));
    	switch(type)
    	{

    	case MIDI_SYSEX_PATTERN_TRANSFER:
			sendMessageToHw(String("Received pattern"), HoldingWindowComponent::MESSAGE_TYPE_FROM_MOTAS);
			activeCommandToGui = ACTION_FOR_GUI_PATTERN;
			break;
		case MIDI_SYSEX_PATTERN_BANK_TRANSFER:
			sendMessageToHw(String("Received pattern bank"), HoldingWindowComponent::MESSAGE_TYPE_FROM_MOTAS);
			activeCommandToGui = ACTION_FOR_GUI_PATTERN_BANK;
			break;
		case MIDI_SYSEX_SEQUENCE_TRANSFER:
			sendMessageToHw(String("Received sequence"), HoldingWindowComponent::MESSAGE_TYPE_FROM_MOTAS);
			activeCommandToGui = ACTION_FOR_GUI_SEQUENCE;
			break;
		case MIDI_SYSEX_SEQUENCE_BANK_TRANSFER:
			sendMessageToHw(String("Received sequence bank"), HoldingWindowComponent::MESSAGE_TYPE_FROM_MOTAS);
			activeCommandToGui = ACTION_FOR_GUI_SEQUENCES_BANK;
			break;
		case MIDI_SYSEX_REQUEST_BULK_FLASH_DATA_SEND:
			sendMessageToHw(String("Received bulk data"), HoldingWindowComponent::MESSAGE_TYPE_FROM_MOTAS);
			activeCommandToGui = ACTION_FOR_GUI_BULK_TRANSFER;
			break;
		case MIDI_SYSEX_GLOBALS_TRANSFER:
			sendMessageToHw(String("Received setup data"), HoldingWindowComponent::MESSAGE_TYPE_FROM_MOTAS);
			activeCommandToGui = ACTION_FOR_GUI_GLOBALS_TRANSFER;
			break;
		case MIDI_SYSEX_INFO_TRANSFER:
		{

			sendMessageToHw(String("Received Motas info"), HoldingWindowComponent::MESSAGE_TYPE_FROM_MOTAS);
			activeCommandToGui = ACTION_FOR_GUI_INFO_TRANSFER;
			break;
		}
		case MIDI_SYSEX_BITMAP_TRANSFER:
			sendMessageToHw(String("Received screenshot (uncompressed)"), HoldingWindowComponent::MESSAGE_TYPE_FROM_MOTAS);
			activeCommandToGui = ACTION_FOR_GUI_SCREENSHOT;
			break;
		case MIDI_SYSEX_STATUS_TRANSFER:
		{
			tSysexStatus* status = (tSysexStatus*) buffer;

			if (activePatchCRC32 != status->data.lastSinglePatchCRC32 && retryPatchSendCount < 5)
			{
				// patch on Motas doesn't match what we have here, send update
				//commandRequest(MIDIProcessor::COMMAND_REQUEST_SEND_ACTIVE_PATCH, 0);
				retryPatchSendCount++;
	//			DBG("CRC WRONG - \n\n\nactivePatchCRC32: " + String(activePatchCRC32) + " motas crc: " + String(status->data.lastSinglePatchCRC32));
	    		DBG("\nRE-SEND PATCH");
			} else
			{
				retryPatchSendCount = 0;
	//			DBG("\n\n\nactivePatchCRC32: " + String(activePatchCRC32) + " motas crc: " + String(status->data.lastSinglePatchCRC32));
			}

			sendMessageToHw(String("Received status"), HoldingWindowComponent::MESSAGE_TYPE_FROM_MOTAS);

			activeCommandToGui = ACTION_FOR_GUI_FIRMWARE_VERSION_REPORTED;
			break;
		}
		case MIDI_SYSEX_BITMAP_COMPRESSED_TRANSFER:
			sendMessageToHw(String("Received screenshot"), HoldingWindowComponent::MESSAGE_TYPE_FROM_MOTAS);
			len = utilities.runLengthDecode(buffer, (uint16_t) (convertedLength - 4), this->bitmapBuffer, 4096);
			if (len == 4096)
			{
				activeCommandToGui = ACTION_FOR_GUI_SCREENSHOT;
			} else
			{
				String msg = "bitmap  - incorrect num of byes decoded: "  + String(len);
				sendMessageToHw(msg, HoldingWindowComponent::MESSAGE_TYPE_FROM_MOTAS);
				//DBG("incorrect num of byes decoded: "  + String(len));
			}
			break;
		case MIDI_SYSEX_OSCILLOSCOPE_TRANSFER:
			break;
		default:
			break;
    	}
        HoldingWindowComponent* hw = getHoldingWindow();
        if (hw != nullptr)
        {
			switch (hw->getActiveTabIndex())
			{
				default:
					break;
				case TAB_INDEX_EDIT: // edit tab
				{
					switch (type)
					{
						case MIDI_SYSEX_PATCH_TRANSFER:
							sendMessageToHw(String("Received patch"), HoldingWindowComponent::MESSAGE_TYPE_FROM_MOTAS);
							//this->populatePatch(buffer);
							//activeCommandToGui = ACTION_FOR_GUI_POPULATE_PATCH;
							populatePatch(buffer);
							break;
					}
					break;
				}
				case TAB_INDEX_PATCH_PATTERN: // patch/patterns tab
				{
					switch (type)
					{
						case MIDI_SYSEX_PATCH_TRANSFER:
							DBG("Received patch data ACTION_FOR_GUI_PATCH");
							sendMessageToHw(String("Received patch"), HoldingWindowComponent::MESSAGE_TYPE_FROM_MOTAS);
							activeCommandToGui = ACTION_FOR_GUI_PATCH;
							break;
						case MIDI_SYSEX_PATCH_BANK_TRANSFER:
							sendMessageToHw(String("Received patch bank"), HoldingWindowComponent::MESSAGE_TYPE_FROM_MOTAS);
							activeCommandToGui = ACTION_FOR_GUI_PATCH_BANK;
							break;
					}
					break;
				}
			}
        }
        isBusyReceivingGuiCommand = guiUpdateNeeded;
    } else
    {
    	String msg = "CRC error!";
		sendMessageToHw(msg, HoldingWindowComponent::MESSAGE_TYPE_FROM_MOTAS);

    	DBG("CRC error!");
    }
    if (type != MIDI_SYSEX_BITMAP_TRANSFER
    		&& type != MIDI_SYSEX_BITMAP_COMPRESSED_TRANSFER
			&& type != MIDI_SYSEX_STATUS_TRANSFER)
	{
		busySendingSysEx = false;
	}
}




// MUST provide the destructor if we specified it in the header file declaration otherwise get linker error.
MIDIProcessor::~MIDIProcessor()
{

	// allow the thread 2 seconds to stop cleanly - should be plenty of time.
	/* If your thread class's destructor has been called without first stopping the thread, that
	       means that this partially destructed object is still performing some work - and that's
	       probably a Bad Thing!

	       To avoid this type of nastiness, always make sure you call stopThread() before or during
	       your subclass's destructor.
	    */
    stopThread (2000);
    DBG(String("~MIDIProcessor..") + String(" this pointer: ") +  String((uint64) this));
}


static int8_t remapShape (int8_t val)
{
	if (val == 0)
		val = 32;
	else if (val == 1)
		val = 0;
	else
		val = -32;
	return val;
}


#if BUILD_STANDALONE_EXECUTABLE == 0

void  MIDIProcessor::informDAW(NRPNmessage_t& msg)
{
#else
	void  MIDIProcessor::informDAW(NRPNmessage_t& )
	{
#endif
#if BUILD_STANDALONE_EXECUTABLE == 0

	MotasPluginParameter* m = getDAWParameter(msg.parameterMSB, msg.parameterLSB);
	// ***************
	// notify the DAW host that we have changed a parameter
	if (m != nullptr)
	{
		m->beginChangeGesture();
		m->setValueInternal(msg.valueMSB, msg.valueLSB);
		m->endChangeGesture();

	} else
	{
		DBG("Parameter not found!");
	}
#endif

}


/*
 *
 *  Updates the active patch with the new NRPN data (which could be from internal parameter changes on the GUI)
 *
 *
 *
 */
void  MIDIProcessor::processMIDIControllerChangeMessage(NRPNmessage_t* msg)
{


	bool refreshAllGUISettings = false;
	singleParam*  p = nullptr;

#if BUILD_STANDALONE_EXECUTABLE == 0
	tAllParams* patch = (tAllParams*) pluginProcessor->appCommonPtr->getActivePatch();//gPatchInUse; // pointer to patch in use
#else
	tAllParams* patch;
	if (getHoldingWindow() != nullptr)
	{
		patch = (tAllParams*) getHoldingWindow()->appCommon->getActivePatch();//gPatchInUse; // pointer to patch in use
	} else
	{
		return;
	}
#endif

	//DBG("Process MIDI NRPN message");
/*	String infoMsg = "Process NRPN data: " + String((int)msg->parameterMSB)
											+ " " +  String((int)msg->parameterLSB)
											+ " " + String((int)msg->valueMSB)
											+ " " +  String((int)msg->valueLSB) ;
	*/
	int page = -1;
	if (msg->parameterMSB == NRPN_MSB_GLOBAL_ADVANCED_MODULATION)
	{
		uint8_t amodSlot = (msg->parameterLSB >> 3) & 0x0F; // choice of 16
		uint8_t amodParameter = msg->parameterLSB & 0x07; // up to 8 options

		if (amodSlot < 16)
		{
			t_modOfMod* mod = &patch->params.singleParams[amodSlot].more.lfo2Mod_And_ModofMod.modOfmodulation;

			switch (amodParameter)
			{
				default: // do nothing
					break;
				case AMOD_NRPN_TYPE_SOURCE1:
					mod->source1Page = msg->valueMSB;
					mod->source1Param = msg->valueLSB;
					break;
				case AMOD_NRPN_TYPE_SOURCE2:
					mod->source2Page = msg->valueMSB;
					mod->source2Param = msg->valueLSB;
					break;
				case AMOD_NRPN_TYPE_DEST:
					mod->destinationPage = msg->valueMSB;
					mod->destinationParam = msg->valueLSB;
					break;
				case AMOD_NRPN_TYPE_ALGORITHM:
					mod->algorithm = msg->valueLSB;
					break;
				case AMOD_NRPN_TYPE_GAIN:
					mod->gain = (((uint16_t) msg->valueMSB << 7) | msg->valueLSB) - 8192L;
					break;
				case AMOD_NRPN_TYPE_UNIPOLAR:
					mod->unipolar = msg->valueLSB;
					break;
				case AMOD_NRPN_TYPE_MODE:
					patch->advancedModulationMode = msg->valueLSB;
					break;
			}

		}
	}
	else if (msg->parameterMSB == 0) // global setting
	{
		switch (msg->parameterLSB)
		{
			case NRPN_VALUE_GLOBAL_MOD1_SOURCE:
				patch->MOD1Source = (uint8_t) Utilities::checkNRPNValue(0, MAX_MIDI_CONTROL_VALUE, msg);
				break;
			case NRPN_VALUE_GLOBAL_MOD2_SOURCE:
				patch->MOD2Source = (uint8_t) Utilities::checkNRPNValue(0, MAX_MIDI_CONTROL_VALUE, msg);
				break;
			case NRPN_VALUE_GLOBAL_MOD3_SOURCE:
				patch->MOD3Source = (uint8_t) Utilities::checkNRPNValue(0, MAX_MIDI_CONTROL_VALUE, msg);
				break;
			case NRPN_VALUE_GLOBAL_MOD4_SOURCE:
				patch->MOD4Source = (uint8_t) Utilities::checkNRPNValue(0, MAX_MIDI_CONTROL_VALUE, msg);
				break;
			case NRPN_VALUE_GLOBAL_LFO1_FREQ:
				patch->commonLFOs[0].freq = (int16_t) Utilities::checkNRPNValue(-33, 4095, msg);
				break;
			case NRPN_VALUE_GLOBAL_LFO2_FREQ:
				patch->commonLFOs[1].freq = (int16_t) Utilities::checkNRPNValue(-33, 4095, msg);
				break;
			case NRPN_VALUE_GLOBAL_LFO3_FREQ:
				patch->commonLFOs[2].freq = (int16_t) Utilities::checkNRPNValue(-33, 4095, msg);
				break;
			case NRPN_VALUE_GLOBAL_LFO4_FREQ:
				patch->commonLFOs[3].freq = (int16_t) Utilities::checkNRPNValue(-33, 4095, msg);
				break;
			case NRPN_VALUE_GLOBAL_LFO1_TRIGGER:
				patch->commonLFOs[0].trigger = (uint8_t) Utilities::checkNRPNValue(0, LFO_TRIGGER_MAX_STATE - 1, msg);
				break;
			case NRPN_VALUE_GLOBAL_LFO2_TRIGGER:
				patch->commonLFOs[1].trigger = (uint8_t) Utilities::checkNRPNValue(0, LFO_TRIGGER_MAX_STATE - 1, msg);
				break;
			case NRPN_VALUE_GLOBAL_LFO3_TRIGGER:
				patch->commonLFOs[2].trigger = (uint8_t) Utilities::checkNRPNValue(0, LFO_TRIGGER_MAX_STATE - 1, msg);
				break;
			case NRPN_VALUE_GLOBAL_LFO4_TRIGGER:
				patch->commonLFOs[3].trigger = (uint8_t) Utilities::checkNRPNValue(0, LFO_TRIGGER_MAX_STATE - 1, msg);
				break;

			case NRPN_VALUE_GLOBAL_LFO1_SINGLE_SHOT:
				patch->commonLFOs[0].extras.singleShot = (uint8_t) Utilities::checkNRPNValue(0, 1, msg);
				break;
			case NRPN_VALUE_GLOBAL_LFO2_SINGLE_SHOT:
				patch->commonLFOs[1].extras.singleShot = (uint8_t) Utilities::checkNRPNValue(0, 1, msg);
				break;
			case NRPN_VALUE_GLOBAL_LFO3_SINGLE_SHOT:
				patch->commonLFOs[2].extras.singleShot = (uint8_t) Utilities::checkNRPNValue(0, 1, msg);
				break;
			case NRPN_VALUE_GLOBAL_LFO4_SINGLE_SHOT:
				patch->commonLFOs[3].extras.singleShot = (uint8_t) Utilities::checkNRPNValue(0, 1, msg);
				break;
			case NRPN_VALUE_GLOBAL_LFO1_WAVEFORM:
				patch->commonLFOs[0].waveformType = (uint8_t) Utilities::checkNRPNValue(0, LFO1_MAX_CHOICE_VALUE, msg);

				break;
			case NRPN_VALUE_GLOBAL_LFO2_WAVEFORM:
				patch->commonLFOs[1].waveformType = (uint8_t) Utilities::checkNRPNValue(0, LFO1_MAX_CHOICE_VALUE, msg);
				break;
			case NRPN_VALUE_GLOBAL_LFO3_WAVEFORM:
				patch->commonLFOs[2].waveformType = (uint8_t) Utilities::checkNRPNValue(0, LFO1_MAX_CHOICE_VALUE, msg);
				break;
			case NRPN_VALUE_GLOBAL_LFO4_WAVEFORM:
				patch->commonLFOs[3].waveformType = (uint8_t) Utilities::checkNRPNValue(0, LFO1_MAX_CHOICE_VALUE, msg);
				break;

			case NRPN_VALUE_GLOBAL_LFO1_PITCH_TRACK:
				patch->commonLFOs[0].extras.pitchTrack = (uint8_t) Utilities::checkNRPNValue(0, LFO_MODE_MAX-1, msg);
				break;
			case NRPN_VALUE_GLOBAL_LFO2_PITCH_TRACK:
				patch->commonLFOs[1].extras.pitchTrack = (uint8_t) Utilities::checkNRPNValue(0, LFO_MODE_MAX-1, msg);
				break;
			case NRPN_VALUE_GLOBAL_LFO3_PITCH_TRACK:
				patch->commonLFOs[2].extras.pitchTrack = (uint8_t) Utilities::checkNRPNValue(0, LFO_MODE_MAX-1, msg);
				break;
			case NRPN_VALUE_GLOBAL_LFO4_PITCH_TRACK:
				patch->commonLFOs[3].extras.pitchTrack = (uint8_t) Utilities::checkNRPNValue(0, LFO_MODE_MAX-1, msg);
				break;



			case NRPN_VALUE_GLOBAL_EG1_TRIGGER:
				patch->commonEGs[0].egRetrig = (uint8_t) Utilities::checkNRPNValue(0, EG_RETRIG_MAX, msg);
				break;
			case NRPN_VALUE_GLOBAL_EG2_TRIGGER:
				patch->commonEGs[1].egRetrig = (uint8_t) Utilities::checkNRPNValue(0, EG_RETRIG_MAX, msg);
				break;
			case NRPN_VALUE_GLOBAL_EG3_TRIGGER:
				patch->commonEGs[2].egRetrig = (uint8_t) Utilities::checkNRPNValue(0, EG_RETRIG_MAX, msg);
				break;
			case NRPN_VALUE_GLOBAL_EG4_TRIGGER:
				patch->commonEGs[3].egRetrig = (uint8_t) Utilities::checkNRPNValue(0, EG_RETRIG_MAX, msg);
				break;

			case NRPN_VALUE_GLOBAL_EG1_DELAY:
				patch->commonEGs[0].delay = (uint16_t) Utilities::checkNRPNValue(0, 4095, msg);
				break;
			case NRPN_VALUE_GLOBAL_EG2_DELAY:
				patch->commonEGs[1].delay = (uint16_t) Utilities::checkNRPNValue(0, 4095, msg);

				break;
			case NRPN_VALUE_GLOBAL_EG3_DELAY:
				patch->commonEGs[2].delay = (uint16_t) Utilities::checkNRPNValue(0, 4095, msg);
				break;
			case NRPN_VALUE_GLOBAL_EG4_DELAY:
				patch->commonEGs[3].delay = (uint16_t) Utilities::checkNRPNValue(0, 4095, msg);
				break;


			case NRPN_VALUE_GLOBAL_EG1_ATTACK:
				if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
					patch->commonEGs[0].attack = Utilities::checkNRPNValue(0, 4095, msg);
				else
					patch->commonEGs[0].attack = (int32_t) Utilities::convertEGValueToTime(Utilities::checkNRPNValue(0, 4095, msg));
				break;
			case NRPN_VALUE_GLOBAL_EG2_ATTACK:
				if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
					patch->commonEGs[1].attack = Utilities::checkNRPNValue(0, 4095, msg);
				else
					patch->commonEGs[1].attack = (int32_t)Utilities::convertEGValueToTime(Utilities::checkNRPNValue(0, 4095, msg));
				break;
			case NRPN_VALUE_GLOBAL_EG3_ATTACK:
				if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
					patch->commonEGs[2].attack = Utilities::checkNRPNValue(0, 4095, msg);
				else
					patch->commonEGs[2].attack = (int32_t)Utilities::convertEGValueToTime(Utilities::checkNRPNValue(0, 4095, msg));
				break;
			case NRPN_VALUE_GLOBAL_EG4_ATTACK:
				if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
					patch->commonEGs[3].attack = Utilities::checkNRPNValue(0, 4095, msg);
				else
					patch->commonEGs[3].attack = (int32_t)Utilities::convertEGValueToTime(Utilities::checkNRPNValue(0, 4095, msg));
				break;



			case NRPN_VALUE_GLOBAL_EG1_DECAY:
				if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
					patch->commonEGs[0].decay = Utilities::checkNRPNValue(0, 4095, msg);
				else
					patch->commonEGs[0].decay = (int32_t)Utilities::convertEGValueToTime(Utilities::checkNRPNValue(0, 4095, msg));
				break;
			case NRPN_VALUE_GLOBAL_EG2_DECAY:
				if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
					patch->commonEGs[1].decay = Utilities::checkNRPNValue(0, 4095, msg);
				else
					patch->commonEGs[1].decay = (int32_t)Utilities::convertEGValueToTime(Utilities::checkNRPNValue(0, 4095, msg));
				break;
			case NRPN_VALUE_GLOBAL_EG3_DECAY:
				if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
					patch->commonEGs[2].decay = Utilities::checkNRPNValue(0, 4095, msg);
				else
					patch->commonEGs[2].decay = (int32_t)Utilities::convertEGValueToTime(Utilities::checkNRPNValue(0, 4095, msg));
				break;
			case NRPN_VALUE_GLOBAL_EG4_DECAY:
				if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
					patch->commonEGs[3].decay = Utilities::checkNRPNValue(0, 4095, msg);
				else
					patch->commonEGs[3].decay = (int32_t)Utilities::convertEGValueToTime(Utilities::checkNRPNValue(0, 4095, msg));
				break;


			case NRPN_VALUE_GLOBAL_EG1_SUSTAIN:
				patch->commonEGs[0].sustain = (uint16_t) Utilities::checkNRPNValue(0, 4095, msg);
				break;
			case NRPN_VALUE_GLOBAL_EG2_SUSTAIN:
				patch->commonEGs[1].sustain = (uint16_t) Utilities::checkNRPNValue(0, 4095, msg);
				break;
			case NRPN_VALUE_GLOBAL_EG3_SUSTAIN:
				patch->commonEGs[2].sustain = (uint16_t) Utilities::checkNRPNValue(0, 4095, msg);


				break;
			case NRPN_VALUE_GLOBAL_EG4_SUSTAIN:
				patch->commonEGs[3].sustain = (uint16_t) Utilities::checkNRPNValue(0, 4095, msg);
				break;


			case NRPN_VALUE_GLOBAL_EG1_RELEASE:
				if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
					patch->commonEGs[0].release = Utilities::checkNRPNValue(0, 4095, msg);
				else
					patch->commonEGs[0].release = (int32_t)Utilities::convertEGValueToTime(Utilities::checkNRPNValue(0, 4095, msg));
				break;
			case NRPN_VALUE_GLOBAL_EG2_RELEASE:
				if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
					patch->commonEGs[1].release = Utilities::checkNRPNValue(0, 4095, msg);
				else
					patch->commonEGs[1].release = (int32_t)Utilities::convertEGValueToTime(Utilities::checkNRPNValue(0, 4095, msg));
				break;
			case NRPN_VALUE_GLOBAL_EG3_RELEASE:
				if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
					patch->commonEGs[2].release = Utilities::checkNRPNValue(0, 4095, msg);
				else
					patch->commonEGs[2].release = (int32_t)Utilities::convertEGValueToTime(Utilities::checkNRPNValue(0, 4095, msg));
				break;
			case NRPN_VALUE_GLOBAL_EG4_RELEASE:
				if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
					patch->commonEGs[3].release = Utilities::checkNRPNValue(0, 4095, msg);
				else
					patch->commonEGs[3].release = (int32_t)Utilities::convertEGValueToTime(Utilities::checkNRPNValue(0, 4095, msg));
				break;


			case NRPN_VALUE_GLOBAL_EG1_RESTART_ON_TRIGGER:
				patch->commonEGs[0].egRestartOnTrigger = (uint8_t) Utilities::checkNRPNValue(0, MAX_VALUE_EG_RESTART_ON_TRIGGER, msg);
				break;
			case NRPN_VALUE_GLOBAL_EG2_RESTART_ON_TRIGGER:
				patch->commonEGs[1].egRestartOnTrigger = (uint8_t) Utilities::checkNRPNValue(0, MAX_VALUE_EG_RESTART_ON_TRIGGER, msg);
				break;
			case NRPN_VALUE_GLOBAL_EG3_RESTART_ON_TRIGGER:
				patch->commonEGs[2].egRestartOnTrigger = (uint8_t) Utilities::checkNRPNValue(0, MAX_VALUE_EG_RESTART_ON_TRIGGER, msg);
				break;
			case NRPN_VALUE_GLOBAL_EG4_RESTART_ON_TRIGGER:
				patch->commonEGs[3].egRestartOnTrigger =(uint8_t) Utilities::checkNRPNValue(0, MAX_VALUE_EG_RESTART_ON_TRIGGER, msg);
				break;

			case NRPN_VALUE_GLOBAL_EG1_SHAPE:
				patch->commonEGs[0].egShape = remapShape((int8_t) Utilities::checkNRPNValue(0, 2, msg));
				break;
			case NRPN_VALUE_GLOBAL_EG1_SHAPE_MULTI:
				patch->commonEGs[0].egShape = (int8_t) Utilities::checkNRPNValue(-64, 64, msg);
				break;
			case NRPN_VALUE_GLOBAL_EG2_SHAPE:
				patch->commonEGs[1].egShape = remapShape((int8_t) Utilities::checkNRPNValue(0, 2, msg));
				break;
			case NRPN_VALUE_GLOBAL_EG2_SHAPE_MULTI:
				patch->commonEGs[1].egShape = (int8_t) Utilities::checkNRPNValue(-64, 64, msg);
				break;
			case NRPN_VALUE_GLOBAL_EG3_SHAPE:
				patch->commonEGs[2].egShape = remapShape((int8_t)  Utilities::checkNRPNValue(0, 2, msg));
				break;
			case NRPN_VALUE_GLOBAL_EG3_SHAPE_MULTI:
				patch->commonEGs[2].egShape = (int8_t) Utilities::checkNRPNValue(-64, 64, msg);
				break;
			case NRPN_VALUE_GLOBAL_EG4_SHAPE:
				patch->commonEGs[3].egShape = remapShape((int8_t) Utilities::checkNRPNValue(0, 2, msg));
				break;
			case NRPN_VALUE_GLOBAL_EG4_SHAPE_MULTI:
				patch->commonEGs[3].egShape = (int8_t) Utilities::checkNRPNValue(-64, 64, msg);
				break;

			case NRPN_VALUE_GLOBAL_EG1_UNIPOLAR:
				patch->commonEGs[0].egUnipolar = (uint8_t) Utilities::checkNRPNValue(0, 1, msg);
				break;
			case NRPN_VALUE_GLOBAL_EG2_UNIPOLAR:
				patch->commonEGs[1].egUnipolar = (uint8_t) Utilities::checkNRPNValue(0, 1, msg);
				break;
			case NRPN_VALUE_GLOBAL_EG3_UNIPOLAR:
				patch->commonEGs[2].egUnipolar =(uint8_t)  Utilities::checkNRPNValue(0, 1, msg);
				break;
			case NRPN_VALUE_GLOBAL_EG4_UNIPOLAR:
				patch->commonEGs[3].egUnipolar = (uint8_t) Utilities::checkNRPNValue(0, 1, msg);
				break;



			case NRPN_VALUE_GLOBAL_PORTAMENTO_MODE:
				patch->portamentoMode = (uint8_t) Utilities::checkNRPNValue(0, 2, msg);
				break;

			case NRPN_VALUE_GLOBAL_PORTAMENTO_TIME_OR_RATE:
				patch->portamentoTimeOrRate = (uint8_t) Utilities::checkNRPNValue(0, 1, msg);
				break;
			case NRPN_VALUE_GLOBAL_PORTAMENTO_TIME:
				patch->portamentoTime = (uint16_t) Utilities::checkNRPNValue(0, 2, msg);

				break;

	/*		case 110: // special - request send of patch data
			//	gGlobalData.pendingPatchSend = msg;
				break;*/
			default:
			{
	//			if (msg.parameter >= 90 && msg.parameter <  90 + LENGTH_OF_PATCH_NAME) // name of patch
	//				patch->header.name[msg.parameter - 90] = msg;
				break;
			}
		}
	} else // not a global setting
	{
/*		button = msg->parameterMSB; // find which button this represents - and hence which parameter page
		if (button > 0 && button < PTR_PARAMS_SIZE)
		{
			thisPage = ptrParams[button];
		}*/
		//p = &patch->params.singleParams[msg->parameterMSB];

		page = msg->parameterMSB;
		if (page > 0 && page <= 29)
			p = &patch->params.singleParams[page - 1];
		else if (page > 29 && page < 35)
			p = &patch->params.singleParams[page - 2];
		else
			p = &patch->params.paramNames.VcaMaster;


		if (p != nullptr) // specific destination page
		{

		//	p = thisPage->p;
			switch (msg->parameterLSB)
			{
				default:
					break;
				case NRPN_VALUE_PAGE_OFFSET:
				{

					#if MOTAS_VERSION == 6
						//int16_t currentPotValue = p->potentiometerValue;
						if (page == 3 || page == 10 || page == 17)
						{

							p->basicOffset = Utilities::checkValuePitch(msg) - p->potentiometerValue;;
						} else
						{

							p->basicOffset = Utilities::checkNRPNValue(-4095, 4095, msg) - p->potentiometerValue; // subtract current pot value so value is corrected elsewhere in interrupt routine encoder.c

		//					DBG("Edit basic offset: " + String(Utilities::checkNRPNValue(-4095, 4095, msg)));
						}
					#else

						if (button == BUTTON_VCO1_FREQ || button == BUTTON_VCO2_FREQ || button == BUTTON_VCO3_FREQ)
						{
							p->basicOffset = checkValuePitch(msg);
						} else
						{
							p->basicOffset = Utilities::checkNRPNValue(-4095, 4095, msg); // subtract current pot value so value is corrected elsewhere in interrupt routine encoder.c
						}

					#endif

					break;
				}
				case NRPN_VALUE_PAGE_OPTION1:
					p->option1 = (uint8_t) Utilities::checkNRPNValue(0, 7, msg);
					break;
				case NRPN_VALUE_PAGE_OPTION2:
					p->option2.value = (uint8_t) Utilities::checkNRPNValue(0, 6, msg);
					break;
				case NRPN_VALUE_PAGE_EG1_MOD_DEPTH:
					p->EG1.modDepth = (int16_t) Utilities::checkNRPNValue(-4095, 4095, msg);;
					break;

				case NRPN_VALUE_PAGE_EG1_SOURCE:

					refreshAllGUISettings = true;
					p->EG1.source = (uint8_t) Utilities::checkNRPNValue(0, 0xFF, msg);
					{
					/*	thisPage->p->EG1.source =
						if (thisPage->p->EG1.source < NUM_GLOBAL_EGS)
						{
							thisPage->p->EG1.pEG = &gPatchInUse->commonEGs[thisPage->p->EG1.source];
							thisPage->pEnvelope = &globalEGs[thisPage->p->EG1.source];
						} else
						{
							uint8_t index  = thisPage->paramIndex;
							thisPage->p->EG1.pEG = &gPatchInUse->params.singleParams[index].EG0; // use individual EG
							thisPage->pEnvelope = &gEG[index];
						}*/
					}
					break;
				case NRPN_VALUE_PAGE_EG1_RETRIG:
					p->EG0.egRetrig = (uint8_t) Utilities::checkNRPNValue(0, EG_RETRIG_MAX, msg);
					break;
				case NRPN_VALUE_PAGE_EG1_DELAY:
					p->EG0.delay = (uint16_t) Utilities::checkNRPNValue(0, 4095, msg);
					break;
				case NRPN_VALUE_PAGE_EG1_ATTACK:
					if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
						p->EG0.attack = Utilities::checkNRPNValue(0, 4095, msg);
					else
						p->EG0.attack = (int32_t)Utilities::convertEGValueToTime(Utilities::checkNRPNValue(0, 4095, msg));
					break;
				case NRPN_VALUE_PAGE_EG1_DECAY:
					if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
						p->EG0.decay = Utilities::checkNRPNValue(0, 4095, msg);
					else
						p->EG0.decay = (int32_t)Utilities::convertEGValueToTime(Utilities::checkNRPNValue(0, 4095, msg));
					break;
				case NRPN_VALUE_PAGE_EG1_SUSTAIN:
					p->EG0.sustain = (uint16_t)Utilities::checkNRPNValue(0, 4095, msg);
					break;
				case NRPN_VALUE_PAGE_EG1_RELEASE:
					if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
						p->EG0.release = Utilities::checkNRPNValue(0, 4095, msg);
					else
						p->EG0.release = (int32_t)Utilities::convertEGValueToTime(Utilities::checkNRPNValue(0, 4095, msg));
					break;
				case NRPN_VALUE_PAGE_EG1_SHAPE:
				{
					p->EG0.egShape = remapShape((int8_t) Utilities::checkNRPNValue(0, 2, msg));
					break;
				}
				case NRPN_VALUE_PAGE_EG1_SHAPE_MULTI:

					p->EG0.egShape = (int8_t) Utilities::checkNRPNValue(-64, 64, msg);
					break;
				case NRPN_VALUE_PAGE_EG1_UNIPOLAR:
					p->EG0.egUnipolar = (uint8_t) Utilities::checkNRPNValue(0, 1, msg);
					break;



				case NRPN_VALUE_PAGE_EG1_RESTART_ON_TRIGGER:
					p->EG0.egRestartOnTrigger = (uint8_t) Utilities::checkNRPNValue(0, MAX_VALUE_EG_RESTART_ON_TRIGGER, msg);
					break;

				case NRPN_VALUE_PAGE_LFO1_SOURCE:
					refreshAllGUISettings = true;
					p->LFO1.source = (uint8_t) Utilities::checkNRPNValue(0, 3, msg);
				/*
					{
						p->LFO1.source = Utilities::checkNRPNValue(0, 3, msg);
						p->LFO1.pLFO = &patch->commonLFOs[p->LFO1.source];
						thisPage->pLFO = &globalLFOs[p->LFO1.source];
					}*/
					break;
				case NRPN_VALUE_PAGE_LFO1_MOD_DEPTH:
					p->LFO1.modDepth = (int16_t) Utilities::checkNRPNValue(-4095, 4095, msg);
					break;
				case NRPN_VALUE_PAGE_LFO2_WAVEFORM:
					p->LFO2.waveformType = (uint8_t) Utilities::checkNRPNValue(0,  LFO2_MAX_CHOICE_VALUE, msg);
					break;
				case NRPN_VALUE_PAGE_LFO2_TRIGGER:
					p->LFO2.trigger =(uint8_t)  Utilities::checkNRPNValue(0, LFO_TRIGGER_MAX_STATE - 1 , msg);
					break;
				case NRPN_VALUE_PAGE_LFO2_SINGLE_SHOT:
					p->LFO2.extras.singleShotLFO2 = (uint8_t) Utilities::checkNRPNValue(0, 1 , msg);
					break;
				case NRPN_VALUE_PAGE_LFO2_FREQ:
					p->LFO2.freq = (int16_t) Utilities::checkNRPNValue(0, 4095, msg);
					break;
				case NRPN_VALUE_PAGE_LFO2_MOD_DEPTH:
					p->LFO2.modDepth = (int16_t) Utilities::checkNRPNValue(-4095, 4095, msg);
					break;
				case NRPN_VALUE_PAGE_LFO2_OUTPUT_MODE:
					p->LFO2.outputMode = (uint8_t) Utilities::checkNRPNValue(0, 2, msg);
					break;
				case NRPN_VALUE_PAGE_LFO2_PITCH_TRACK:
					p->LFO2.extras.pitchTrackLFO2 = (uint8_t) Utilities::checkNRPNValue(0, LFO_MODE_MAX-1, msg);
					break;

				case NRPN_VALUE_PAGE_MOD1_DEST:
					p->MIDICon1.destination =(uint8_t)  Utilities::checkNRPNValue(0, MOD_DEST_END - 1, msg);

					break;

				case NRPN_VALUE_PAGE_LFO2_LFOX_FREQ_MOD:
					p->more.lfo2Mod_And_ModofMod.LFO2_freqModFromLFO1 =  (int16_t)Utilities::checkNRPNValue(-4095, 4095, msg);
					break;
				case NRPN_VALUE_PAGE_LFO2_EG_FREQ_MOD:
					p->more.lfo2Mod_And_ModofMod.LFO2_freqModFromEG =  (int16_t)Utilities::checkNRPNValue(-4095, 4095, msg);
					break;


				case NRPN_VALUE_PAGE_MOD1_MOD_DEPTH:
					p->MIDICon1.modDepth = (int16_t) Utilities::checkNRPNValue(-4095, 4095, msg);
				//	DBG("update MOD1 depth");
					break;
				case NRPN_VALUE_PAGE_MOD1_MOD_DEPTH_ALTERNATE:
					p->MIDICon1.modDepthAlternate = (int16_t) Utilities::checkNRPNValue(-4095, 4095, msg);
					break;
				case NRPN_VALUE_PAGE_MOD1_MOD_UNIPOLAR:
					p->MIDICon1.unipolar = (uint8_t) Utilities::checkNRPNValue(0, 1, msg);
					break;



				case NRPN_VALUE_PAGE_MOD2_DEST:
					p->MIDICon2.destination = (uint8_t) Utilities::checkNRPNValue(0, MOD_DEST_END - 1, msg);
					break;
				case NRPN_VALUE_PAGE_MOD2_MOD_DEPTH:
					p->MIDICon2.modDepth = (int16_t) Utilities::checkNRPNValue(-4095, 4095, msg);
					break;
				case NRPN_VALUE_PAGE_MOD2_MOD_DEPTH_ALTERNATE:
					p->MIDICon2.modDepthAlternate = (int16_t) Utilities::checkNRPNValue(-4095, 4095, msg);
					break;
				case NRPN_VALUE_PAGE_MOD2_MOD_UNIPOLAR:
					p->MIDICon2.unipolar = (uint8_t) Utilities::checkNRPNValue(0, 1, msg);
					break;

				case NRPN_VALUE_PAGE_MOD3_DEST:
					p->MIDICon3.destination = (uint8_t) Utilities::checkNRPNValue(0, MOD_DEST_END - 1, msg);
					break;
				case NRPN_VALUE_PAGE_MOD3_MOD_DEPTH:
					p->MIDICon3.modDepth =(int16_t)  Utilities::checkNRPNValue(-4095, 4095, msg);

					break;
				case NRPN_VALUE_PAGE_MOD3_MOD_DEPTH_ALTERNATE:
					p->MIDICon3.modDepthAlternate = (int16_t) Utilities::checkNRPNValue(-4095, 4095, msg);
					break;
				case NRPN_VALUE_PAGE_MOD3_MOD_UNIPOLAR:
					p->MIDICon3.unipolar = (uint8_t) Utilities::checkNRPNValue(0, 1, msg);
					break;

				case NRPN_VALUE_PAGE_MOD4_DEST:
					p->MIDICon4.destination = (uint8_t)  Utilities::checkNRPNValue(0, MOD_DEST_END - 1, msg);
					break;
				case NRPN_VALUE_PAGE_MOD4_MOD_DEPTH:
					p->MIDICon4.modDepth = (int16_t) Utilities::checkNRPNValue(-4095, 4095, msg);
					break;
				case NRPN_VALUE_PAGE_MOD4_MOD_UNIPOLAR:
					p->MIDICon4.unipolar = (uint8_t) Utilities::checkNRPNValue(0, 1, msg);
					break;


				case NRPN_VALUE_PAGE_MOD4_MOD_DEPTH_ALTERNATE:
					p->MIDICon4.modDepthAlternate = (int16_t) Utilities::checkNRPNValue(-4095, 4095, msg);
					break;

				case NRPN_VALUE_PAGE_VELOCITY_DEST:
					p->MIDIVelocity.destination = (uint8_t) Utilities::checkNRPNValue(0, MOD_DEST_END - 1, msg);
					break;
				case NRPN_VALUE_PAGE_VELOCITY_MOD_DEPTH:
					p->MIDIVelocity.modDepth = (int16_t) Utilities::checkNRPNValue(-4095, 4095, msg);
					break;
				case NRPN_VALUE_PAGE_VELOCITY_MOD_DEPTH_ALTERNATE:
					p->MIDIVelocity.modDepthAlternate = (int16_t) Utilities::checkNRPNValue(-4095, 4095, msg);
					break;
				case NRPN_VALUE_PAGE_VELOCITY_MOD_UNIPOLAR:
					p->MIDIVelocity.unipolar = (uint8_t) Utilities::checkNRPNValue(0, 1, msg);
					break;



				case NRPN_VALUE_PAGE_NOTE_DEST:
					p->MIDINote.destination = (uint8_t) Utilities::checkNRPNValue(0, MOD_DEST_END - 1, msg);
					break;
				case NRPN_VALUE_PAGE_NOTE_MOD_DEPTH:
					p->MIDINote.modDepth = (int16_t) Utilities::checkNRPNValue(-4095, 4095, msg);
					break;
				case NRPN_VALUE_PAGE_NOTE_MOD_DEPTH_ALTERNATE:
					p->MIDINote.modDepthAlternate = (int16_t) Utilities::checkNRPNValue(-4095, 4095, msg);
					break;

				case NRPN_VALUE_PAGE_NOTE_MOD_UNIPOLAR:
					p->MIDINote.unipolar = (uint8_t) Utilities::checkNRPNValue(0, 1, msg);
					break;

			}
		}
	}
	crc.updatePatchCRC(patch);

	if (!dataFromGUI || refreshAllGUISettings)
		guiNeedsUpdate = true;
	dataChanged = true;


}







uint32_t MIDIProcessor::getMIDISendRate()
{
	return this->MIDISendRate;
}


void MIDIProcessor::populatePatch(uint8_t* buffer)
{
	if (buffer != nullptr)
	{
#if BUILD_STANDALONE_EXECUTABLE == 0
		//memcpy(pluginProcessor->appCommonPtr->getActivePatch(), buffer, PATCH_LENGTH_BYTES_UNPACKED);
		pluginProcessor->appCommonPtr->updateCurrentPatch(buffer);
#else
//	tAllParams* patch;

	if (getHoldingWindow() != nullptr)
	{
		//memcpy(getHoldingWindow()->appCommon->activePatch, buffer, PATCH_LENGTH_BYTES_UNPACKED);
		getHoldingWindow()->appCommon->updateCurrentPatch(buffer);
	} else
	{
		return;
	}

#endif
		commandRequest(MIDIProcessor::COMMAND_REQUEST_GUI_UPDATE, 0);
	}
}

#if BUILD_STANDALONE_EXECUTABLE == 0
MotasPluginParameter* MIDIProcessor::getDAWParameter(int page, int param)
{
	MotasPluginParameter* m = nullptr;

	int keyToLookFor = page * 128 + param;
	if (pluginProcessor->hashMapParameters.contains(keyToLookFor))
	{
		m = pluginProcessor->hashMapParameters[keyToLookFor];
	}
	return m;
}



#endif



void MIDIProcessor::updateMIDISettings()
{
	ApplicationCommon* appCom = getApplicationCommon();


	if (appCom != nullptr)
	{
		allowMIDIThru = appCom->loadAppPropertyBool(ApplicationCommon::APP_PROPERTY_MIDI_THRU_ENABLED);
		autoConnectMotas = appCom->loadAppPropertyBool(ApplicationCommon::APP_PROPERTY_AUTO_CONNECT_MOTAS);
		allowForwardSyExToDAW = appCom->loadAppPropertyBool(ApplicationCommon::APP_PROPERTY_PASS_SYSEX);

		hideNonMotasDevices  = appCom->loadAppPropertyBool(ApplicationCommon::APP_PROPERTY_HIDE_NON_MOTAS_DEVICES);
/*		int updateInt = appCom->loadAppPropertyInteger(ApplicationCommon::APP_PROPERTY_MIDI_UPDATE_INTERVAL);

		float deviceCheckDividerF = 30.0f / timerUpdatePeriodMS; // 30ms at fastest setting
		for (int j = 1; j < updateInt; j++)
		{
			deviceCheckDividerF *= 1.55f;
		}
		deviceCheckDivider  = (int) deviceCheckDividerF;
		if (deviceCheckDivider <= 0)
			deviceCheckDivider = 1;
*/

		midiRoutingBitfield = (uint16_t) appCom->loadAppPropertyInteger(ApplicationCommon::APP_PROPERTY_MIDI_ROUTING_BITFIELD);
		float rate = 200000.0f;
		int rateInt = appCom->loadAppPropertyInteger(ApplicationCommon::APP_PROPERTY_MIDI_SEND_RATE);
		for (int j = 0; j < rateInt; j++)
		{
			rate *= 0.55f;
		}
		MIDISendRate = (uint32_t) rate;

		autoPageChange = appCom->loadAppPropertyBool(ApplicationCommon::APP_PROPERTY_ENABLE_AUTO_PAGE_CHANGE);
		liveScreenshotEnabled = appCom->loadAppPropertyBool(ApplicationCommon::APP_PROPERTY_LIVE_SCREENSHOT);
		showOnlyMonitor = appCom->loadAppPropertyBool(ApplicationCommon::APP_PROPERTY_ENABLE_SHOW_MONITOR_ONLY);
	} else
	{
		MIDISendRate = 11000;
	//	deviceCheckDivider = 20;
		allowMIDIThru = false;
		autoConnectMotas = false;
		allowForwardSyExToDAW = false;
		hideNonMotasDevices = true;
		autoPageChange = false;
		liveScreenshotEnabled = false;
		showOnlyMonitor = false;
	}

	if (!autoConnectMotas)
	{
		connectedMotasInput = false;
		connectedMotasOutput = false;
	}

	//setUpdateInterval(MIDIupdateInterval);

}
void MIDIProcessor::timerCallback()
{
	//DBG("Standard timer callback");
	doSlowerTasks();
}

void MIDIProcessor::indicatePatchDataChanged()
{

	this->patchDataEdited = true;

}

/*
 *
 * MessageManagerLock mml (Thread::getCurrentThread());
				//
				//
				// It appears that we must NOT try to use the MessageManagerLock if there
				// is no GUI thread otherwise this will hang on Windows when program closed
				// as cannot kill the thread!
				//
				if (mml.lockWasGained())
				{
					DBG("Thread locked from HiResTimer");
					processSysExMIDIdata(MIDItransferType, readMidiStreamData);
					DBG("finished GUI update (unlock imminent)");
				} else
				{
					DBG("Thread could NOT be locked!-------------");
				}

				*/

void MIDIProcessor::sysExDoProcessing()
{

	static const int TIMEOUT_SEND_SCREENSHOT_REQ = 1000;
	int currentTab = -1;
	HoldingWindowComponent* hw = getHoldingWindow();

	if (hw != nullptr && hw->getActiveTabIndex() >= 0)
	{
		currentTab = hw->getActiveTabIndex();


		DBG("retry:" + String((int) retry));
		DBG("MIDIsysExFullyReceived:" + String((int) MIDIsysExFullyReceived));


		if (retry || MIDIsysExFullyReceived) // when we have all the data for processing
		{
			if (MIDIsysExFullyReceived)
			{
				processSysExMIDIdata(MIDItransferType, readMidiStreamData);
				MIDIsysExFullyReceived = false;
			}
			if (!busySendingSysEx && liveScreenshotEnabled
					&& currentTab == 0)
			{
				// request screenshot from Motas
				if (sendSysEx("bitmap cmd", MIDI_SYSEX_REQUEST_BITMAP_COMPRESSED,  0))
				{
					// was busy so re-try later
					retry = true;
				} else
				{
					retry = false;
					timeLastRequestSent = Time::getMillisecondCounter();
					DBG("Request screenshot- timeLastRequestSent:" + String(timeLastRequestSent));
				}
			}
		} else
		{
			//DBG("!MIDIsysExFullyReceived");
			if (!busySendingSysEx)
			{
				if (liveScreenshotEnabled && currentTab == 0)
				{
					uint32_t t = Time::getMillisecondCounter();
					if (lastActiveTab != 0 || (t - timeLastRequestSent) > TIMEOUT_SEND_SCREENSHOT_REQ) // if moved back to tab = 0 or more than 2secs since last request
					{
						lastActiveTab = 0;
						this->sendSysEx("bitmap cmd", MIDI_SYSEX_REQUEST_BITMAP_COMPRESSED,  0);
						timeLastRequestSent = Time::getMillisecondCounter();
					}
				}
				else
				{
					lastActiveTab = currentTab;
				}
			}
		}
	}
}


/**
 *
 * // slower update inside this to connect MIDI devices, send NRPN for page change and sysEx request screenshot
 * * called from the GUI thread (or similar...)
 */
void MIDIProcessor::doSlowerTasks()
{

	stopTimer();

#if RUN_SEPARATE_THREAD == 0
	threadFunction();
#endif
	randomNumber++;

	if (fifoParamChangesFromMotasToDAW.isAnyDataReady())
	{
		int numBytesInBuffer = fifoParamChangesFromMotasToDAW.readAllFromFifo(paramChangesFromMotasToDAWBuffer, 4);
		int bytesRead = 0;
//		DBG("Data ready in fifoParamChangesFromMotasToDAW : " + String(numBytesInBuffer) + " bytes");
		NRPNmessage_t n;
		while (numBytesInBuffer > 0)
		{
			n.parameterMSB = *(paramChangesFromMotasToDAWBuffer + bytesRead++);
			n.parameterLSB = *(paramChangesFromMotasToDAWBuffer + bytesRead++);
			n.valueMSB = *(paramChangesFromMotasToDAWBuffer + bytesRead++);
			n.valueLSB = *(paramChangesFromMotasToDAWBuffer + bytesRead++);
			numBytesInBuffer -= bytesRead;
			informDAW(n);
		}
	}

	HoldingWindowComponent* hw = getHoldingWindow();
	if (hw != nullptr
			&& hw->patchPatternSequence != nullptr
			&& hw->firmwareComponent != nullptr
			&& hw->mainComponent != nullptr)
	{

		if (guiBusyUpdating)
		{
			hw->mainComponent->updatePanel();
			//DBG("UPDATE GUI");
			guiBusyUpdating = false;
		}
		hw->mainComponent->updatePatchComboText();
		for (int i = 0; i < 4; i++)
		{
			if (!latestMessageRead[i])
			{
				hw->newMessage(getInfoMessage(i), i);
				latestMessageRead[i] = true;
			}
		}

		hw->indicateMessageIsPending(isBusySendingGuiCommand);
		if (midiDataInFromMotas)
		{
			midiDataInFromMotas = false;
			hw->indicateMidiInFromMotas();
		}
		if (midiDataOutToMotas)
		{
			midiDataOutToMotas = false;
			hw->indicateMidiOutToMotas();
		}
		if (midiSysExDataOutToMotas)
		{
			midiSysExDataOutToMotas = false;
			hw->indicateMidiOutSysExToMotas();
		}
		if (midiSysExDataInFromMotas)
		{
			midiSysExDataInFromMotas = false;
			hw->indicateMidiInSysExToMotas();
		}
		hw->updateLEDs();
		if (isBusyReceivingGuiCommand)
		{
			if (activeCommandToGui != ACTION_FOR_GUI_NOTHING)
			{
				DBG("There is an action for the GUI: " + String(activeCommandToGui));
			}
			uint8_t* buffer = incomingDataArray.getRawDataPointer();
			switch(activeCommandToGui)
			{
				default:
				case ACTION_FOR_GUI_NOTHING:
					break;
				case ACTION_FOR_GUI_GLOBALS_TRANSFER:
					hw->patchPatternSequence->saveData(MIDI_SYSEX_GLOBALS_TRANSFER, buffer);
					break;
				case ACTION_FOR_GUI_FIRMWARE_VERSION_REPORTED:
					hw->firmwareComponent->updateFirmwareVersionReported(buffer);
					break;
				case ACTION_FOR_GUI_INFO_TRANSFER:
					hw->firmwareComponent->updateInfo(buffer);
					break;
				case ACTION_FOR_GUI_BULK_TRANSFER:
					hw->patchPatternSequence->saveData(MIDI_SYSEX_REQUEST_BULK_FLASH_DATA_SEND, buffer);
					break;
				case ACTION_FOR_GUI_SCREENSHOT:
					hw->mainComponent->updateMotasScreenshot(this->bitmapBuffer, 4096);
					break;
				case ACTION_FOR_GUI_PATTERN:
					hw->patchPatternSequence->populateCurrentPattern(buffer);
					break;
				case ACTION_FOR_GUI_PATTERN_BANK:
					hw->patchPatternSequence->populateAllPatterns(buffer);
					break;
				case ACTION_FOR_GUI_SEQUENCE:
					hw->patchPatternSequence->populateCurrentSequence(buffer);
					break;
				case ACTION_FOR_GUI_SEQUENCES_BANK:
					hw->patchPatternSequence->populateAllSequences(buffer);
					break;
				case ACTION_FOR_GUI_PATCH:
					hw->patchPatternSequence->populateCurrentPatch(buffer);
					break;
				case ACTION_FOR_GUI_PATCH_BANK:
					hw->patchPatternSequence->populateAllPatches(buffer);
					break;
	/*			case ACTION_FOR_GUI_POPULATE_PATCH:
					populatePatch(buffer);
					break;*/
			}
			isBusyReceivingGuiCommand = false;
		}
		hw->mainComponent->updateParamText();
	}


	uint32_t timeStamp = Time::getMillisecondCounter();
	// every 500ms check the inputs and send command setting the page update option
	if ((timeStamp - prevTimeStamp) > 1000)
	{
		prevTimeStamp = timeStamp;
	//	DBG("autoConnectMIDI check devices..." + String(timeStamp) + " this pointer: "+ String((uint64) this));
		autoConnectMIDI();
		uint8_t val = 0;
		if (autoPageChange)
			val = 0x01;
		if (showOnlyMonitor)
			val |= 0x02; // request show only monitor page on the Motas OLED
		// request info, and if not matching the current patch then send the patch

		commandRequest(COMMAND_REQUEST_STATUS, val);

	}

	debugThin++;


	if (debugThin % 100 == 0)
	{
		DBG("slow task..." + String(timeStamp) + " this pointer: "+ String((uint64) this));
		debugThin = 0;
	}

	//DBG("doSlowerTasks.." + String(prevTimeStamp) + " pointer: "+ String((uint64) this));

	// start the timer that calls this function again
	startTimer(timerUpdatePeriodMS);

}



void MIDIProcessor::doTasks()
{

	// check for GUI changes FIRST
	if (fifoParamChangesFromGUI.isAnyDataReady())
	{
		int numBytesInBuffer = fifoParamChangesFromGUI.readAllFromFifo(paramChangesFromGUIBuffer, 4);
		int bytesRead = 0;
//		DBG("Data ready in fifoParamChangesFromGUI : " + String(numBytesInBuffer) + " bytes");
		NRPNmessage_t n;
		dataFromGUI = true;
		while (numBytesInBuffer > 0)
		{
			n.parameterMSB = *(paramChangesFromGUIBuffer + bytesRead++);
			n.parameterLSB = *(paramChangesFromGUIBuffer + bytesRead++);
			n.valueMSB = *(paramChangesFromGUIBuffer + bytesRead++);
			n.valueLSB = *(paramChangesFromGUIBuffer + bytesRead++);
			numBytesInBuffer -= bytesRead;
			sendNRPNtoMIDIOut(n, MIDI_DATA_SEND_TYPE_NRPN_FROM_MOTAS_EDIT);
			processMIDIControllerChangeMessage(&n);

		}
		dataFromGUI = false;
	}

#if BUILD_STANDALONE_EXECUTABLE == 0
	if (pluginProcessor->fifoMidiParameters.isAnyDataReady())
	{
		int numBytesInBuffer = this->pluginProcessor->fifoMidiParameters.readAllFromFifo(midiInDAWParametersBuffer, 4);
//		DBG("Data ready in fifoMidiParameters : " + String(numBytesInBuffer) + " bytes");
		int bytesRead = 0;
		NRPNmessage_t n;
		while (numBytesInBuffer > 0)
		{
			n.parameterMSB = *(midiInDAWParametersBuffer + bytesRead++);
			n.parameterLSB = *(midiInDAWParametersBuffer + bytesRead++);
			n.valueMSB = *(midiInDAWParametersBuffer + bytesRead++);
			n.valueLSB = *(midiInDAWParametersBuffer + bytesRead++);
			numBytesInBuffer -= bytesRead;
			sendNRPNtoMIDIOut(n, MIDI_DATA_SEND_TYPE_NRPN_FROM_DAW);
			processMIDIControllerChangeMessage(&n);
		}
	}
#endif

	// 3 byte controller / note on/off messages from the GUI fake keyboard and modulator controls
	if (fifoMidiFromGUI.isAnyDataReady())
	{
		int numBytesInBuffer = fifoMidiFromGUI.readAllFromFifo(notesControllersFromGUIBuffer, 3);
		int bytesRead = 0;
		int numBytesUsed = 0;
//		DBG("Data ready in fifoMidiFromGUI : " + String(numBytesInBuffer) + " bytes");
		//NRPNmessage_t n;

#if INCLUDE_TEST_FUNCTIONS == 1

		int time = 0;

		int origNumBytesInBuffer = numBytesInBuffer;
		for (int i = 0; i < 1; i++)
		{
			bytesRead = 0;
			numBytesUsed = 0;
			numBytesInBuffer = origNumBytesInBuffer;
			while (numBytesInBuffer > 0)
			{
				MidiMessage mm(notesControllersFromGUIBuffer + bytesRead, numBytesInBuffer, numBytesUsed, time, 0 , false);
				time += 100;


				if (numBytesUsed <= 0)
					break;
				bytesRead += numBytesUsed;
				numBytesInBuffer -= numBytesUsed;
				processMidiInMessage(mm, MIDI_DATA_SOURCE_MOTAS_EDIT);
			}
		}
#else

		while (numBytesInBuffer > 0)
		{
			MidiMessage mm(notesControllersFromGUIBuffer + bytesRead, numBytesInBuffer, numBytesUsed, 0, 0 , false);
			if (numBytesUsed <= 0)
				break;
			bytesRead += numBytesUsed;
			numBytesInBuffer -= numBytesUsed;
			processMidiInMessage(mm, MIDI_DATA_SOURCE_MOTAS_EDIT);
		}
#endif
	}


	// get any new MIDI incoming data from the atomic FIFO circular buffer
	if (fifoMidiInFromMotas.isAnyDataReady())
	{
		int numBytesInBuffer = fifoMidiInFromMotas.readAllFromFifo(midiInFromMotasBuffer, 1);
//		DBG("Data ready in fifoMidiInFromMotas : " + String(numBytesInBuffer) + " bytes");
		int numBytesUsed = 0;
		int bytesRead = 0;
		while (numBytesInBuffer > 0)
		{
			MidiMessage mm(midiInFromMotasBuffer + bytesRead, numBytesInBuffer, numBytesUsed, 0, 0 , false);
		//	DBG("midiInFromMotasBuffer... numBytesInBuffer: "
		//			+ String(numBytesInBuffer) +  " numBytesUsed: " + String(numBytesUsed)  + " bytesRead: " + String(bytesRead));
			if (numBytesUsed <= 0)
				break;
			bytesRead += numBytesUsed;
			numBytesInBuffer -= numBytesUsed;
			processMidiInMessage(mm, MIDI_DATA_SOURCE_MOTAS);
		}

	}


#if BUILD_STANDALONE_EXECUTABLE == 1
	if (fifoMidiInFromOthers.isAnyDataReady())
	{
		int numBytesInBuffer = fifoMidiInFromOthers.readAllFromFifo(midiInFromOthersBuffer, 1);
		int numBytesUsed = 0;
		int bytesRead = 0;
		while (numBytesInBuffer > 0)
		{
			MidiMessage mm(midiInFromOthersBuffer + bytesRead, numBytesInBuffer, numBytesUsed, 0, 0 , false);
		//	DBG("midiInFromMotasBuffer... numBytesInBuffer: "
		//			+ String(numBytesInBuffer) +  " numBytesUsed: " + String(numBytesUsed)  + " bytesRead: " + String(bytesRead));
			if (numBytesUsed <= 0)
				break;
			bytesRead += numBytesUsed;
			numBytesInBuffer -= numBytesUsed;
			processMidiInMessage(mm, MIDI_DATA_SOURCE_DAW);
		}

	}



#endif


#if PROCESS_MIDI_INCOMING_FROM_DAW_PROCESS_BLOCK == 1
	if (pluginProcessor->fifoMidiIn.isAnyDataReady())
	{
		int numBytesInBuffer = this->pluginProcessor->fifoMidiIn.readAllFromFifo(midiInBuffer, 1);
		DBG("Data ready in fifoMidiIn : " + String(numBytesInBuffer) + " bytes");
		if (numBytesInBuffer > 0)
		{
			//DBG(String("num bytes in buffer " + String(numBytesInBuffer)));
			for (int i = 0 ; i < numBytesInBuffer; i++)
			{
				DBG(String("0x" + String::toHexString(this->midiInBuffer[i])));
			}
			int numBytesUsed = 0;
			int bytesRead = 0;
			//uint8_t lastByte = 0;
			while (numBytesInBuffer > 0)
			{
				MidiMessage mm(this->midiInBuffer + bytesRead, numBytesInBuffer, numBytesUsed, 0, 0 , false);

				//lastByte = this->midiInBuffer[bytesRead];
				bytesRead += numBytesUsed;
				numBytesInBuffer -= numBytesUsed;
				processMidiInMessage(mm, MIDI_DATA_SOURCE_DAW);
			}
		}
	}
#endif


#if BUILD_STANDALONE_EXECUTABLE == 0
		/*
	numBytes = this->pluginProcessor->fifoMidiInFromDAWParameters.readAllFromFifo(midiInDAWParametersBuffer);
	if (numBytes > 0)
	{
		DBG("DAW num bytes " + String(numBytes));

		//HoldingWindowComponent* hw = (HoldingWindowComponent*) pluginProcessor->getActiveEditor();
		//if (hw != nullptr && hw->midiComms.get() != nullptr)
		{
			//hw->newMessage("hi there");

			for (int i = 0 ; i < numBytes; i++)
			{
				DBG(String("0x" + String::toHexString(this->midiInDAWParametersBuffer[i])));
			}
			int numBytesUsed = 0;
			int bytesRead = 0;
			while (numBytes > 0)
			{
				MidiMessage mm(this->midiInDAWParametersBuffer + bytesRead, numBytes, numBytesUsed, 0, 0 , false);
				bytesRead += numBytesUsed;
				processMidiInMotasMessage(mm, true, true);
				numBytes -= numBytesUsed;
			}
		}
	}*/
#endif
/*
	auto& params = this->pluginProcessor->getParameters();
	for (auto i = 0; i < params.size(); ++i)
	{
		if (auto* param = dynamic_cast<AudioParameterFloat*> (params[i]))
		{
			if (i < paramSliders.size())
				paramSliders[i]->setValue (*param);
		}
	}

*/


	//DBG("gui command PENDING executing: " + String(activeCommand));

	isMotasMidiOutConnected = true; // force always try to send in case we are using a DAW output only!


	// see if there is a command from the UI to send to Motas over the MIDI port
	if (isBusySendingGuiCommand && isMotasMidiOutConnected)
	{
		DBG("gui command executing: " + String(activeCommand));

		switch (activeCommand)
		{
			default:
				break;



			case COMMAND_REQUEST_GET_INFO:
				sendSysEx("Info request",  MIDI_SYSEX_REQUEST_INFO,  0);
				break;
			case COMMAND_REQUEST_GET_BULK:
				sendSysEx("bulk FLASH data cmd", MIDI_SYSEX_REQUEST_BULK_FLASH_DATA_SEND, 0);
				break;
			case COMMAND_REQUEST_GET_GLOBALS:
				sendSysEx("global setup data cmd", MIDI_SYSEX_REQUEST_GLOBALS, 0);
				break;
			case COMMAND_REQUEST_PATCH:
				DBG("Get patch command!");
				sendSysEx("get patch cmd", MIDI_SYSEX_REQUEST_PATCH, 0);
				break;
			case COMMAND_REQUEST_PATTERN:
				sendSysEx("get pattern cmd", MIDI_SYSEX_REQUEST_PATTERN, 0);
				break;
			case COMMAND_REQUEST_SEQUENCE:
				sendSysEx("get sequence cmd", MIDI_SYSEX_REQUEST_SEQUENCE, 0);
				break;
			case COMMAND_REQUEST_SEND_ACTIVE_PATCH:
				sendPatch(activePatchPreset, getApplicationCommon()->getActivePatch());
				break;


		}
		if (!sendArray.isEmpty())
		{
			uint8_t* buffer = sendArray.getRawDataPointer();
			switch (activeCommand)
			{
				default:
					break;
				case COMMAND_REQUEST_PATCH_NAME_CHANGE:
					getApplicationCommon()->changeActivePatchName(buffer);
					activePatchCRC32 = crc.crcFastSTMF4Patch((tAllParams*) getApplicationCommon()->getActivePatch());
					break;
				case COMMAND_REQUEST_STATUS:

					sendSysEx("Status request",  MIDI_SYSEX_REQUEST_STATUS,  (char) sendArray[0]);
					break;
				case COMMAND_REQUEST_RESET_PARAMETER_PAGE:
				{

					NRPNmessage_t n = ParameterData::convertBufferToNRPN(sendArray.getRawDataPointer());
					resetParameterInPatch(n);
					dataChanged = true;
					guiNeedsUpdate = true;
					sendPatch(activePatchPreset, getApplicationCommon()->getActivePatch());
					break;
				}
				case COMMAND_REQUEST_PATCH_CHANGE:
					getApplicationCommon()->setPatchNumber(sendArray[0]);
					dataChanged = true;
					guiNeedsUpdate = true;
					sendPatch(activePatchPreset, getApplicationCommon()->getActivePatch());
#if BUILD_STANDALONE_EXECUTABLE == 0
					pluginProcessor->updateHostDisplay();
#endif
					break;
				case COMMAND_REQUEST_PATCH_BANK:
					sendSysEx("get bank patches cmd", MIDI_SYSEX_REQUEST_PATCHES_BANK, (char) sendArray[0] );
					break;
				case COMMAND_REQUEST_PATTERN_BANK:
					sendSysEx("get patterns bank cmd", MIDI_SYSEX_REQUEST_PATTERN_BANK, (char)sendArray[0] );
					break;
				case COMMAND_REQUEST_SEQUENCE_BANK:
					sendSysEx("get sequence bank cmd", MIDI_SYSEX_REQUEST_SEQUENCE_BANK, (char)sendArray[0] );
					break;
				case COMMAND_REQUEST_SEND_FIRMARE:
					sendFirmwareOverMIDI();
					break;
				case COMMAND_REQUEST_SEND_GLOBALS:
					sendDataOverMIDIDirect(buffer, (uint32_t) sendArray.size(), 0);
					break;

				case COMMAND_REQUEST_SEND_PROGRAM_CHANGE:
					activePatchPreset = sendArray[0];
					sendProgramChange(sendArray[0]);
					break;
		/*		case COMMAND_REQUEST_SEND_NOTE_ON:
				{
					MidiMessage m (MidiMessage::noteOn (channelMIDI, sendArray[0], sendArray[1]));

					DBG("Note on velocity: " + String(sendArray[1]));
					m.setTimeStamp (Time::getMillisecondCounterHiRes() * 0.001);
					MidiBuffer buf;
					buf.addEvent(m, 1);
					sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_NOTES_FROM_MOTAS_EDIT, "Note-on from MotasEdit");
					break;
				}
	/  		case COMMAND_REQUEST_SEND_NOTE_OFF:
				{
					MidiMessage m (MidiMessage::noteOff (channelMIDI,sendArray[0], sendArray[1]));
					m.setTimeStamp (Time::getMillisecondCounterHiRes() * 0.001);
					MidiBuffer buf;
					buf.addEvent(m, 1);
					sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_NOTES_FROM_MOTAS_EDIT, "Note-off from MotasEdit");
					break;
				}
				case COMMAND_REQUEST_SEND_CONTROLLER:
				{
					MidiMessage m (MidiMessage::controllerEvent(channelMIDI, sendArray[0], sendArray[1]));
					m.setTimeStamp (Time::getMillisecondCounterHiRes() * 0.001);
					MidiBuffer buf;
					buf.addEvent(m, 1);
					sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_CONTROLLER_FROM_MOTAS_EDIT, "Controller from MotasEdit");
					break;
				}*/
				case COMMAND_REQUEST_SEND_BULK:
				{
					int32_t lastPacketNum = 0;
					const int32_t sectorCount = 96;
					uint8_t* outBuffer = new uint8_t[4096*sectorCount * 2];
					for (int i = 0; i < 8; i++) // send 96 sectors at a time
					{
						int32_t offset = i * sectorCount;
						uint32_t bytesToSend  = Utilities::prepareSysExPacketsFromUnpacked(outBuffer,
								&buffer[4096 * offset],
								4096*sectorCount,  MIDI_SYSEX_RECEIVE_BULK_FLASH_DATA, (uint8_t) (offset / 128), offset % 128, sectorCount,
								motasModel, channelMIDI);
						if (bytesToSend == 0)
						   break;
						DBG("Bytes to send: " + String(bytesToSend));
						lastPacketNum = sendDataOverMIDIDirect(outBuffer, bytesToSend, lastPacketNum);
						DBG("Last packetNum: " + String(lastPacketNum));
						lastPacketNum += 10000;
					}
					delete [] outBuffer;
				}
					break;
			}
		}
		activeCommand = COMMAND_REQUEST_NOTHING;
		isBusySendingGuiCommand = false;
	}
}


void MIDIProcessor::resetParameterInPatch(NRPNmessage_t& msg)
{
	int page = msg.parameterMSB;//.getPage();
	singleParam* sp;
	tAllParams* patch = (tAllParams*) getApplicationCommon()->getActivePatch();

	if (page > 0 && page <= 29)
		sp = &patch->params.singleParams[page - 1];
	else if (page > 29 && page < 35)
		sp = &patch->params.singleParams[page - 2];
	else
		sp = &patch->params.paramNames.VcaMaster;
	DBG("Page: " + String(page));
	for (int parameter = 0; parameter < PanelControl::CONTROL_PARAM_END_MARKER; parameter++)
	{
		int32_t val = 0;
		if (page > 0)
		{
			switch (parameter)
			{
				case PanelControl::CONTROL_PARAM_OFFSET:
					DBG("Adjusting basic page offset: " + String(val));

					if (page == PanelControl::PANEL_CONTROL_OSC1_PITCH || page == PanelControl::PANEL_CONTROL_OSC2_PITCH || page == PanelControl::PANEL_CONTROL_OSC3_PITCH)
						val = 18000;
					else if (page == PanelControl::PANEL_CONTROL_MASTER_PITCH)
						val = 0;

					// the final result we want is 'val' but internally Motas will add the potentiometerValue to the basic offset to get that result.
					sp->basicOffset  = val - sp->potentiometerValue;
					// DO NOT tamper with offsetIncludingPot as Motas will set that by adding basicOffset to potentiometerValue
					//sp->offsetIncludingPot = val;
					break;

				case PanelControl::CONTROL_PARAM_EG_MOD_DEPTH:
					sp->EG1.modDepth = (int16_t) val;
					break;
				case PanelControl::CONTROL_PARAM_EG_DELAY:
					sp->EG0.delay = (uint16_t) val;
					break;
				case PanelControl::CONTROL_PARAM_EG_ATTACK:
					sp->EG0.attack = EG_RATE_MAX_VALUE;
					break;
				case PanelControl::CONTROL_PARAM_EG_DECAY:
					sp->EG0.decay = EG_RATE_MAX_VALUE;
					break;
				case PanelControl::CONTROL_PARAM_EG_SUSTAIN:
					sp->EG0.sustain = (uint16_t) val;
					break;
				case PanelControl::CONTROL_PARAM_EG_RELEASE:
					sp->EG0.release = EG_RATE_MAX_VALUE;
					break;
				case PanelControl::CONTROL_PARAM_EG_UNIPOLAR:
					sp->EG0.egUnipolar = (uint8_t) val;
					break;
				case PanelControl::CONTROL_PARAM_LFOX_MOD_DEPTH:
					sp->LFO1.modDepth = (int16_t)val;
					break;
				case PanelControl::CONTROL_PARAM_LFO_FREQ:
					sp->LFO2.freq = (int16_t)val;
					break;
				case PanelControl::CONTROL_PARAM_LFO2_EG_FREQ_MOD:
					sp->more.lfo2Mod_And_ModofMod.LFO2_freqModFromEG = (int16_t)val;
					break;
				case PanelControl::CONTROL_PARAM_LFO2_LFOX_FREQ_MOD:
					sp->more.lfo2Mod_And_ModofMod.LFO2_freqModFromLFO1 = (int16_t)val;
					break;
				case PanelControl::CONTROL_PARAM_LFO_MOD_DEPTH:
					sp->LFO2.modDepth = (int16_t) val;
					break;
				case PanelControl::CONTROL_PARAM_M1_LEVEL:
					sp->MIDICon1.modDepth = (int16_t)  val;
					break;
				case PanelControl::CONTROL_PARAM_M1_ALT_LEVEL:
					sp->MIDICon1.modDepthAlternate = (int16_t)  val;
					break;
				case PanelControl::CONTROL_PARAM_M1_UNIPOLAR:
					sp->MIDICon1.unipolar = (uint8_t)  val;
					break;



				case PanelControl::CONTROL_PARAM_M2_LEVEL:
					sp->MIDICon2.modDepth = (int16_t)  val;
					break;
				case PanelControl::CONTROL_PARAM_M2_ALT_LEVEL:
					sp->MIDICon2.modDepthAlternate = (int16_t)  val;
					break;
				case PanelControl::CONTROL_PARAM_M2_UNIPOLAR:
					sp->MIDICon2.unipolar = (uint8_t)  val;
					break;

				case PanelControl::CONTROL_PARAM_M3_LEVEL:
					sp->MIDICon3.modDepth = (int16_t)  val;
					break;
				case PanelControl::CONTROL_PARAM_M3_ALT_LEVEL:
					sp->MIDICon3.modDepthAlternate =(int16_t)   val;
					break;
				case PanelControl::CONTROL_PARAM_M3_UNIPOLAR:
					sp->MIDICon3.unipolar = (uint8_t)  val;
					break;

				case PanelControl::CONTROL_PARAM_M4_LEVEL:
					sp->MIDICon4.modDepth = (int16_t)  val;
					break;
				case PanelControl::CONTROL_PARAM_M4_ALT_LEVEL:
					sp->MIDICon4.modDepthAlternate = (int16_t)  val;
					break;
				case PanelControl::CONTROL_PARAM_M4_UNIPOLAR:
					sp->MIDICon4.unipolar = (uint8_t)  val;
					break;

				case PanelControl::CONTROL_PARAM_VELOCITY_LEVEL:
					sp->MIDIVelocity.modDepth =(int16_t)   val;
					break;
				case PanelControl::CONTROL_PARAM_VELOCITY_ALT_LEVEL:
					sp->MIDIVelocity.modDepthAlternate = (int16_t)  val;
					break;
				case PanelControl::CONTROL_PARAM_VELOCITY_UNIPOLAR:
					sp->MIDIVelocity.unipolar = (uint8_t)  val;
					break;

				case PanelControl::CONTROL_PARAM_NOTE_LEVEL:
					sp->MIDINote.modDepth = (int16_t)  val;
					break;
				case PanelControl::CONTROL_PARAM_NOTE_ALT_LEVEL:
					sp->MIDINote.modDepthAlternate = (int16_t) val;
					break;
				case PanelControl::CONTROL_PARAM_NOTE_UNIPOLAR:
					sp->MIDINote.unipolar = (uint8_t) val;
					break;

				case PanelControl::CONTROL_PARAM_PAGE_OPTION1:
					sp->option1 = (uint8_t) val;
					break;
				case PanelControl::CONTROL_PARAM_PAGE_OPTION2:
					sp->option2.value = (uint8_t) val;
					break;

				case PanelControl::CONTROL_PARAM_EG_SOURCE:
					sp->EG1.source = 0xFF;
					break;
				case PanelControl::CONTROL_PARAM_EG_RETRIG:
					sp->EG0.egRetrig = (uint8_t) val;
					break;
				case PanelControl::CONTROL_PARAM_EG_RESTART:
					sp->EG0.egRestartOnTrigger = (uint8_t) val;
					break;
				case PanelControl::CONTROL_PARAM_EG_SHAPE_MULTI:
					sp->EG0.egShape =(int8_t)  val;
					break;

				case PanelControl::CONTROL_PARAM_LFO_SOURCE:
					sp->LFO1.source = (uint8_t) 0;
					break;

				case PanelControl::CONTROL_PARAM_LFO_WAVE:
					sp->LFO2.waveformType = (uint8_t) val;
					break;

				case PanelControl::CONTROL_PARAM_LFO_TRIGGER:
					sp->LFO2.trigger = (uint8_t) val;
					break;

				case PanelControl::CONTROL_PARAM_LFO_SINGLE_SHOT:
					sp->LFO2.extras.singleShotLFO2 = (uint8_t) val;
					break;
				case PanelControl::CONTROL_PARAM_LFO_OUTPUT_MODE:
					sp->LFO2.outputMode = (uint8_t) val;
					break;
				case PanelControl::CONTROL_PARAM_LFO2_PITCH_TRACK:
					sp->LFO2.extras.pitchTrackLFO2 = (uint8_t) val;

					break;

				case PanelControl::CONTROL_PARAM_M1_DESTINATION:
					sp->MIDICon1.destination = (uint8_t) val;
					break;
				case PanelControl::CONTROL_PARAM_M2_DESTINATION:
					sp->MIDICon2.destination = (uint8_t) val;
					break;
				case PanelControl::CONTROL_PARAM_M3_DESTINATION:
					sp->MIDICon3.destination =(uint8_t)  val;
					break;
				case PanelControl::CONTROL_PARAM_M4_DESTINATION:
					sp->MIDICon4.destination =  (uint8_t) val;
					break;
				case PanelControl::CONTROL_PARAM_VELOCITY_DESTINATION:
					sp->MIDIVelocity.destination = (uint8_t) val;
					break;
				case PanelControl::CONTROL_PARAM_NOTE_DESTINATION:
					sp->MIDINote.destination = (uint8_t) val;
					break;
				default:
					break;

			}
		}
	}
	crc.updatePatchCRC(patch);

}

bool MIDIProcessor::commandRequest(int commandNum, uint8_t value)
{
	return commandRequest(commandNum, &value, 1);
}


bool MIDIProcessor::commandRequest(int commandNum, NRPNmessage_t& n)
{
	uint8_t buffer[4];
	buffer[0] = n.parameterMSB;
	buffer[1] = n.parameterLSB;
	buffer[2] = n.valueMSB;
	buffer[3] = n.valueLSB;

	if (commandNum == COMMAND_REQUEST_PARAMETER_CHANGED)
	{
		#if BUILD_STANDALONE_EXECUTABLE == 0
			informDAW(n);
		#endif
	}
	return commandRequest(commandNum, buffer, 4);


}

void MIDIProcessor::updateGUIRequest()
{
	guiNeedsUpdate = true;
	dataChanged = true;
	this->notify();
}
bool MIDIProcessor::commandRequest(int commandNum, uint8_t* dataBuffer,	int size)
{
	//DBG("commandRequest: " + String(commandNum));
	if (commandNum == COMMAND_REQUEST_PARAMETER_CHANGED)
	{
		// make this one faster than all the others
		// add to the GUI changes FIFO!
		fifoParamChangesFromGUI.addToFifo(dataBuffer, size);
		this->notify();
		return 1;
	} else if (commandNum == COMMAND_REQUEST_GUI_UPDATE)
	{
		updateGUIRequest();
		return 1;
	}else if (commandNum == COMMAND_REQUEST_SEND_GUI_NOTES_CONTROLLERS)
	{
		dataBuffer[0] |= this->channelMIDI - 1;
		fifoMidiFromGUI.addToFifo(dataBuffer, size);
		this->notify();
		return 1;
	} else if (commandNum == COMMAND_REQUEST_TEST_MIDI)
	{
		fifoMidiFromGUI.addToFifo(dataBuffer, size);
		this->notify();
		return 1;
	}
	bool retVal = false;
	if (getHoldingWindow() != nullptr && isBusySendingGuiCommand)
		return retVal; // already busy
	sendArray.clear();
	switch (commandNum)
	{
		default:
			break;
		case COMMAND_REQUEST_GET_INFO:
			retVal = true;
			break;
		case COMMAND_REQUEST_GET_BULK:
			retVal = true;
			break;
		case COMMAND_REQUEST_GET_GLOBALS:
			retVal = true;
			break;
		case COMMAND_REQUEST_PATCH:
		case COMMAND_REQUEST_PATTERN:
		case COMMAND_REQUEST_SEQUENCE:
			retVal = true;
			break;

		case COMMAND_REQUEST_SEND_ACTIVE_PATCH:
			retVal = true;
			break;

		case COMMAND_REQUEST_PATCH_BANK:
		case COMMAND_REQUEST_PATTERN_BANK:
		case COMMAND_REQUEST_SEQUENCE_BANK:

		case COMMAND_REQUEST_SEND_BULK:
		case COMMAND_REQUEST_SEND_PROGRAM_CHANGE:
		case COMMAND_REQUEST_PATCH_CHANGE:
		case COMMAND_REQUEST_RESET_PARAMETER_PAGE:
		case COMMAND_REQUEST_STATUS:
		case COMMAND_REQUEST_PATCH_NAME_CHANGE:
			for (int i = 0; i < size; i++)
			{
				sendArray.add(dataBuffer[i]);
			}
			retVal = true;
			break;
		case COMMAND_REQUEST_SEND_FIRMARE:
			retVal = true;
			for (int i = 0; i < size; i++)
			{
				if (dataBuffer[i] > 0x7F)
				{
					 retVal = false;
					 break;
				}
				sendArray.add(dataBuffer[i]);
			}
			break;
		case COMMAND_REQUEST_SEND_GLOBALS:
			if (size == SETUP_DATA_LENGTH_BYTES)
			{
				uint8_t* outBuffer = new uint8_t[size * 2];
				uint32_t bytesToSend  = Utilities::prepareSysExPacketsFromUnpacked(outBuffer, dataBuffer,
						(uint32_t) size,  MIDI_SYSEX_GLOBALS_TRANSFER, 0, 0, 0, motasModel, channelMIDI);
				for (uint32_t i = 0; i < bytesToSend; i++)
				{
					sendArray.add(outBuffer[i]);
				}
				delete [] outBuffer;
				retVal = true;
			} else
			{
				DBG("Size of setup data is wrong");
			}
			break;
	}
	if (retVal)
	{
		activeCommand = commandNum;
		isBusySendingGuiCommand = true;
		this->notify();
	}
	return retVal;
}

int MIDIProcessor::threadFunction()
{


#if TEST_MALLOC_CRASH > 0
	for (int i = 0; i < 50000; i++)
	{
		String msg = "TEST OF MALLOC CRASH... " + String((int) randomNumber++ ) + " bytes";
		sendMessageToHw(msg, i % 4);

	// it seems that what I was doing with StringArray storing message strings
	//  was NOT thread safe and crashed malloc randomly... changed method to simply using a
	//  char buffer shared between the GUI and worker threads.
	}
#endif
	//sendMessageToHw(String::createStringFromData(bitmapBuffer, 256), 0);
	// because this is a background thread, we mustn't do any UI work without
	// first grabbing a MessageManagerLock..
	doTasks();
	sysExDoProcessing();

	if (patchDataEdited)
	{
		this->dataChanged = true;
		patchDataEdited = false;

		tAllParams* patch = nullptr;
	#if BUILD_STANDALONE_EXECUTABLE == 0
		patch = (tAllParams*) pluginProcessor->appCommonPtr->getActivePatch();//gPatchInUse; // pointer to patch in use
	#else
		if (getHoldingWindow() != nullptr)
		{
			patch = (tAllParams*) getHoldingWindow()->appCommon->getActivePatch();//gPatchInUse; // pointer to patch in use
		}
	#endif


		if (patch != nullptr)
			crc.updatePatchCRC(patch);

	}
	if (!guiBusyUpdating) // do nothing if GUI thread already doing something
	{
		if (dataChanged)
		{

#if BUILD_STANDALONE_EXECUTABLE == 0
			pluginProcessor->appCommonPtr->setGUIPatch();
#else
			if (getHoldingWindow() != nullptr)
			{
				getHoldingWindow()->appCommon->setGUIPatch();
			}
#endif
			dataChanged = false;
		}
		if (guiNeedsUpdate)
		{
			guiNeedsUpdate = false;
			guiBusyUpdating = true;
		}
		return 500;
	} else // GUI is busy, but needs updating so sleep for a shorter time
	{
		// sleep a bit so the threads don't all grind the CPU to a halt..
		return 10;
	}

}

