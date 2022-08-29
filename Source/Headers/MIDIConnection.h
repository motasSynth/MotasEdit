
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

#ifndef SOURCE_MIDI_CONNECTION_H_
#define SOURCE_MIDI_CONNECTION_H_
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
//#include "stdint.h"
//#include "utilities.h"
#include "ApplicationCommon.h"

class MIDIDeviceListBox;
#include "FifoMidi.h"


const int fifoMidiInFromOthers_Size = 4096*4;
const int fifoMidiInFromMotas_Size = 4096*4;
const int fifoParamChangesFromGUI_Size = 1024;
const int fifoMidiFromGUI_Size = 1024;
const int fifoParamChangesFromMotasToDAW_Size = 1024;


#define TEST_MALLOC_CRASH 0 // set to 0 normally (no testing), 1 to test system that crashes, 2 to test system that SHOULD NOT crash


struct MidiDeviceListEntry : ReferenceCountedObject
{
    MidiDeviceListEntry (const String& deviceName) : name (deviceName) {}
    String name;
    std::unique_ptr<MidiInput> inDevice;
    std::unique_ptr<MidiOutput> outDevice;
    typedef ReferenceCountedObjectPtr<MidiDeviceListEntry> Ptr;
};




struct MidiCallbackMessage : public Message
{
    MidiCallbackMessage (const MidiMessage& msg, const bool b) : message (msg), fromMotas(b) {}
    MidiMessage message;
    bool fromMotas;
};



class HoldingWindowComponent;

class MIDIConnection : private MidiInputCallback, public Thread
{
public:

	MIDIConnection();

	//virtual void processMidiInMessage(const MidiMessage & mm, bool includeNrpn) = 0;



	std::unique_ptr<MIDIDeviceListBox> midiInputSelectorMotas;
	std::unique_ptr<MIDIDeviceListBox> midiOutputSelectorMotas;
	std::unique_ptr<MIDIDeviceListBox> midiInputSelector;
	std::unique_ptr<MIDIDeviceListBox> midiOutputSelector;


	ReferenceCountedArray<MidiDeviceListEntry, CriticalSection> midiInputsMotas;
	ReferenceCountedArray<MidiDeviceListEntry, CriticalSection> midiOutputsMotas;


	ReferenceCountedArray<MidiDeviceListEntry, CriticalSection> midiInputs; // array of MidiDeviceListEntry for the inputs
	ReferenceCountedArray<MidiDeviceListEntry, CriticalSection> midiOutputs;


	int getNumMidiInputs(bool isMotas) const noexcept;
	int getNumMidiOutputs(bool isMotas) const noexcept;

	ReferenceCountedObjectPtr<MidiDeviceListEntry> getMidiDevice (int index, bool isInput , bool isMotas) const noexcept;

	void openDevice (bool isInput, bool isMotas, int index);

	void closeDevice (bool isInput, bool isMotas, int index);

protected:

#define MSG_BUFFER_SIZE 64
	char msgBuffer[4][MSG_BUFFER_SIZE];



	bool isMotasMidiOutConnected;
	void sendMessageToHw(const String& msg, int type);
	void autoConnectMIDI(void);
	String getInfoMessage(int type);

	FifoMidi fifoMidiInFromMotas{fifoMidiInFromMotas_Size};

#if BUILD_STANDALONE_EXECUTABLE == 1
	FifoMidi fifoMidiInFromOthers{fifoMidiInFromOthers_Size};
#endif


	FifoMidi fifoMidiFromGUI{fifoMidiFromGUI_Size};


	FifoMidi fifoParamChangesFromGUI{fifoParamChangesFromGUI_Size};

	FifoMidi fifoParamChangesFromMotasToDAW{fifoParamChangesFromMotasToDAW_Size};




	bool connectedMotasInput;
	bool connectedMotasOutput;
	bool hideNonMotasDevices;
	bool autoConnectMotas;
	Array<uint8_t> midiStreamData1;
	Array<uint8_t> midiStreamData2;
	Array<uint8_t>* readMidiStreamData;
	Array<uint8_t>* writeMidiStreamData;
	virtual HoldingWindowComponent* getHoldingWindow() = 0;
	StringArray latestMessages;

	bool latestMessageRead[4];
private:







	void updateDeviceList (bool isInputDeviceList, bool isMotas);

	ReferenceCountedObjectPtr<MidiDeviceListEntry> findDeviceWithName (const String& name, bool isInputDevice, bool isMotas) const;

	//void handleMessage (const Message& msg);
	void handleIncomingMidiMessage (MidiInput* source, const MidiMessage &message);


	void closeUnpluggedDevices (StringArray& currentlyPluggedInDevices, bool isInputDevice, bool isMotas);
	bool hasDeviceListChanged (const StringArray& deviceNames, bool isInputDevice, bool isMotas);
	int getIndexInDevices(int listIndex, ReferenceCountedArray<MidiDeviceListEntry, CriticalSection>& m, StringArray  deviceNames);




	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MIDIConnection)

};





#endif /* SOURCE_MIDICOMMSNoUI_H_ */
