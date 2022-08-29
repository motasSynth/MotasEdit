

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
 * MIDIConnection.cpp
 *
 */
#include "../JuceLibraryCode/JuceHeader.h"
#include "./Headers/MIDIConnection.h"

#include "Headers/MIDIComms.h"
#include "Headers/HoldingWindowComponent.h"

/**
 *
 * Deals with the MIDI port opening/closing stuff
 */
const int numMessageTypes = 4;


MIDIConnection::MIDIConnection() :  Thread ("MotasMIDIThread", 0),
	 connectedMotasInput(false), connectedMotasOutput(false), hideNonMotasDevices(true),   autoConnectMotas(false)
{

	for (int i = 0; i < 4; i++)
		msgBuffer[i][0] = 0;

	isMotasMidiOutConnected = false;
	midiInputSelectorMotas  =  std::make_unique<MIDIDeviceListBox> ("Midi Input Selector Motas",  *this, true, true);
	midiOutputSelectorMotas = std::make_unique< MIDIDeviceListBox> ("Midi Output Selector Motas", *this, false, true);
	midiInputSelectorMotas->setTooltip("Choose MIDI Motas input(s) to connect to");
	midiOutputSelectorMotas->setTooltip("Choose MIDI Motas output(s) to connect to");



	this->writeMidiStreamData = &this->midiStreamData1;
	this->readMidiStreamData = &this->midiStreamData2;

	for (int i = 0; i < numMessageTypes; i++)
	{
		latestMessages.add(" ");
		latestMessageRead[i] = false;
	}
}




/**
 *
 * Return pointer to MidiDeviceListEntry input or output device according to the supplied, index and whether is from Motas and input/output
 */
ReferenceCountedObjectPtr<MidiDeviceListEntry> MIDIConnection::getMidiDevice (int index, bool isInput, bool isLookingForMotas) const noexcept
{

	if (isLookingForMotas)
		return isInput ? midiInputsMotas[index] : midiOutputsMotas[index];
	else
		return isInput ? midiInputs[index] : midiOutputs[index];

}

String MIDIConnection::getInfoMessage(int type)
{


#if TEST_MALLOC_CRASH == 1
	if (type >= 0 && type < 4)
	{
		return latestMessages[type];
	}else
		return " ";
#else
	if (type >= 0 && type < 4 && msgBuffer[type] != nullptr)
	{
		//
		// read from the buffer, stopping when a zero is hit
		return String::fromUTF8(msgBuffer[type], -1);
	} else
		return " ";
#endif

}




/**
 *
 * Return the index of the supplied array of device names that matches one of the supplied  array of MidiDeviceListEntry items
 */
int MIDIConnection::getIndexInDevices(int listIndex,
		ReferenceCountedArray<MidiDeviceListEntry, CriticalSection>& m, StringArray deviceNames)
{
	String name = m[listIndex]->name;
	for (int i = 0; i < deviceNames.size(); i++)
	{
		 if (deviceNames[i].compare(name) == 0)
		 {
			return i;
		 }
	}
	return -1;
}


/**
 *
 *
 *  // This is called on the MIDI thread by the subsystem. Must return quickly so
 *  just post another message and deal with that later
 *
 *
 *
 */

#if BUILD_STANDALONE_EXECUTABLE == 1
void MIDIConnection::handleIncomingMidiMessage (MidiInput* source, const MidiMessage &m)
 {

	bool fromMotas = false;
	for (auto input : midiInputsMotas)
	{
		if (input->inDevice.get() == source)
		{
		   fromMotas = true;
		   break;
		}
	}
	if (fromMotas)
	{
		DBG("MIDI message received from Motas");
		if (fifoMidiInFromMotas.addToFifo(m.getRawData(), m.getRawDataSize()))
		{
	//		DBG("Post to fifoMidiInFromMotas: " + String( message.getRawDataSize()));

			this->notify();
		} else
		{
			DBG("fifoMidiInFromMotas has NOT ENOUGH ROOM!");
		}

	} else
	{
		DBG("MIDI message received NOT from Motas");

		if (fifoMidiInFromOthers.addToFifo(m.getRawData(), m.getRawDataSize()))
		{
	//		DBG("Post to fifoMidiInFromMotas: " + String( message.getRawDataSize()));

			this->notify();
		} else
		{
			DBG("fifoMidiInFromOthers has NOT ENOUGH ROOM!");
		}


	}
	//postMessage (new MidiCallbackMessage (message, fromMotas));
#else
void MIDIConnection::handleIncomingMidiMessage (MidiInput* , const MidiMessage &m)
	 {

	//postMessage (new MidiCallbackMessage (message, true));

	// add the new event to the incoming FIFO for Motas input
	//int size = ;
	if (fifoMidiInFromMotas.addToFifo(m.getRawData(), m.getRawDataSize()))
	{
//		DBG("Post to fifoMidiInFromMotas: " + String( message.getRawDataSize()));

		this->notify();
	} else
	{
		DBG("fifoMidiInFromMotas has NOT ENOUGH ROOM!");
	}

#endif
 }






/*
 *
 *  This is called via post message in 'handleIncomingMidiMessage'
 *
 */

/*
void MIDIConnection::handleMessage (const Message& msg)
{
	// This is called on the message loop
	auto& mm = dynamic_cast<const MidiCallbackMessage&> (msg).message;
	auto& fromMotas = dynamic_cast<const MidiCallbackMessage&> (msg).fromMotas;
	if (fromMotas)
	{
//		DBG("Handling message from Motas input");
		processMidiInMotasMessage(mm, true);
	}
#if BUILD_STANDALONE_EXECUTABLE == 1
	else
	{
		processMidiInMessage(mm);
	}
#endif

}
*/





void MIDIConnection::openDevice (bool isInput, bool isMotas, int index)
{
	if (isInput)
	{
		  DBG ("MidiDemo::openDevice input");
		  if (!isMotas)
		  {
		#if BUILD_STANDALONE_EXECUTABLE == 1
				jassert (midiInputs[index]->inDevice.get() == nullptr);

				// find the index that matches the name
				int deviceIndex = getIndexInDevices(index, midiInputs, MidiInput::getDevices());

				// THIS SEEMS TO BE A BAD THING TO DO, CRASHES
				//midiInputs[index]->inDevice.reset(MidiInput::openDevice (deviceIndex, this).get());

				midiInputs[index]->inDevice = std::move(MidiInput::openDevice (deviceIndex, this));





				if (midiInputs[index]->inDevice.get() == nullptr)
				{
					  DBG ("MidiDemo::openDevice: open input device for index = " << index << " failed!");
					  return;
				} else
				{
					sendMessageToHw("Opened device.", HoldingWindowComponent::MESSAGE_TYPE_FROM_DAW);
					midiInputs[index]->inDevice->start();
				}
		#endif
		  } else
		  {
				jassert (midiInputsMotas[index]->inDevice.get() == nullptr);
				int deviceIndex = getIndexInDevices(index, midiInputsMotas, MidiInput::getDevices());
				midiInputsMotas[index]->inDevice = std::move(MidiInput::openDevice (deviceIndex, this));
				if (midiInputsMotas[index]->inDevice.get() == nullptr)
				{
				  DBG ("MidiDemo::openDevice: open input device for index = " << index << " failed!");
				  return;
				} else
				{
					 DBG ("MidiDemo::openDevice SUCCESS");
					sendMessageToHw(String("Opened device."), HoldingWindowComponent::MESSAGE_TYPE_FROM_MOTAS);
				}
				midiInputsMotas[index]->inDevice->start();

		  }
		}
		else // is output
		{
		  //initNRPN();
		  DBG ("MidiDemo::openDevice output");
		  if (!isMotas)
		  {
#if BUILD_STANDALONE_EXECUTABLE == 1
			  jassert (midiOutputs[index]->outDevice.get() == nullptr);
			  int deviceIndex = getIndexInDevices(index, midiOutputs, MidiOutput::getDevices());
			  //


			  // THIS SEEMS TO BE A BAD THING TO DO, CRASHES
			  //midiOutputs[index]->outDevice.reset (MidiOutput::openDevice (deviceIndex).get());


			  midiOutputs[index]->outDevice = std::move (MidiOutput::openDevice (deviceIndex));

			  if (midiOutputs[index]->outDevice.get() == nullptr)
			  {
				  DBG ("MidiDemo::openDevice: open output device for index = " << index << " failed!");
			  } else
			  {
				  sendMessageToHw("Opened device.", HoldingWindowComponent::MESSAGE_TYPE_TO_DAW);
			  }
#endif
		  } else // connect Motas output
		  {
			  jassert (midiOutputsMotas[index]->outDevice.get() == nullptr);
			  int deviceIndex = getIndexInDevices(index, midiOutputsMotas, MidiOutput::getDevices());

			  midiOutputsMotas[index]->outDevice = std::move (MidiOutput::openDevice (deviceIndex));

			  if (midiOutputsMotas[index]->outDevice.get() == nullptr)
			  {
				  DBG ("MidiDemo::openDevice: open output device for index = " << index << " failed!");
			  } else
			  {
				  sendMessageToHw("Opened device.", HoldingWindowComponent::MESSAGE_TYPE_TO_MOTAS);
				  isMotasMidiOutConnected = true;
			  }
		  }
	}
}





void MIDIConnection::closeDevice (bool isInput, bool isMotas, int index)
{
	String msg = "Closing device";
	if (isInput)
	{
		DBG ("MidiDemo::close Device input");
		if (!isMotas)
		{
#if BUILD_STANDALONE_EXECUTABLE == 1
			//jassert (midiInputs[index]->inDevice.get() != nullptr);
			if (midiInputs[index].get() != nullptr &&  midiInputs[index]->inDevice != nullptr && midiInputs[index]->inDevice.get() != nullptr)
			{
				midiInputs[index]->inDevice->stop();
				midiInputs[index]->inDevice.reset();
			}else
			{
				msg = "Device removed from system.";
			}
			sendMessageToHw(msg, HoldingWindowComponent::MESSAGE_TYPE_FROM_DAW);
#endif
		} else
		{
			if (midiInputsMotas[index].get() != nullptr &&  midiInputsMotas[index]->inDevice != nullptr &&  midiInputsMotas[index]->inDevice.get() != nullptr)
			{

				midiInputsMotas[index]->inDevice->stop();
				midiInputsMotas[index]->inDevice.reset();
			}else
			{
				msg = "Device removed from system.";
			}
			sendMessageToHw(msg, HoldingWindowComponent::MESSAGE_TYPE_FROM_MOTAS);
		}
	}
	else
	{
		DBG ("MidiDemo::close Device output");
		if (!isMotas)
		{
#if BUILD_STANDALONE_EXECUTABLE == 1
			if (midiOutputs[index].get() != nullptr && midiOutputs[index]->outDevice != nullptr && midiOutputs[index]->outDevice.get() != nullptr)
			{
				midiOutputs[index]->outDevice.reset();
			} else
			{
				msg = "Device removed from system.";
			}
			sendMessageToHw(msg, HoldingWindowComponent::MESSAGE_TYPE_TO_DAW);
#endif
		} else
		{
			isMotasMidiOutConnected = false;
			if (midiOutputsMotas[index].get() != nullptr && midiOutputsMotas[index]->outDevice != nullptr &&  midiOutputsMotas[index]->outDevice.get() != nullptr)
			{
				msg = "Closing device.";
				sendMessageToHw(msg, HoldingWindowComponent::MESSAGE_TYPE_TO_MOTAS);
				midiOutputsMotas[index]->outDevice.reset();
			} else
			{
				msg = "Device removed from system.";
			}
		}
	}
}

int MIDIConnection::getNumMidiInputs(bool isMotas) const noexcept
{
	if (!isMotas)
		return midiInputs.size();
	else
		return midiInputsMotas.size();

}

int MIDIConnection::getNumMidiOutputs(bool isMotas) const noexcept
{
	if (!isMotas)
		return midiOutputs.size();
	else
		return midiOutputsMotas.size();
}

void MIDIConnection::sendMessageToHw(const String& msg, int type)
{

	if (msg.length() < MSG_BUFFER_SIZE && type >=0 && type < 4)
	{

#if TEST_MALLOC_CRASH == 1
		latestMessages.set(type,  msg);
#else
		strcpy(msgBuffer[type], msg.getCharPointer());
#endif
		latestMessageRead[type] = false;
	}






	//setInfoMessage(msg, type);
	/*
	HoldingWindowComponent* hw = getHoldingWindow();
	if (hw != nullptr)
	{
		hw->newMessage(msg, type);
	}*/
/*
	latestMessage = msg;
	if (!messageRead)
	{
		message = latestMessage;
		messageRead = true;
	}
*/


}

ReferenceCountedObjectPtr<MidiDeviceListEntry> MIDIConnection::findDeviceWithName (const String& name, bool isInputDevice, bool isLookingForMotas) const
{
	if (isLookingForMotas)
	{
		const ReferenceCountedArray<MidiDeviceListEntry>& midiDevices = isInputDevice ? midiInputsMotas
				: midiOutputsMotas;
		for (auto midiDevice : midiDevices)
			if (midiDevice->name == name)
				return midiDevice;
		return nullptr;
	}
	else
	{
#if BUILD_STANDALONE_EXECUTABLE == 1
		const ReferenceCountedArray<MidiDeviceListEntry>& midiDevices = isInputDevice ? midiInputs
				: midiOutputs;
		for (auto midiDevice : midiDevices)
			if (midiDevice->name == name)
				return midiDevice;

#endif
		return nullptr;
	}
}


bool MIDIConnection::hasDeviceListChanged (const StringArray& deviceNames, bool isInputDevice, bool isLookingForMotas)
{
	if (isLookingForMotas)
	{
		ReferenceCountedArray<MidiDeviceListEntry, CriticalSection> &midiDevices = isInputDevice ? midiInputsMotas  : midiOutputsMotas;
		if (deviceNames.size() != midiDevices.size())
		{
			DBG("Is Motas, isInputDevice: "
					+ String((int) isInputDevice) + " Num of devices changed, found system devices:"
					+ String(deviceNames.size()) + " " + String(midiDevices.size()));
			return true;
		}
		for (auto i = 0; i < deviceNames.size(); i++)
		{
			if (deviceNames[i] != midiDevices[i]->name)
				return true;
		}
		return false;
	}
	else
	{
#if BUILD_STANDALONE_EXECUTABLE == 1
		ReferenceCountedArray<MidiDeviceListEntry, CriticalSection> &midiDevices = isInputDevice ? midiInputs   : midiOutputs;
		if (deviceNames.size() != midiDevices.size())
		{
			DBG("NOT Motas, isInputDevice: "
					+ String((int) isInputDevice) + " Num of devices changed, found system devices:"
					+ String(deviceNames.size()) + " " + String(midiDevices.size()));
			return true;
		}
		for (auto i = 0; i < deviceNames.size(); i++)
		{
			if (deviceNames[i] != midiDevices[i]->name)
				return true;
		}
#endif
		return false;
	}

}


void MIDIConnection::autoConnectMIDI()
{
	//DBG("autoConnectMIDI check devices...");
#if BUILD_STANDALONE_EXECUTABLE == 1
	updateDeviceList (true, false);
	updateDeviceList (false, false);
#endif

	updateDeviceList (true, true);
	updateDeviceList (false, true);

	if (autoConnectMotas)
	{
		 if (!connectedMotasInput)
		 {
			 for (int i =  0; i < midiInputsMotas.size(); i++)
			 {
				 DBG(midiInputsMotas[i]->name);
				 if (midiInputsMotas[i]->name.contains("Motas-6"))
				 {
					 DBG("Auto connecting MIDI in to Motas-6");
					 SparseSet<int> selectedRows;
					 Range<int> r(i,i+1);
					 selectedRows.addRange(r);
					 midiInputSelectorMotas->setSelectedRows(selectedRows, NotificationType::sendNotification);
					 connectedMotasInput = true;
					 break; // connect to the first one only
				 }
			 }
		 }
		 if (!connectedMotasOutput)
		 {
			 for (int i =  0; i < midiOutputsMotas.size(); i++)
			 {
				 if (midiOutputsMotas[i]->name.contains("Motas-6"))
				 {
					 DBG("Auto connecting MIDI out to Motas-6");
					 SparseSet<int> selectedRows;
					 Range<int> r(i,i+1);
					 selectedRows.addRange(r);
					 midiOutputSelectorMotas->setSelectedRows(selectedRows, NotificationType::sendNotification);
					 connectedMotasOutput  = true;
					 break; // connect to the first one only
				 }
			 }
		 }
		 //autoConnected = true;
	 }

}



void MIDIConnection::updateDeviceList (bool isInput, bool isLookingForMotas)
{
	//DBG("updateDeviceList");
	HoldingWindowComponent* hw = getHoldingWindow();
	StringArray deviceNames;

	if (isInput)
	{
		deviceNames = MidiInput::getDevices();
	} else
	{
		deviceNames = MidiOutput::getDevices();
	}
	StringArray newDeviceNames;
	if (hideNonMotasDevices)
	{
		if (isLookingForMotas)
		{
			// if there are inputs/outputs called Motas-6, then remove all others from the list
			for (int i = 0; i < deviceNames.size(); i++)
			{
				String name = deviceNames[i];
				if (name.containsWholeWord("Motas"))
					newDeviceNames.add(name);
			}
		} else
		{
#if BUILD_STANDALONE_EXECUTABLE == 1
			// show devices that are NOT Motas
			for (int i = 0; i < deviceNames.size(); i++)
			{
				String name = deviceNames[i];
				if (!name.containsWholeWord("Motas"))
					newDeviceNames.add(name);
			}
#endif
		}
	} else
	{
		// include all devices in the list
		newDeviceNames = deviceNames;
	}


	//DBG("Size midiInputs: " + String(midiInputs.size()));
	if (hasDeviceListChanged(newDeviceNames, isInput, isLookingForMotas))
	{
		if (isLookingForMotas)
		{
			DBG("(Looking for Motas) Devices available to the system have changed");
			ReferenceCountedArray<MidiDeviceListEntry, CriticalSection>& midiDevices = isInput ? midiInputsMotas  : midiOutputsMotas;
			closeUnpluggedDevices (newDeviceNames, isInput, isLookingForMotas);

			ReferenceCountedArray<MidiDeviceListEntry> newDeviceList;

			// add all currently plugged-in devices to the device list
			for (auto newDeviceName : newDeviceNames)
			{
				MidiDeviceListEntry::Ptr entry = findDeviceWithName (newDeviceName, isInput, isLookingForMotas);

				if (entry == nullptr)
				{
					entry = new MidiDeviceListEntry (newDeviceName);
					DBG( "new device: " + newDeviceName);
				}

				newDeviceList.add (entry);
			}

			// actually update the device list
			midiDevices = newDeviceList;


			if (auto* midiSelector = isInput ? midiInputSelectorMotas.get() : midiOutputSelectorMotas.get())
			{
				midiSelector->syncSelectedItemsWithDeviceList (midiDevices);
			}

			DBG("New size inputs: " + String(midiInputs.size()));
		}
#if BUILD_STANDALONE_EXECUTABLE == 1
		else
		{
			DBG("(Looking for non-motas) devices available to the system have changed");
			ReferenceCountedArray<MidiDeviceListEntry, CriticalSection>& midiDevices = isInput ? midiInputs : midiOutputs;

			closeUnpluggedDevices (newDeviceNames, isInput, isLookingForMotas);

			ReferenceCountedArray<MidiDeviceListEntry> newDeviceList;

			// add all currently plugged-in devices to the device list
			for (auto newDeviceName : newDeviceNames)
			{
				MidiDeviceListEntry::Ptr entry = findDeviceWithName (newDeviceName, isInput, isLookingForMotas);

				if (entry == nullptr)
				{
					entry = new MidiDeviceListEntry (newDeviceName);
					DBG( "new device: " + newDeviceName);
				}

				newDeviceList.add (entry);
			}

			// actually update the device list
			midiDevices = newDeviceList;

			// if (hw != nullptr && hw->midiComms != nullptr)
			{
				// update the selection status of the combo-box
				if (auto* midiSelector = isInput ? midiInputSelector.get() : midiOutputSelector.get())
				{
					midiSelector->syncSelectedItemsWithDeviceList (midiDevices);
				}
			}

			// DBG("New size inputs: " + String(midiInputs.size()));
		}
#endif

		// repaint();
		if (hw != nullptr && hw->midiComms != nullptr)
		{
			hw->midiComms->repaint();
		}

	}
}

void MIDIConnection::closeUnpluggedDevices (StringArray& currentlyPluggedInDevices, bool isInputDevice, bool isMotas)
{
   if (!isMotas)
   {
#if BUILD_STANDALONE_EXECUTABLE == 1
	   ReferenceCountedArray<MidiDeviceListEntry, CriticalSection>& midiDevices = isInputDevice ? midiInputs : midiOutputs;
	   DBG("closeUnpluggedDevices");
	   for (auto i = midiDevices.size(); --i >= 0;)
	   {
		   auto& d = *midiDevices[i];
		   DBG("Device names: " + d.name);
		   if (! currentlyPluggedInDevices.contains (d.name))
		   {
			   DBG("closeUnpluggedDevices name not in list");
			   if (isInputDevice ? d.inDevice .get() != nullptr
								 : d.outDevice.get() != nullptr)
				   closeDevice (isInputDevice, i, isMotas);
			   midiDevices.remove (i);
		   }
	   }
#endif
   } else
   {
	   ReferenceCountedArray<MidiDeviceListEntry, CriticalSection>& midiDevices = isInputDevice ? midiInputsMotas : midiOutputsMotas;
	   DBG("closeUnpluggedDevices Motas");
	   for (auto i = midiDevices.size(); --i >= 0;)
	   {
		   auto& d = *midiDevices[i];

		   if (! currentlyPluggedInDevices.contains (d.name))
		   {
			   DBG("closeUnpluggedDevices name not in list");
			   if (isInputDevice ? d.inDevice .get() != nullptr
								 : d.outDevice.get() != nullptr)
				   closeDevice (isInputDevice, i, isMotas);
			   midiDevices.remove (i);
		   }
	   }
   }
}




