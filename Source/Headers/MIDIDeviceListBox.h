
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
 * MIDIDeviceListBox.h
 *
 */

#ifndef SOURCE_MIDIDEVICELISTBOX_H_
#define SOURCE_MIDIDEVICELISTBOX_H_
#pragma once

#include "ApplicationCommon.h"
class MIDIConnection;
struct MidiDeviceListEntry;


class MIDIDeviceListBox : public ListBox,   private ListBoxModel
{
public:

	//==============================================================================
	MIDIDeviceListBox (const String& name,	MIDIConnection& contentComponent,  bool isInputDeviceList, bool isMotasDeviceList);

	//==============================================================================
	int getNumRows() override;

	//==============================================================================
	void paintListBoxItem (int rowNumber, Graphics &g,
						   int width, int height, bool rowIsSelected) override;

	//==============================================================================
	void selectedRowsChanged (int) override;

	//==============================================================================
	//void syncSelectedItemsWithDeviceList (const ReferenceCountedArray<MidiDeviceListEntry>& midiDevices);

	void syncSelectedItemsWithDeviceList (const ReferenceCountedArray<MidiDeviceListEntry>& midiDevices);


private:
	//==============================================================================
	MIDIConnection& parent;
	bool isInput;

	bool isMotas;
	SparseSet<int> lastSelectedItems;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MIDIDeviceListBox)



};

#endif /* SOURCE_MIDIDEVICELISTBOX_H_ */
