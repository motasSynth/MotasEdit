
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
  ==============================================================================

    CustomListBox.h


  ==============================================================================
*/

#pragma once

#include "ApplicationCommon.h"

struct ListEntry;
class PatchPatternSequence;
//==============================================================================
/*
*/
class CustomListBox    : public ListBox,   private ListBoxModel, public DragAndDropTarget
{
public:


	//==============================================================================
	int getNumRows() override;

	//==============================================================================
	void paintListBoxItem (int rowNumber, Graphics &g,
						   int width, int height, bool rowIsSelected) override;

	//==============================================================================
	void selectedRowsChanged (int) override;

	void syncSelectedItemsWithDeviceList (const ReferenceCountedArray<ListEntry>& listBoxEntry);


    CustomListBox(const String& name, PatchPatternSequence& contentComponent, int type);


    bool isInterestedInDragSource (const SourceDetails& /*dragSourceDetails*/) override;

    void itemDropped (const SourceDetails& dragSourceDetails) override;

    void mouseDrag(const MouseEvent &event) override;

    void mouseUp(const MouseEvent &event) override;

    var getDragSourceDescription (const SparseSet<int>& selectedRows) override;

    void setHoldingType(int holdingType);
private:


    int x;
    int y;

    PatchPatternSequence& parent;
    int type;
    int holdingType;

	SparseSet<int> lastSelectedItems;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomListBox)
};



