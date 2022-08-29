

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

    CustomListBox.cpp

  ==============================================================================
*/


#include "./Headers/CustomListBox.h"
#include "./Headers/utilities.h"
#include "./Headers/PatchPatternSequence.h"

struct ListEntry;

//==============================================================================

CustomListBox::CustomListBox (const String& name, PatchPatternSequence& contentComponent, int type)
            : ListBox (name, this),
              parent (contentComponent), type (type), holdingType(0)
{
	setOutlineThickness (1);
	setMultipleSelectionEnabled (false);
	setClickingTogglesRowSelection (true);

	x = 0;
	y = 0;
}

//==============================================================================
int CustomListBox::getNumRows()
{
	if (type == 0)
		return parent.listBoxItems.size();
	else
		return parent.listBoxPresetsItems.size();
}

bool CustomListBox::isInterestedInDragSource(const SourceDetails& s)
{

	if (holdingType == 0  && s.description.isInt())
		return true;
	else
		return false;
}

void CustomListBox::itemDropped(const SourceDetails& dragSourceDetails)
{


	int row = getInsertionIndexForPosition(dragSourceDetails.localPosition.x, dragSourceDetails.localPosition.y) - 1;

	int source = int(dragSourceDetails.description);

	if (row >= 0 && row <= 50)
	{
		DBG("last rows dropped: " + dragSourceDetails.description.toString() + " onto row: " + String(row));
		if (type == 0)
			parent.copyPatchToBank(source, row);
		else
			parent.copyPatchToPresets(source, row);
	}

	repaint();

}

void CustomListBox::mouseUp(const MouseEvent& event)
{


	x = event.getScreenX();
	y = event.getScreenY();
	DBG("X: " + String(x) + " Y:" + String(y));


}

var CustomListBox::getDragSourceDescription (const SparseSet<int>& selectedRows)
{

	if (holdingType == 0) // holds patches
	{
		DBG("getDragSourceDescription mouse drag");

        // for our drag desctription, we'll just make a list of the selected
        // row numbers - this will be picked up by the drag target and displayed in
        // its box.
        String desc;

        for (int i = 0; i < selectedRows.size(); ++i)
            desc << (selectedRows [i] + 1) << " ";

        return selectedRows[0];
	} else
	{
		return "invalid";
	}
}


//==============================================================================
void CustomListBox::paintListBoxItem (int rowNumber, Graphics &g,
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

	if (type == 0)
	{
		if (rowNumber < parent.listBoxItems.size())
		g.drawText (parent.listBoxItems[rowNumber]->name,
					5, 0, width, height, Justification::centredLeft, true);
	} else
	{
		if (rowNumber < parent.listBoxPresetsItems.size())
				g.drawText (parent.listBoxPresetsItems[rowNumber]->name,
							5, 0, width, height, Justification::centredLeft, true);
	}
}

//==============================================================================
void CustomListBox::selectedRowsChanged (int i)
{
	DBG("selectedRowsChanged" + String(i));
	parent.changeListBoxRow(i);
}

void CustomListBox::syncSelectedItemsWithDeviceList(
		const ReferenceCountedArray<ListEntry>& )
{
/*
	SparseSet<int> selectedRows;
	for (auto i = 0; i < listBoxEntry.size(); i++)
	{
		//if (listBoxEntry[i]->inDevice.get() != nullptr || listBoxEntry[i]->outDevice.get() != nullptr)
			selectedRows.addRange (Range<int> (i, i + 1));
	}
	lastSelectedItems = selectedRows;
	updateContent();
	setSelectedRows (selectedRows, dontSendNotification);
*/
}

void CustomListBox::mouseDrag(const MouseEvent&)
{
	DBG("mouse drag");



}

void CustomListBox::setHoldingType(int ht)
{
	this->holdingType = ht;
}
