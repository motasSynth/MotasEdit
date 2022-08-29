
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

#ifndef SOURCE_PATCH_SETUP_H_
#define SOURCE_PATCH_SETUP_H_
#pragma once

#include "ApplicationCommon.h"
#include "MIDIDeviceListBox.h"
#include "ParameterData.h"
#include "stdint.h"
#include "utilities.h"
class HoldingWindowComponent;
class PatchPatternSequence;
class Firmware;
class MotasEditPluginAudioProcessor;




class PatchSetup  : public Component,  public ComboBox::Listener,
	public TextButton::Listener, public ChangeListener

{
public:

	PatchSetup(HoldingWindowComponent* parent, LookAndFeel* laf);

	~PatchSetup();

	Colour LineColour;
	HoldingWindowComponent* holdingWindow;


	void initPanelSettings();

	void updateCCMapping(int ccIndex, int page , int dest);

private:



	float fieldHeight;
	Rectangle<int>localBoundsRect;

	float boundsHeight;
	int width;
	int widthMatrix;


	void changeListenerCallback (ChangeBroadcaster* source) override;

	void buttonClicked(Button* b) override;



	Label ccMappingLabel { "Parameter mapping", "Parameter mapping" };

	void comboBoxChanged (ComboBox *comboBoxThatHasChanged) override;


	void updateComboMapping(int index,bool updatePatch, int page, int dest);


	TextButton sendAllPatchSettingsButton;
	Array<int> lastIndexChosenArray;
	Array<ComboBox*> mappingCCArray;
	Array<ComboBox*> mappingParameterPageArray;
	Array<ComboBox*> mappingParameterDestinationArray;

	//ComboBox mappingParameterPage;

	//ComboBox mappingParameterDestination;


	void addLabelAndSetStyle (Label& label);

	void resized() override;
#if BUILD_STANDALONE_EXECUTABLE == 1

#else
	ComboBox guiSizeCombo;
#endif

    void paint (Graphics&) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PatchSetup)

};




#endif /* SOURCE_MIDICOMMS_H_ */
