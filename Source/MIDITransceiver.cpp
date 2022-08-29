
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
 */
#include "Headers/MIDITransceiver.h"
#include "./Headers/MIDIConnection.h"

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

/**
 *
 * Deals with the MIDI communication stuff
 */


#if BUILD_STANDALONE_EXECUTABLE == 0



MIDITransceiver::MIDITransceiver(MotasEditPluginAudioProcessor* p)
: pluginProcessor(p)
{


		busySendingSysEx = false;
		MIDItransferType = 0;

		MIDISendRate = 11000;
		allowMIDIThru = false;
		MIDIsysExFullyReceived = true;
		screenShotPostponed =false;
		motasModel = 6;
		channelMIDI = 1;
		allowForwardSyExToDAW = false;

		midiDataInFromMotas = false;
		midiDataOutToMotas = false;
		midiSysExDataOutToMotas = false;
		retryPatchSendCount = 0;
		activePatchCRC32 = 0;
		midiSysExDataInFromMotas = false;
}

#else
MIDITransceiver::MIDITransceiver()
{


		busySendingSysEx = false;
		MIDItransferType = 0;

		MIDISendRate = 11000;
		allowMIDIThru = false;
		MIDIsysExFullyReceived = true;
		screenShotPostponed =false;
		motasModel = 6;
		channelMIDI = 1; // for some stupid reason JUCE expects value to be from 1... 16
		allowForwardSyExToDAW = false;

		midiDataInFromMotas = false;
		midiDataOutToMotas = false;
		midiSysExDataOutToMotas = false;
		retryPatchSendCount = 0;
		activePatchCRC32 = 0;
		midiSysExDataInFromMotas = false;
		midiRoutingBitfield = 0x07; // default turn on only MotasEdit data to Motas
}
#endif



// MUST provide the destructor if we specified it in the header file declaration otherwise get linker error.
MIDITransceiver::~MIDITransceiver()
{

}

/**
 *
 *
 * Send out MIDI data to either direct MIDI or onto FIFO (in case of plugin)
 */

bool MIDITransceiver::sendToOutputs(const MidiBuffer& msg, bool sendNow, uint32_t index, int type,const String & text)
{


	bool retryLater = false;
	bool sendToMotas = false;
	bool sendToDAW = false;
	switch (type)
	{
		default:
		case MIDI_DATA_SEND_TYPE_UNKNOWN:
			DBG("Default data send  - no send at all!");
			break;
		case MIDI_DATA_SEND_TYPE_NRPN_FROM_MOTAS_EDIT:
			if (midiRoutingBitfield & MOTASEDIT_NRPN_TO_MOTAS)
				sendToMotas = true;
			if (midiRoutingBitfield & MOTASEDIT_NRPN_TO_DAW)
				sendToDAW = true;
			break;
		case MIDI_DATA_SEND_TYPE_NOTES_FROM_MOTAS_EDIT:
		case MIDI_DATA_SEND_TYPE_CONTROLLER_FROM_MOTAS_EDIT:
		case MIDI_DATA_SEND_TYPE_PROGRAM_CHANGE_MOTAS_EDIT:
			if (midiRoutingBitfield & MOTASEDIT_OTHER_TO_MOTAS)
				sendToMotas = true;
			if (midiRoutingBitfield & MOTASEDIT_OTHER_TO_DAW)
				sendToDAW = true;
			break;
		case MIDI_DATA_SEND_TYPE_NRPN_FROM_DAW:
			if (midiRoutingBitfield & DAW_NRPN_TO_MOTAS)
				sendToMotas = true;
			if (midiRoutingBitfield & DAW_NRPN_TO_DAW)
				sendToDAW = true;
			break;
		case MIDI_DATA_SEND_TYPE_OTHER_FROM_DAW:
		case MIDI_DATA_SEND_TYPE_NOTES_FROM_DAW:
		case MIDI_DATA_SEND_TYPE_PITCH_WHEEL_FROM_DAW:
		case MIDI_DATA_SEND_TYPE_CONTROLLERS_FROM_DAW:
			if (midiRoutingBitfield & DAW_OTHER_TO_MOTAS)
				sendToMotas = true;
			if (midiRoutingBitfield & DAW_OTHER_TO_DAW)
				sendToDAW = true;
			break;
		case MIDI_DATA_SEND_TYPE_NRPN_FROM_MOTAS: // don't send this back to Motas!
			if (midiRoutingBitfield & MOTAS_NRPN_TO_DAW)
				sendToDAW = true;
			break;
		case MIDI_DATA_SEND_TYPE_SYSEX_FROM_MOTAS_EDIT: // patch data for example
		case MIDI_DATA_SEND_TYPE_SYSEX_FIRMWARE_FROM_MOTAS_EDIT: // new motas firmware transfer
		case MIDI_DATA_SEND_TYPE_SYSEX_REQUEST_FROM_MOTAS_EDIT: // request for SysEx data
			if (midiRoutingBitfield & MOTASEDIT_SYSEX_TO_MOTAS)
				sendToMotas = true;
			if (midiRoutingBitfield & MOTASEDIT_SYSEX_TO_DAW)
				sendToDAW = true;
			break;
		case MIDI_DATA_SEND_TYPE_SYSEX_FROM_DAW:
			if (midiRoutingBitfield & DAW_SYSEX_TO_MOTAS)
				sendToMotas = true;
			if (midiRoutingBitfield & DAW_SYSEX_TO_DAW)
				sendToDAW = true;
			break;
		case MIDI_DATA_SEND_TYPE_SYSEX_FROM_MOTAS: // could be patch data sent by Motas
			if (midiRoutingBitfield & MOTAS_SYSEX_TO_DAW)
				sendToDAW = true;
			break;
		case MIDI_DATA_SEND_TYPE_OTHER_FROM_MOTAS:
			if (midiRoutingBitfield & MOTAS_OTHER_TO_DAW)
				sendToDAW = true;
			break;
	}


	if (sendToDAW)
	{


#if BUILD_STANDALONE_EXECUTABLE == 0
	#if SEND_MIDI_TO_DAW_PROCESS_BLOCK == 1
			sendMessageToHw(text, HoldingWindowComponent::MESSAGE_TYPE_TO_DAW);
			sendToOutgoingFIFOToDAW(msg);
			DBG("Adding data to out FIFO");
	#endif
#else
//		DBG("Send MIDI to DAW OUT");
		if (!sendToOutputsLowLevel(&midiOutputs, msg, sendNow, index))
		{
			String m = "No DAW devices available";
			sendMessageToHw(m, HoldingWindowComponent::MESSAGE_TYPE_TO_DAW);

		} else
		{
			sendMessageToHw(text, HoldingWindowComponent::MESSAGE_TYPE_TO_DAW);

		}
#endif
	}

	if (sendToMotas)
	{
		const int timeOffsetms = 0;
//		DBG("Sending MIDI to Motas, getNumEvents:" + String(msg.getNumEvents()));
		static uint32_t timeToSend = 0;
		uint32_t timeNow = Time::getMillisecondCounter();
		// if it is now later than the end of the last data to be sent
		// or we are sending the many packets in one transfer (index > 0)

		if ((index > 0) || (timeNow >= (timeToSend + timeOffsetms)))
		{
			if (sendNow)
			{
				if (timeNow > timeOffsetms)
					timeToSend = timeNow - timeOffsetms;
				else
					timeToSend = timeNow;

			} else
			{
				// since are sending large amounts of data, don't send screenshot requests
				//busySendingSysEx = true;
				static const int  SEND_BLOCK_OF_MESSAGE_TIME_OFFSET  = 20;
				timeToSend = timeNow + SEND_BLOCK_OF_MESSAGE_TIME_OFFSET + (uint32_t) ((index * 1000.0f) / MIDISendRate);

				if (timeToSend > timeNow + 5 * 60 * 1000) // check for a crazy time in the future, just in case!
				{
					timeToSend = timeNow + 5 * 60 * 1000; // 5 mins in the future
				}

//				DBG("Time now: " + String(timeNow) + " time to send: " + String(timeToSend));
			}
			if (!sendToOutputsLowLevel(&midiOutputsMotas, msg, sendNow, timeToSend, index))
			{
				midiDataOutToMotas = false;
				String m("No Motas device available.");
				DBG ("No Motas device available.");
				sendMessageToHw(m, HoldingWindowComponent::MESSAGE_TYPE_TO_MOTAS);
			} else
			{

				switch (type)
				{
					default:
						break;
					case MIDI_DATA_SEND_TYPE_NRPN_FROM_MOTAS_EDIT:
					case MIDI_DATA_SEND_TYPE_NOTES_FROM_MOTAS_EDIT:
					case MIDI_DATA_SEND_TYPE_CONTROLLER_FROM_MOTAS_EDIT:
					case MIDI_DATA_SEND_TYPE_PROGRAM_CHANGE_MOTAS_EDIT:
					case MIDI_DATA_SEND_TYPE_NRPN_FROM_DAW:
					case MIDI_DATA_SEND_TYPE_OTHER_FROM_DAW:
					case MIDI_DATA_SEND_TYPE_NOTES_FROM_DAW:
					case MIDI_DATA_SEND_TYPE_PITCH_WHEEL_FROM_DAW:
					case MIDI_DATA_SEND_TYPE_CONTROLLERS_FROM_DAW:
						midiDataOutToMotas = true;
						break;
					case MIDI_DATA_SEND_TYPE_SYSEX_FROM_MOTAS_EDIT: // patch data for example
					case MIDI_DATA_SEND_TYPE_SYSEX_FIRMWARE_FROM_MOTAS_EDIT: // new motas firmware transfer
					case MIDI_DATA_SEND_TYPE_SYSEX_REQUEST_FROM_MOTAS_EDIT: // request for SysEx data
					case MIDI_DATA_SEND_TYPE_SYSEX_FROM_DAW:
						midiSysExDataOutToMotas = true;
						break;

				}
				sendMessageToHw(text, HoldingWindowComponent::MESSAGE_TYPE_TO_MOTAS);
			}
		} else
		{
			DBG("Already busy sending... not sent!");
			retryLater = true;
			sendMessageToHw("Already busy sending!", HoldingWindowComponent::MESSAGE_TYPE_TO_MOTAS);
		}
	}
	return retryLater;
}


#if BUILD_STANDALONE_EXECUTABLE == 0

void MIDITransceiver::sendToOutgoingFIFOToDAW(const MidiBuffer& buf)
{
	MidiBuffer::Iterator midi_buffer_iter(buf);
	const uint8_t* mm;
	int numBytes;
	int samplePosition;

	while (midi_buffer_iter.getNextEvent(mm, numBytes, samplePosition))
	{
/*
		DBG("Bytes written "  + String(numBytes)
				+ String(" ") + String(mm[0])
				+ String(" ") + String(mm[1])
				+ String(" ") + String(mm[2]));

*/

		pluginProcessor->fifoMidiUI.addToFifo(mm, numBytes);

	}

}

#endif



void MIDITransceiver::updateMIDIReceiveSysExByte(uint8_t data)
{

	static uint32_t numBytesReceived = 0;


    bool final = FALSE;
    static int32_t indexF0 = -1;
    uint8_t type = 0;

  //  if (this->MIDIsysExFullyReceived == true) // still busy processing previous batch of data
   //     return;

    if (data == 0xF0) // start packet seen
    {

    	DBG("MIDI sysex start 0xF0 num bytes received: " + String(numBytesReceived));
        indexF0 = (int32_t) numBytesReceived;
        String msg = "Received SysEx... " + String((int) numBytesReceived ) + " bytes";
        sendMessageToHw(msg, HoldingWindowComponent::MESSAGE_TYPE_FROM_MOTAS);

    }

    this->writeMidiStreamData->add(data);
    numBytesReceived++;
    bool abortEarly = FALSE;
    if (data == 0xF7) // end of packet
    {
    //	 DBG("MIDI sysex full packet received num bytes:"  + String(numBytesReceived) + " indexF0: " + String (indexF0)) ;
         if (indexF0 >= 0 && (numBytesReceived > (uint32_t) (indexF0 + 8)) )
         {
            if (this->writeMidiStreamData->getUnchecked(indexF0 + 8) ==  MIDI_SYSEX_END_TRANSFER)
            {
                final = TRUE;
                DBG("FINAL = true");
                type = this->writeMidiStreamData->getUnchecked(indexF0  + 7);
            } else
            {
            	DBG("not final sysex " + String(numBytesReceived)
            			+ " value in stream: " + String(this->writeMidiStreamData->getUnchecked(indexF0 + 8)));
            }
            //        DBG("MIDI sysex decode");

            for (int i = 0; i < 7; i++)
            {
                int val = this->writeMidiStreamData->getUnchecked(indexF0 + i);
                switch (i)
                {
                    case 0:
                        if (val != 0xF0)
                        {
                        	DBG(" error val: " + String(val) );
                            abortEarly = TRUE;
                        }
                        break;
                    case 1:
                        if (val != MIDI_stuff::BYTE_SYSEX_ID1)
                        {
							DBG(" error val: " + String(val) );
							abortEarly = TRUE;
						}
                        break;
                    case 2:
                        if (val != MIDI_stuff::BYTE_SYSEX_ID2)
                        {
							DBG(" error val: " + String(val) );
							abortEarly = TRUE;
						}
                        break;
                    case 3:
                        if (val != MIDI_stuff::BYTE_SYSEX_ID3 )
                        {
							DBG(" error val: " + String(val) );
							abortEarly = TRUE;
						}
                        break;
                    case 4:
              //          if (val != ui->spinBoxMotasModel->value())
               //             abortEarly = TRUE;
                        break;
                    case 5:
                        // channel, ignored.
                       // if (val != MIDI_stuff::BYTE_SYSEX_PRODUCT_ID_2 )
                       //    abortEarly = TRUE;
                        break;
                    case 6:

                        break;
                    default:
                        break;
                }
                if (abortEarly)
                	break;
            }
            if (abortEarly)
            {
                sendMessageToHw("aborted - SysEx not received correctly", HoldingWindowComponent::MESSAGE_TYPE_FROM_MOTAS);
                DBG(" aborted - SYSex received not correctly from Motas bytes: " + String(numBytesReceived) );
                final = true;
            }
        }
        indexF0 = -1;
    }

    if (final)
    {

    	if (abortEarly)
    	{
    		writeMidiStreamData->clear();
    	} else
    	{
    		DBG("SysEx data ok, bytes: " + String(numBytesReceived));


			// swap the buffers over
			if (this->writeMidiStreamData == &this->midiStreamData1)
			{
				this->writeMidiStreamData = &this->midiStreamData2;
				this->readMidiStreamData = &this->midiStreamData1;
			}
			else
			{
				this->writeMidiStreamData = &this->midiStreamData1;
				this->readMidiStreamData = &this->midiStreamData2;
			}
			MIDItransferType = type;
			MIDIsysExFullyReceived = true;
    	}
    	numBytesReceived = 0;

    }
}

void MIDITransceiver::processNRPN(const uint8_t* data, NRPNmessage_t& NRPNMessage, NRPNinit_t &NRPNInitialised)
{

	if (data[1] == 0x63) //  NRPN Most Significant Bit (MSB)
	{
		NRPNInitialised.bits.NRPN_paramMSBReceived = 1;
		NRPNMessage.parameterMSB = data[2];
	}
	else if (data[1] == 0x62)  //NRPN Least Significant Bit (LSB)
	{
		NRPNInitialised.bits.NRPN_paramLSBReceived = 1;
		NRPNMessage.parameterLSB = data[2];
	}
	else if (data[1] == 0x06) // data controller
	{
		NRPNInitialised.bits.NRPN_dataMSBReceived = 1;
		NRPNMessage.valueMSB =  data[2];
	}
	else if (data[1] == 0x26
			&& NRPNInitialised.bits.NRPN_paramMSBReceived
			&& NRPNInitialised.bits.NRPN_paramLSBReceived
			&& NRPNInitialised.bits.NRPN_dataMSBReceived)// LSB
	{
		// if we just send LSB data then will update the same parameter (since assumed parameter and MSB from previous transfers)
		NRPNMessage.valueLSB =  data[2];
		processMIDIControllerChangeMessage(&NRPNMessage);

		DBG("Process change message NRPN");
	}
}




void MIDITransceiver::processMidiInMessage(const MidiMessage & mm, int source)
{
	String midiString;
	//DBG("Handle Message");
	const uint8_t* data =  mm.getRawData();
	MidiBuffer buf;
	buf.addEvent(mm, 1);
	if (mm.isSysEx())
	{
		if (source == MIDI_DATA_SOURCE_MOTAS)
		{
			// incoming SYSEX from Motas

			midiSysExDataInFromMotas = true;
			sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_SYSEX_FROM_MOTAS, String("SysEx pass through"));
			//int length = mm.getRawDataSize();
			//DBG("Length of SYSEX data: " + String(length));
			//DBG("SYSEX MIDI message received");
			for (int32_t i = 0; i < mm.getRawDataSize(); i++)
			{
				uint8_t b = mm.getRawData()[i];
			//		DBG("byte " + String(i) + " " + String((int) b));
				updateMIDIReceiveSysExByte(b);
			}
		} else if (source == MIDI_DATA_SOURCE_DAW)
		{
			sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_SYSEX_FROM_DAW, String("SysEx from DAW")); // send to Motas
			String msg = "SyEx data received " + String((int)mm.getRawDataSize()) + " bytes";
			sendMessageToHw(msg, HoldingWindowComponent::MESSAGE_TYPE_FROM_DAW);
		}
	}
	else
	{
		bool MIDInrpn = false;
		if (mm.isController() ) // Motas knob/button value change from panel
		{
			uint8_t f = (uint8_t) mm.getControllerNumber();
			//const uint8_t* data =  mm.getRawData();

			const char* name  = MidiMessage::getControllerName(static_cast<int>(data[1]));
			if (name != nullptr)
				midiString << name;
			else
				midiString << String("controller: ") + String(static_cast<int> (data[1]));
			midiString << " ";
			midiString << static_cast<int> (data[2]);


			DBG(midiString);


			// check is one of the 4 NRPN codes
			if (f == 0x63)
				MIDInrpn = true;
			else if (f == 0x62)
				MIDInrpn = true;
			else if (f == 0x06)
				MIDInrpn = true;
			else if (f == 0x26)
				MIDInrpn = true;

			if (MIDInrpn) // NRPN panel change data from Motas
			{
				String m = "Sent MIDI Thru data";
				DBG("New NRPN");
#if BUILD_STANDALONE_EXECUTABLE == 0
				if (source == MIDI_DATA_SOURCE_MOTAS)
				{
					midiDataInFromMotas = true;
					sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_NRPN_FROM_MOTAS, m);
					static NRPNmessage_t NRPNMessage;
					static NRPNinit_t NRPNInitialised = {0};
					// process the incoming NRPN message from live edit on Motas-6, i.e. update slider values etc.
					processNRPN(mm.getRawData(), NRPNMessage, NRPNInitialised);

					uint8_t buffer[4];

					buffer[0] = NRPNMessage.parameterMSB;
					buffer[1] = NRPNMessage.parameterLSB;
					buffer[2] = NRPNMessage.valueMSB;
					buffer[3] = NRPNMessage.valueLSB;

					// now add to the DAW fifo (since we cannot directly send to DAW in this non-GUI thread (at least with Ableton..)
					fifoParamChangesFromMotasToDAW.addToFifo(buffer, 4);
					sendMessageToHw(midiString, HoldingWindowComponent::MESSAGE_TYPE_FROM_MOTAS);
					//updateGUI();
				} else if (source == MIDI_DATA_SOURCE_DAW)
				{
					sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_NRPN_FROM_DAW, midiString);
					sendMessageToHw(midiString, HoldingWindowComponent::MESSAGE_TYPE_FROM_DAW);
					static NRPNmessage_t NRPNMessageDAW;
					static NRPNinit_t NRPNInitialisedDAW = {0};
					// process the incoming NRPN message from DAW
					processNRPN(mm.getRawData(), NRPNMessageDAW, NRPNInitialisedDAW);
				}

#else
				// relay the incoming NRPN to the DAW
				if (source == MIDI_DATA_SOURCE_DAW)
				{
					sendToOutputs(buf, true, 1, MIDI_DATA_SEND_TYPE_NRPN_FROM_DAW, m);
					sendMessageToHw(midiString, HoldingWindowComponent::MESSAGE_TYPE_FROM_DAW);
				}
				else
				{
					midiDataInFromMotas = true;
					sendToOutputs(buf, true, 1, MIDI_DATA_SEND_TYPE_NRPN_FROM_MOTAS, m);
					sendMessageToHw(midiString, HoldingWindowComponent::MESSAGE_TYPE_FROM_MOTAS);
				}

				// we need a separate record of NRPN history so we can know what parameter to change
				static NRPNmessage_t NRPNMessage;
				static NRPNinit_t NRPNInitialised = {0};
				// process the incoming NRPN message from live edit on Motas-6, i.e. update slider values etc.
				processNRPN(mm.getRawData(), NRPNMessage, NRPNInitialised);
		// ??		updateGUI();
#endif

			} else
			{
				if (source != MIDI_DATA_SOURCE_MOTAS)
				{
					if (source == MIDI_DATA_SOURCE_DAW)
					{
						sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_CONTROLLERS_FROM_DAW, midiString); // send to Motas
						sendMessageToHw(midiString, HoldingWindowComponent::MESSAGE_TYPE_FROM_DAW);
					}
					else
					{
						sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_CONTROLLER_FROM_MOTAS_EDIT, midiString); // send to Motas
					}
				}
			}

		} else if (mm.isNoteOn() || mm.isNoteOff())
		{

			midiString << (mm.isNoteOn() ? String ("Note on: ") : String ("Note off: "));
			midiString << (MidiMessage::getMidiNoteName (mm.getNoteNumber(), true, true, true));
			midiString << (String (" vel = "));
			midiString << static_cast<int> (mm.getVelocity());
			DBG(midiString);
			if (source == MIDI_DATA_SOURCE_MOTAS)
			{
				sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_OTHER_FROM_MOTAS, midiString);
				sendMessageToHw(midiString, HoldingWindowComponent::MESSAGE_TYPE_FROM_MOTAS);
			}
			else if (source == MIDI_DATA_SOURCE_DAW)
			{
				sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_NOTES_FROM_DAW, midiString); // send to Motas
				sendMessageToHw(midiString, HoldingWindowComponent::MESSAGE_TYPE_FROM_DAW);
			}
			else
			{
				sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_NOTES_FROM_MOTAS_EDIT, midiString); // send to Motas
			}

		} else if (mm.isAftertouch())
		{
			midiString << "Aftertouch" ;
			midiString << " ";
			midiString << static_cast<int> (data[1]);
			midiString << " ";
			midiString << static_cast<int> (data[2]);
			DBG(midiString);
			if (source == MIDI_DATA_SOURCE_MOTAS)
			{
				sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_OTHER_FROM_MOTAS, midiString);
				sendMessageToHw(midiString, HoldingWindowComponent::MESSAGE_TYPE_FROM_MOTAS);
			}
			else if (source == MIDI_DATA_SOURCE_DAW)
			{
				sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_OTHER_FROM_DAW, midiString); // send to Motas
				sendMessageToHw(midiString, HoldingWindowComponent::MESSAGE_TYPE_FROM_DAW);
			}


		} else if (mm.isPitchWheel())
		{
			midiString << "Pitch Wheel" ;
			midiString << " ";
			midiString << static_cast<int> (data[1]);
			midiString << " ";
			midiString << static_cast<int> (data[2]);
			DBG(midiString);
			if (source == MIDI_DATA_SOURCE_MOTAS)
			{
				sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_OTHER_FROM_MOTAS, midiString);
				sendMessageToHw(midiString, HoldingWindowComponent::MESSAGE_TYPE_FROM_MOTAS);
			}
			else if (source == MIDI_DATA_SOURCE_DAW)
			{
				sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_PITCH_WHEEL_FROM_DAW, midiString); // send to Motas
				sendMessageToHw(midiString, HoldingWindowComponent::MESSAGE_TYPE_FROM_DAW);
			}


		}
		else
		{
			int length = mm.getRawDataSize();
			midiString << String(length) + " bytes ";
			for (int i = 0; i < length; i++)
			{
				//midiString << String("0x")  + String::toHexString(static_cast<int> ( mm.getRawData()[i])).toUpperCase();//static_cast<int> ( mm.getRawData()[i]);
				midiString << static_cast<int> (data[i]);//static_cast<int> ( mm.getRawData()[i]);
				midiString << " ";
			}
			if (source == MIDI_DATA_SOURCE_MOTAS)
			{
				sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_OTHER_FROM_MOTAS, midiString);
				sendMessageToHw("data received", HoldingWindowComponent::MESSAGE_TYPE_FROM_MOTAS);
			}
			else if (source == MIDI_DATA_SOURCE_DAW)
			{
				sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_OTHER_FROM_DAW, midiString); // send to Motas
				sendMessageToHw("data received", HoldingWindowComponent::MESSAGE_TYPE_FROM_DAW);
			}
			DBG("Length of data: " + String(length));

		}

	}

}


void MIDITransceiver::sendProgramChange(int newProgram)
{

	MidiBuffer buf;
	MidiMessage m0 = MidiMessage::programChange(channelMIDI, newProgram);
	buf.addEvent(m0, 1);
	String m = "program change " + String((int) newProgram);
	sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_PROGRAM_CHANGE_MOTAS_EDIT, m); // send to Motas

	String msg =  String("Bytes sent: 0xC0 0x" + String::toHexString(newProgram).toUpperCase());
	//sentMIDIMessageText.setText(msg, NotificationType::dontSendNotification);
}



bool MIDITransceiver::sendSysEx(String name, char msg1, char msg2)
{
	switch (msg1)
	{
		case MIDI_SYSEX_REQUEST_BITMAP:
		case MIDI_SYSEX_REQUEST_BITMAP_COMPRESSED:
		case MIDI_SYSEX_REQUEST_STATUS:
			break;
		default:
			busySendingSysEx = true;
			break;
	}
	DBG("Sending sysex command: " + name);
	Array<uint8_t> message;
	message.add(MIDI_stuff::BYTE_SYSEX_START);
	message.add(MIDI_stuff::BYTE_SYSEX_ID1);
	message.add(MIDI_stuff::BYTE_SYSEX_ID2);
	message.add(MIDI_stuff::BYTE_SYSEX_ID3);
	message.add(this->motasModel); // motas model
	if (this->channelMIDI > 0)
		message.add(this->channelMIDI - 1); // midi channel
	else
		message.add(this->channelMIDI); // midi channel
	message.add( 0);
	message.add((uint8_t) msg1);
	message.add((uint8_t) msg2);
	message.add(MIDI_stuff::BYTE_SYSEX_END);

	MidiBuffer msg;
	msg.addEvent(message.getRawDataPointer(), message.size(), 1);
	String m = "SysEx: " + name;
	return sendToOutputs(msg, true, 0, MIDI_DATA_SEND_TYPE_SYSEX_REQUEST_FROM_MOTAS_EDIT, m);
}

void MIDITransceiver::sendPatch(uint8_t preset, uint8_t* patchBuffer)
{

	//patchBuffer[100] = 99;

	activePatchCRC32 = crc.crcFastSTMF4Patch((tAllParams*) patchBuffer);
	uint32_t bytesToSend;
	bytesToSend = Utilities::prepareSysExPacketsFromUnpacked(rawTransferBuffer,
			  patchBuffer, PATCH_LENGTH_BYTES_UNPACKED, MIDI_SYSEX_PATCH_TRANSFER, 0x7E, preset, 0, motasModel, channelMIDI);
	DBG("bytes to send (in MIDI form):"  + String( bytesToSend));
	if (sendDataOverMIDIDirect(rawTransferBuffer, bytesToSend, 0))
	{
		DBG("Cannot send - busy");
	}
}





/*
void MIDITransceiver::processMidiInMessage(const MidiMessage& mm)
{
	String midiString;
	const uint8_t* data =  mm.getRawData();

	MidiBuffer buf;
	buf.addEvent(mm, 1);

	if (mm.isSysEx())
	{
		sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_SYSEX_FROM_DAW, String("SysEx from DAW")); // send to Motas
		String msg = "SyEx data received " + String((int)mm.getRawDataSize()) + " bytes";

		sendMessageToHw(msg, HoldingWindowComponent::MESSAGE_TYPE_FROM_DAW);

	} else
	{
		if (mm.isNoteOn() || mm.isNoteOff())
		{
			midiString << (mm.isNoteOn() ? String ("Note on: ") : String ("Note off: "));
			midiString << (MidiMessage::getMidiNoteName (mm.getNoteNumber(), true, true, true));
			midiString << (String (" vel = "));
			midiString << static_cast<int> (mm.getVelocity());
			sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_NOTES_FROM_DAW, midiString); // send to Motas
		} else
		{
			int length = mm.getRawDataSize();
			if (length == 3)
			{
				if (mm.isController())
				{
					bool MIDInrpn = false;
					uint8_t f = mm.getControllerNumber();
					if (f == 99)
						MIDInrpn = true;
					else if (f == 98)
						MIDInrpn = true;
					else if (f == 6)
						MIDInrpn = true;
					else if (f == 38)
						MIDInrpn = true;
					MidiBuffer msgbuf;
					const char* name  = MidiMessage::getControllerName(static_cast<int>(data[1]));
					if (name != nullptr)
						midiString << name;
					else
						midiString << String("controller: ") + String(static_cast<int> (data[1]));
					midiString << " ";
					midiString << static_cast<int> (data[2]);
					if (MIDInrpn) // NRPN panel change data from DAW
					{
						msgbuf.addEvent(mm, 1);
						// relay the incoming NRPN to the DAW
						sendToOutputs(msgbuf, true, 0, MIDI_DATA_SEND_TYPE_NRPN_FROM_DAW, midiString);


						static NRPNmessage_t NRPNMessage;
						static NRPNinit_t NRPNInitialised = {0};

						// process the incoming NRPN message from DAW
						processNRPN(mm.getRawData(), NRPNMessage, NRPNInitialised, false);
					} else
					{
						sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_CONTROLLERS_FROM_DAW, midiString); // send to Motas
					}

				}else if (mm.isAftertouch())
				{
					midiString << "Aftertouch" ;
					midiString << " ";
					midiString << static_cast<int> (data[1]);
					midiString << " ";
					midiString << static_cast<int> (data[2]);
					sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_OTHER_FROM_DAW, midiString); // send to Motas

				} else if (mm.isPitchWheel())
				{
					midiString << "Pitch Wheel" ;
					midiString << " ";
					midiString << static_cast<int> (data[1]);
					midiString << " ";
					midiString << static_cast<int> (data[2]);
					sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_PITCH_WHEEL_FROM_DAW, midiString); // send to Motas
				}
				else
				{
					midiString << static_cast<int> (data[0]);
					midiString << " ";
					midiString << static_cast<int> (data[1]);
					midiString << " ";
					midiString << static_cast<int> (data[2]);
					sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_OTHER_FROM_DAW, midiString); // send to Motas
				}

			} else
			{
				midiString << String(length) + " bytes ";
				for (int i = 0; i < length; i++)
				{
					//midiString << String("0x")  + String::toHexString(static_cast<int> ( mm.getRawData()[i])).toUpperCase();//static_cast<int> ( mm.getRawData()[i]);
					midiString << static_cast<int> (data[i]);//static_cast<int> ( mm.getRawData()[i]);
					midiString << " ";
				}
				sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_OTHER_FROM_DAW, midiString); // send to Motas
				DBG("Length of data: " + String(length));
			}
		}
		sendMessageToHw(midiString, HoldingWindowComponent::MESSAGE_TYPE_FROM_DAW);
	}
}
*/


bool MIDITransceiver::sendFirmwareOverMIDI()
{

	uint32_t fileLength = (uint32_t) sendArray.size();

	uint32_t packetSize = 64;
	uint32_t lastPacket = 1 + (fileLength / packetSize);
	uint32_t index = 0;
	for (uint32_t j = 0; j < lastPacket; j++)
	{
		Array<uint8_t> message = Utilities::prepareSysExPacket((int32_t) j, sendArray.getRawDataPointer(), packetSize, fileLength,
				(int32_t) lastPacket, MIDI_SYSEX_FIRMWARE, 0, 0, 0, motasModel, channelMIDI);
		for (int i = 0; i < message.size(); i++)
		{
			rawTransferBuffer[i] = message[i];
		}
		MidiBuffer msgbuf;
		msgbuf.addEvent((void*) rawTransferBuffer, message.size(), 0);
		String m = "Sending Firmware data SysEx" ;
		if (sendToOutputs(msgbuf, false, index, MIDI_DATA_SEND_TYPE_SYSEX_FIRMWARE_FROM_MOTAS_EDIT, m))
			break;
		index += (uint32_t) message.size();
	}
	return 1;
}


int32_t MIDITransceiver::sendDataOverMIDIDirect(uint8_t* buffer, uint32_t len, int32_t packetOffset)
{
	jassert(len < sizeof(rawTransferBuffer));
	//DBG("Send rate: " + String(this->MIDISendRate) + " " + String(this->MIDISendIntervalMilliSeconds));

	if (len >= sizeof(rawTransferBuffer))
		return 0;

	int32_t packetCount = 0;
	int32_t index = packetOffset;
	for (uint32_t j = 0; j < len; )
	{
		MidiBuffer msgbuf;
		uint32_t i;
		int packetSize = 0;
		for (i = 0; i < len; i++)
		{
			if (i + j >= len)
				break;
			unsigned char c = buffer[i + j];
			packetSize++;
			if (c == 0xF7)
			{
				break;
			}
		}
		msgbuf.addEvent((void*) &buffer[j], packetSize, index);
	//	DBG("Send packet: " + String(packet) + " size:"  + String(packetSize));
		String m = "Sending SysEx data...";
		if (sendToOutputs(msgbuf, false, (uint32_t) index, MIDI_DATA_SEND_TYPE_SYSEX_FROM_MOTAS_EDIT, m))
		{
			DBG("sendToOutputs too busy");
			break;
		}

		index += packetSize;
		j += i + 1;
		packetCount++;
	}
	return index ;
}



bool MIDITransceiver::sendToOutputsLowLevel(ReferenceCountedArray<MidiDeviceListEntry, CriticalSection>  *midiOutput,
		const MidiBuffer& msg, bool sendNow, double timeToSend, uint32_t index)
{
	bool sent = false;

/*
 *
 * In windows on Virtual machine get very sluggish slider behaviour in DAW when moving the slider on the MotasEdit
 * when the MIDI out to Motas is enabled adn using 'sendBlockOfMessagesNow' for the NRPNs.
 * This is much improved by sending the MIDI using 'sendBlockOfMessages'
 * in the future, but not really what we want with NRPN data, we want to send 'now.
 * However, testing on a fresh machine with win10 64bit (not a virtual machine) works beautifully with 'sendBlockOfMessagesNow'
 * so assume is a problem with the VM software details.
 *
 */

	for (auto m : *midiOutput)
	{
		if (m->outDevice.get() != nullptr)
		{
			sent = true;
	//		DBG("About to send MIDI data");
			if (sendNow)
			{
				MidiBuffer::Iterator midi_buffer_iter(msg);

	/*			const uint8_t* mm;
				int numBytes;
				int samplePosition;

				while (midi_buffer_iter.getNextEvent(mm, numBytes, samplePosition))
				{
					for (int i = 0; i < numBytes; i++)
						DBG("Final out: " + String("0x" + String::toHexString(mm[i])));
				}
				*/

		//		int64_t ptr = (int64_t) m->outDevice.get();
				// midiOutput->outDevice;
				//if (midiOutput != nullptr && midiOutput->outDevice != nullptr)

				//	MidiBuffer m;
				//	MidiMessage mm(0xC0, 1);
				//	m.addEvent(mm, 1);
				//unsigned char* ptr2 =  msg.data.getRawDataPointer();
	//			DBG("Pointer: " + String(ptr) );//+ " data: " + String(ptr2));
				m->outDevice->sendBlockOfMessagesNow(msg);
			}
			else
			{
	//			DBG("Sending block of messages in the future: " + String(timeToSend) + " time now: " + String (Time::getMillisecondCounter()));
				if (index == 0)
				{
					m->outDevice->startBackgroundThread();
				}
				m->outDevice->sendBlockOfMessages(msg, timeToSend, MIDISendRate);
			//	if ((counter % 100)  ==  0)
			//		DBG("Timestamp: " + String((uint32_t) timeToSend) + " index: "  + String(index));
			}
		}
	}
	return sent;
}


/**
 *
 * Sends MIDI NRPN to MIDI out, thinning the data if already sent the same controller bytes
 * but periodically reseting to send all NRPN bytes
 */
void MIDITransceiver::sendNRPNtoMIDIOut(NRPNmessage_t& n, int type)
{

	static uint8_t lastNRPNParameterLSB = 0xFF;
	static uint8_t lastNRPNDataMSB = 0xFF;
	static uint8_t lastNRPNParameterMSB = 0xFF;


	MidiBuffer msgbuf;
	// Motas Firmware v0x0104 does not need to re-send values that have not changed
	String msg = "";
	MidiMessage mm;
	if (lastNRPNParameterMSB != n.parameterMSB)
	{
		lastNRPNParameterMSB = n.parameterMSB;
		mm = MidiMessage::controllerEvent(channelMIDI, 0x63, n.parameterMSB );
		msgbuf.addEvent(mm, 1);
		//msg += " NRPN MSB " + String::toHexString(n.parameterMSB).toUpperCase();
		msg += " " + String::toHexString(n.parameterMSB).toUpperCase();
	}
	if (lastNRPNParameterLSB != n.parameterLSB)
	{
		lastNRPNParameterLSB = n.parameterLSB;
		mm = MidiMessage::controllerEvent(channelMIDI, 0x62, n.parameterLSB );
		msgbuf.addEvent(mm, 2);
		//msg += " NRPN LSB " + String::toHexString(n.parameterLSB).toUpperCase();
		msg +=  " " +String::toHexString(n.parameterLSB).toUpperCase();
	}
	if (lastNRPNDataMSB != n.valueMSB)
	{
		lastNRPNDataMSB = n.valueMSB ;
		mm = MidiMessage::controllerEvent(channelMIDI, 0x06, n.valueMSB );
		msgbuf.addEvent(mm, 3);
		//msg += " DATA MSB " + String::toHexString(n.valueMSB ).toUpperCase();
		msg += " " + String::toHexString(n.valueMSB ).toUpperCase();
	}
	mm = MidiMessage::controllerEvent(channelMIDI, 0x26, n.valueLSB );
	msgbuf.addEvent(mm, 4);
	//msg += " DATA LSB " + String::toHexString(n.valueLSB).toUpperCase();
	msg += " " + String::toHexString(n.valueLSB).toUpperCase();


	if (type == MIDI_DATA_SEND_TYPE_NRPN_FROM_DAW)
		sendToOutputs(msgbuf, true, 0, MIDI_DATA_SEND_TYPE_NRPN_FROM_DAW, msg + " from DAW");
	else
		sendToOutputs(msgbuf, true, 0, MIDI_DATA_SEND_TYPE_NRPN_FROM_MOTAS_EDIT, msg + " from MotasEdit");


	// if more than x ms has passed then reset to the NRPN system to fully send the next NRPN
	// when there are fast same slider NRPN values data transfer will be quicker as only
	// the changing NRPNs will be sent
	uint32_t time = Time::getApproximateMillisecondCounter();
	static uint32_t lastTime = 0;
	if (time > lastTime + 450)
	{
		lastNRPNParameterLSB = 0xFF;
		lastNRPNDataMSB = 0xFF;
		lastNRPNParameterMSB = 0xFF;
		lastTime = time;
	}


}

int MIDITransceiver::getMIDIChannel()
{
	return this->channelMIDI;
}


void MIDITransceiver::setMIDIChannel(int channel)
{
	if (channel >= 1 && channel <= 16)
	{
		this->channelMIDI = (uint8_t) channel;
	}
}

