
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
 * MIDIDeviceListBox.cpp
 *
 *
 */

#include "./Headers/MIDIDeviceListBox.h"
//#include "./Headers/MIDIComms.h"
#include "../JuceLibraryCode/JuceHeader.h"
#include "./Headers/utilities.h"
#include "Headers/MIDIConnection.h"


MIDIDeviceListBox::MIDIDeviceListBox (const String& name, MIDIConnection& contentComponent, bool isInputDeviceList, bool isMotasDeviceList)
            : ListBox (name, this),
              parent (contentComponent),
              isInput (isInputDeviceList), isMotas(isMotasDeviceList)
{
	setOutlineThickness (1);
	setMultipleSelectionEnabled (true);
	setClickingTogglesRowSelection (true);
}


//==============================================================================
int MIDIDeviceListBox::getNumRows()
{
	return isInput ? parent.getNumMidiInputs(isMotas)
				   : parent.getNumMidiOutputs(isMotas);
}

//==============================================================================
void MIDIDeviceListBox::paintListBoxItem (int rowNumber, Graphics &g,
					   int width, int height, bool rowIsSelected)
{
	auto textColour = getLookAndFeel().findColour (ListBox::textColourId);

	if (rowIsSelected)
	{
	//	g.fillAll (textColour.interpolatedWith (getLookAndFeel().findColour (ListBox::backgroundColourId), 0.5));
		g.fillAll(Utilities::buttonYellow);
		g.setColour(Colours::black);
	} else
	{
		g.setColour (textColour);
	}


	g.setFont (height * 0.7f);

	if (isInput)
	{
		if (rowNumber < parent.getNumMidiInputs(isMotas))
			g.drawText (parent.getMidiDevice (rowNumber, true, isMotas)->name,
						5, 0, width, height,
						Justification::centredLeft, true);
	}
	else
	{
		if (rowNumber < parent.getNumMidiOutputs(isMotas))
			g.drawText (parent.getMidiDevice (rowNumber, false, isMotas)->name,
						5, 0, width, height,
						Justification::centredLeft, true);
	}
}

//==============================================================================
void MIDIDeviceListBox::selectedRowsChanged (int)
{
	auto newSelectedItems = getSelectedRows();

	DBG("Num selected rows: "  + String(newSelectedItems.size()));
	if (newSelectedItems != lastSelectedItems)
	{
		for (auto i = 0; i < lastSelectedItems.size(); i++)
		{
			if (! newSelectedItems.contains (lastSelectedItems[i]))
				parent.closeDevice (isInput, isMotas, lastSelectedItems[i]);
		}

		for (auto i = 0; i < newSelectedItems.size(); i++)
		{
			if (! lastSelectedItems.contains (newSelectedItems[i]))
				parent.openDevice (isInput,  isMotas, newSelectedItems[i]);
		}

		lastSelectedItems = newSelectedItems;
	}
}

//==============================================================================
void MIDIDeviceListBox::syncSelectedItemsWithDeviceList (const ReferenceCountedArray<MidiDeviceListEntry>& midiDevices)
{
	// Update the list entries in the case that new MIDI devices appear on the system.

	SparseSet<int> selectedRows;
	for (auto i = 0; i < midiDevices.size(); i++)
	{
		if (midiDevices[i]->inDevice.get() != nullptr || midiDevices[i]->outDevice.get() != nullptr)
			selectedRows.addRange (Range<int> (i, i + 1));
	}
	lastSelectedItems = selectedRows;
	updateContent(); // redraw?
	setSelectedRows (selectedRows, NotificationType::dontSendNotification);
}



