
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

 */

#ifndef SOURCE_MIDI_TRANSCEIVER_H_
#define SOURCE_MIDI_TRANSCEIVER_H_
#pragma once

#include "ApplicationCommon.h"
#include "ParameterData.h"
#include "stdint.h"
#include "utilities.h"

#include "MIDIConnection.h"
class MotasPluginParameter;
class MIDIDeviceListBox;
#include "crc.h"

#if BUILD_STANDALONE_EXECUTABLE == 0
	#include "PluginProcessor.h"
#endif


#if BUILD_STANDALONE_EXECUTABLE == 0
	class MotasEditPluginAudioProcessor;
#endif



class HoldingWindowComponent;

class MIDITransceiver :  public MIDIConnection
{
public:

#if BUILD_STANDALONE_EXECUTABLE == 0
	MIDITransceiver(MotasEditPluginAudioProcessor* p);
	//MIDITransceiver();
#else
	MIDITransceiver();
#endif

	~MIDITransceiver();

	int32_t sendDataOverMIDIDirect(uint8_t* buffer, uint32_t fileLength, int32_t packetOffset);


protected:

	bool midiDataInFromMotas;
	bool midiDataOutToMotas;
	bool midiSysExDataOutToMotas;
	bool midiSysExDataInFromMotas;
	bool sendToOutputs(const MidiBuffer& msg, bool sendNow, uint32_t index, int type, const String & text);

	bool sendSysEx(String name, char msg1, char msg2);
	void sendPatch(uint8_t preset, uint8_t* patchBuffer);


	void sendProgramChange(int newProgram);

	void sendTestMIDI();

	bool sendFirmwareOverMIDI();
	void populatePatch(uint8_t* buffer);

#if BUILD_STANDALONE_EXECUTABLE == 0

	virtual MotasPluginParameter*  getDAWParameter(int page, int param) = 0;
#endif

	bool sendToOutputsLowLevel(ReferenceCountedArray<MidiDeviceListEntry, CriticalSection>  *midiOutput,
			const MidiBuffer& msg, bool sendNow, double time = 0, uint32_t index = 0);

	virtual void processMIDIControllerChangeMessage(NRPNmessage_t* msg) = 0;
	void processNRPN(const uint8_t* data, NRPNmessage_t& NRPNMessage, NRPNinit_t &NRPNInitialised) ;
	Array<uint8_t> sendArray;
	Array<uint8_t> incomingDataArray;

	bool allowMIDIThru;
public:

	int getMIDIChannel();
	void setMIDIChannel(int channel);


	uint8_t motasModel;
	uint8_t rawTransferBuffer[LARGEST_BUFFER_LENGTH];
protected:


#if BUILD_STANDALONE_EXECUTABLE == 0
	MotasEditPluginAudioProcessor* pluginProcessor;
#endif


	crcCalculator crc;
	enum
	{
		MIDI_DATA_SOURCE_DAW,
		MIDI_DATA_SOURCE_MOTAS,
		MIDI_DATA_SOURCE_MOTAS_EDIT,
	};
	uint32_t activePatchCRC32;
	//String activePatchName;
//	virtual void updateGUI() = 0;
	bool screenShotPostponed;

	bool MIDIsysExFullyReceived;

	//void processMidiInMessage (const MidiMessage& msg);


	bool busySendingSysEx;

	void updateMIDIReceiveSysExByte(uint8_t data);
	//void initNRPN();
	void sendNRPNtoMIDIOut(NRPNmessage_t &msg, int type);
	void processMidiInMessage(const MidiMessage & mm, int source);

	uint32_t MIDISendRate;
	bool allowForwardSyExToDAW;
	uint8_t MIDItransferType;
	int retryPatchSendCount;

# define MOTASEDIT_SYSEX_TO_MOTAS        (1 << 0)
# define MOTASEDIT_NRPN_TO_MOTAS       	 (1 << 1)
# define MOTASEDIT_OTHER_TO_MOTAS      	 (1 << 2)
# define MOTASEDIT_SYSEX_TO_DAW          (1 << 3)
# define MOTASEDIT_NRPN_TO_DAW       	 (1 << 4)
# define MOTASEDIT_OTHER_TO_DAW       	 (1 << 5)

# define DAW_SYSEX_TO_MOTAS        		(1 << 6)
# define DAW_NRPN_TO_MOTAS       	 	(1 << 7)
# define DAW_OTHER_TO_MOTAS      	 	(1 << 8)
# define DAW_SYSEX_TO_DAW         		(1 << 9)
# define DAW_NRPN_TO_DAW       	 		(1 << 10)
# define DAW_OTHER_TO_DAW       		(1 << 11)

# define MOTAS_SYSEX_TO_DAW         	(1 << 12)
# define MOTAS_NRPN_TO_DAW       	 		(1 << 13)
# define MOTAS_OTHER_TO_DAW       		(1 << 14)


	uint16_t midiRoutingBitfield;

protected:

	uint8_t channelMIDI;

private:
	void sendToOutgoingFIFOToDAW(const MidiBuffer& buf);


	ApplicationCommon* getApplicationCommon();


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MIDITransceiver)

};





#endif /* SOURCE_MIDICOMMSNoUI_H_ */
