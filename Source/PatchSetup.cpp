
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
 */
#include "Headers/PatchSetup.h"

#include "./Headers/MIDIComms.h"
#include "./Headers/MIDIProcessor.h"
#include "../JuceLibraryCode/JuceHeader.h"

#include "./Headers/utilities.h"
#include "./Headers/crc.h"
#include "./Headers/MainComponent.h"
#include "./Headers/PatchPatternSequence.h"
#include "./Headers/HoldingWindowComponent.h"
#include "./Headers/Firmware.h"
#include "../binaryResources/MotasEditResources.h"

#include "Headers/pageParams.h"








PatchSetup::PatchSetup(HoldingWindowComponent* parent, LookAndFeel* l) //: midiKeyboard (keyboardState, MidiKeyboardComponent::horizontalKeyboard)
: holdingWindow(parent)
{

	sendAllPatchSettingsButton.setButtonText("Send");
	addAndMakeVisible(sendAllPatchSettingsButton);
	sendAllPatchSettingsButton.addListener(this);
    sendAllPatchSettingsButton.setTooltip("Send patch settings to Motas");
	sendAllPatchSettingsButton.setLookAndFeel(l);



	for (int i = 0; i < NUMBER_OF_CC_MAPPINGS; i++)
	{


		ComboBox* ccIndex = new ComboBox();

		ccIndex->addItem(String("CC #") + String(i + 1), 1);
		ccIndex->setSelectedItemIndex(0, NotificationType::dontSendNotification);



		ComboBox* c = new ComboBox();

		lastIndexChosenArray.add(0);

		for (int j = 0; j < pageSettings.size(); j++)
		{
			c->addItem(pageSettings[j].toUpperCase(), 1 + j);
		}





		ComboBox* d = new ComboBox();
		for (int j = 0; j < globalDestination.size(); j++)
		{
			d->addItem(globalDestination[j].toUpperCase(), 1 + j);
		}

		c->setSelectedItemIndex(0, NotificationType::dontSendNotification);
		d->setSelectedItemIndex(0, NotificationType::dontSendNotification);



		String num6Text = " (N.B. CC#6 is used for NRPNs so functionality may be restricted to internal control only)";
		if ( i == 5)
		{
			ccIndex->setTooltip("Parameter/CC mapping" + num6Text);
			c->setTooltip("Choose page for mapping #" + String(i + 1) + num6Text);
			d->setTooltip("Choose destination for mapping #"+ String(i + 1) + num6Text);
		}
		else
		{
			ccIndex->setTooltip("Parameter/CC mapping");
			c->setTooltip("Choose page for mapping #" + String(i + 1));
			d->setTooltip("Choose destination for mapping #"+ String(i + 1));

		}


		c->addListener(this);
		d->addListener(this);


		addAndMakeVisible(ccIndex);
		addAndMakeVisible(c);
		addAndMakeVisible(d);

		mappingCCArray.add(ccIndex);
		mappingParameterPageArray.add(c);

		mappingParameterDestinationArray.add(d);

	}

	addAndMakeVisible(ccMappingLabel);









	initPanelSettings(); // DO THIS LAST after combo boxes set up etc




}

PatchSetup::~PatchSetup()
 {
	//setLookAndFeel (nullptr);

	DBG("MIDIComms destructor");


	for (int i = 0; i < this->mappingCCArray.size(); i++)
	{
		if (mappingCCArray[i] != NULL)
		{
			delete mappingCCArray[i];
		}
	}
	for (int i = 0; i < this->mappingParameterDestinationArray.size(); i++)
	{
		if (mappingParameterDestinationArray[i] != NULL)
		{
			delete mappingParameterDestinationArray[i];
		}
	}
	for (int i = 0; i < this->mappingParameterPageArray.size(); i++)
	{
		if (mappingParameterPageArray[i] != NULL)
		{
			delete mappingParameterPageArray[i];
		}
	}

 }


void PatchSetup::changeListenerCallback(ChangeBroadcaster* )
{

}


void PatchSetup::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
	int index =  mappingParameterPageArray.indexOf(comboBoxThatHasChanged);
	if (index >= 0) // if the page combo changed
	{
		updateComboMapping((uint8_t) index, true,  comboBoxThatHasChanged->getSelectedItemIndex(), 0);
	}
	else
	{
		index =  mappingParameterDestinationArray.indexOf(comboBoxThatHasChanged);
		if (index >= 0) // destination changed
		{
			DBG("Update CC map. dest: "+ String(comboBoxThatHasChanged->getSelectedItemIndex()));

			holdingWindow->mainComponent->setCCParam(0, (uint8_t) index, (uint8_t) comboBoxThatHasChanged->getSelectedItemIndex());
		}
	}
}




//==============================================================================
void PatchSetup::addLabelAndSetStyle (Label& label)
{
    label.setFont (Font (15.00f, Font::plain));
    label.setJustificationType (Justification::centredLeft);
    label.setEditable (false, false, false);
    label.setColour (TextEditor::textColourId, Colours::black);
    label.setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (label);
}


void PatchSetup::resized()
{
	localBoundsRect = getLocalBounds().reduced(10);


	int w = localBoundsRect.getWidth();
    boundsHeight = (float) localBoundsRect.getHeight();

    width = w/2;
    fieldHeight = boundsHeight / (NUMBER_OF_CC_MAPPINGS  + 1);

    if (fieldHeight > 40)
    {
    	fieldHeight = 30;
    }
	auto r2 = localBoundsRect.removeFromTop((int) fieldHeight);
	//
#define PARAM_CONTROL_WIDTH 830.0f
  //  float buttonHeight =  boundsHeight / 24.0f;
    int buttonWidth = (int) (w *0.138f);


	 widthMatrix = (int) ( width * 2/3);

	 ccMappingLabel.setBounds (r2.removeFromLeft (widthMatrix /2));
	 sendAllPatchSettingsButton.setBounds (r2.removeFromLeft ((int)buttonWidth/2));

	for (int i = 0; i < NUMBER_OF_CC_MAPPINGS; i++)
	{
		r2 = localBoundsRect.removeFromTop((int)(fieldHeight));

		mappingCCArray[i]->setBounds (r2.removeFromLeft (widthMatrix /3));


		mappingParameterPageArray[i]->setBounds (r2.removeFromLeft (widthMatrix /2));
		//r2 = localBoundsRect.removeFromTop((int)(fieldHeight));
		mappingParameterDestinationArray[i]->setBounds(r2.removeFromLeft (widthMatrix/2));

	}






}






void PatchSetup::buttonClicked(Button* b)
{


	if (b == &sendAllPatchSettingsButton)
	{
		uint8_t index = (uint8_t) holdingWindow->mainComponent->patchesCombo.getSelectedItemIndex();
		holdingWindow->midiProcessor->commandRequest(MIDIProcessor::COMMAND_REQUEST_PATCH_CHANGE, index);
	}

}





void PatchSetup::initPanelSettings()
{


#if BUILD_STANDALONE_EXECUTABLE == 0
	guiSizeCombo.setSelectedId(holdingWindow->appCommon->
			loadAppPropertyInteger(ApplicationCommon::APP_PROPERTY_PLUGIN_GUI_SIZE),
			NotificationType::dontSendNotification);
#endif








}

void PatchSetup::updateCCMapping(int ccIndex, int page , int dest)
{
	updateComboMapping(ccIndex, false, page, dest);
	mappingParameterPageArray[ccIndex]->setSelectedItemIndex(page, NotificationType::dontSendNotification);
	mappingParameterDestinationArray[ccIndex]->setSelectedItemIndex(dest, NotificationType::dontSendNotification);
}



void PatchSetup::paint(Graphics& g)
{

	g.setColour(LineColour);

#if BUILD_STANDALONE_EXECUTABLE == 1

	float lineThickness = 3.0f;
	float arrowHeadLength = 12.0f;
	float arrowHeadWidth = 12.0f;

	float xOffset = widthMatrix * 1.03f;
	float yOffset = -boundsHeight * 0.05f + fieldHeight * 2;
	float matrixWidth = widthMatrix*0.97f;
	float circleWidth = 8.0f;
	// horizontal arrow botttom RHS
	Line<float> l = Line<float>(xOffset + 15 *matrixWidth/24, yOffset +  3 * boundsHeight / 8, xOffset + matrixWidth, yOffset +  3 *boundsHeight / 8);
	g.drawArrow(l, lineThickness, arrowHeadWidth, arrowHeadLength);



	// small horiz line RHS
	l = Line<float>(xOffset + 15 *matrixWidth/24, yOffset +   boundsHeight / 8,xOffset  + 7 *matrixWidth/9, yOffset +  boundsHeight / 8);
	g.drawLine(l,lineThickness);


	// small right arrow RHS of MEdit
	l = Line<float>(xOffset  + 15 *matrixWidth/24, yOffset +  7*boundsHeight / 32,xOffset  + 7 *matrixWidth/9, yOffset +  7*boundsHeight / 32);
	g.drawLine(l,lineThickness);
	g.fillEllipse(xOffset  + 7 *matrixWidth/9- circleWidth /2, yOffset +  7*boundsHeight / 32- circleWidth /2, circleWidth,circleWidth);


	// big down line RHS
	l = Line<float>(xOffset  + 7 *matrixWidth/9, yOffset +  boundsHeight / 8,xOffset  + 7 *matrixWidth/9, yOffset +  3 *boundsHeight / 8);
	g.drawLine(l,lineThickness);
	g.fillEllipse(xOffset  + 7 *matrixWidth/9 - circleWidth /2, yOffset +  3 *boundsHeight / 8- circleWidth /2, circleWidth,circleWidth);



	// LHS arrow upper
	l = Line<float>(xOffset, yOffset +   boundsHeight / 8, xOffset + matrixWidth/3, yOffset +  boundsHeight / 8);
	g.drawArrow(l, lineThickness, arrowHeadWidth, arrowHeadLength);

	// LHS arrow lower
	l = Line<float>(xOffset, yOffset +   3 *boundsHeight / 8, xOffset + matrixWidth/3, yOffset +  3* boundsHeight / 8);
	g.drawArrow(l, lineThickness, arrowHeadWidth, arrowHeadLength);





	// LHS small arrow upper lower
	l = Line<float>(xOffset + matrixWidth/8, yOffset +   7*boundsHeight / 32, xOffset + matrixWidth/3, yOffset +  7*boundsHeight / 32);
	g.drawArrow(l, lineThickness, arrowHeadWidth, arrowHeadLength);

	// LHS small arrow lower upper
	l = Line<float>(xOffset + matrixWidth/8, yOffset +   9 *boundsHeight / 32, xOffset + matrixWidth/3, yOffset +  9* boundsHeight / 32);
	g.drawArrow(l, lineThickness, arrowHeadWidth, arrowHeadLength);


	// LHS hline lower lower
	l = Line<float>(xOffset + matrixWidth/8, yOffset +   21 *boundsHeight / 64, xOffset + 8*matrixWidth/12, yOffset +  21* boundsHeight / 64);
	g.drawLine(l, lineThickness);
	g.fillEllipse(xOffset + matrixWidth/8 - circleWidth /2, yOffset +   21 *boundsHeight / 64- circleWidth /2, circleWidth,circleWidth);

	// vertical line onto Motasedit output joiner
	l = Line<float>(xOffset + 8*matrixWidth/12, yOffset +  21* boundsHeight / 64, xOffset + 8*matrixWidth/12, yOffset +  9* boundsHeight / 32);
	g.drawLine(l, lineThickness);
	g.fillEllipse(xOffset + 8*matrixWidth/12- circleWidth /2, yOffset +  9* boundsHeight / 32- circleWidth /2, circleWidth,circleWidth);




	// vertical line
	l = Line<float>(xOffset + matrixWidth/8, yOffset +   3*boundsHeight / 8, xOffset + matrixWidth/8, yOffset +  9*boundsHeight / 32);
	g.drawLine(l,lineThickness);
	l = Line<float>(xOffset + matrixWidth/8, yOffset +   boundsHeight / 8, xOffset + matrixWidth/8, yOffset +  7* boundsHeight / 32);
	g.drawLine(l,lineThickness);
	g.fillEllipse(xOffset + matrixWidth/8 -circleWidth /2,yOffset +  3 * boundsHeight / 8-circleWidth /2, circleWidth,circleWidth);
	g.fillEllipse(xOffset + matrixWidth/8 -circleWidth /2,yOffset +  boundsHeight / 8-circleWidth /2, circleWidth,circleWidth);


	// RHS small line lower upper
	l = Line<float>(xOffset  + 15 *matrixWidth/24, yOffset +   9 *boundsHeight / 32, xOffset  + 8 *matrixWidth/9, yOffset +  9* boundsHeight / 32);
	g.drawLine(l,lineThickness);
	// RHS line going up
	l = Line<float>(xOffset  + 8 *matrixWidth/9, yOffset +  9* boundsHeight / 32, xOffset  + 8 *matrixWidth/9, yOffset +  boundsHeight / 8);
	g.drawLine(l,lineThickness);




	// RHS arrow onto Motas out
	l = Line<float>(xOffset  + 8 *matrixWidth/9, yOffset +  boundsHeight / 8, xOffset  + matrixWidth, yOffset +  boundsHeight / 8);
	g.drawArrow(l, lineThickness, arrowHeadWidth, arrowHeadLength);

#endif

}


/*
 *
 * Populate the destination combo box according to the page choice (which will go in the page combobox)
 *
 */
void PatchSetup::updateComboMapping(int index, bool updatePatchData, int page, int )
{
	// index is the CC controller index (0.. 23)
	if (index >= 0 && index < NUMBER_OF_CC_MAPPINGS)
	{
		// get the combo boxes for this CC index
		ComboBox* c = mappingParameterPageArray[index];
		ComboBox* d = mappingParameterDestinationArray[index];

		// get what we previously had selected for the page
		int prevIndex = lastIndexChosenArray[index];

		int newSelectionType;
		int oldSelectionType;

		if (page < 1)
			newSelectionType = -1; // OFF
		else if (page < 2)
			newSelectionType = 0; // global
		else if  (page == (pageSettings.size() - 1))
			newSelectionType = 2; // param page
		else
			newSelectionType = 1; // patch settings

		if (prevIndex < 1)
			oldSelectionType = -1;
		else if (prevIndex < 2)
			oldSelectionType = 0;
		else if  (prevIndex == (pageSettings.size() - 1))
			oldSelectionType = 2;
		else
			oldSelectionType = 1;

		if (!updatePatchData || oldSelectionType != newSelectionType)
		{
			// either the incoming patch data has called this function, to update the displayed choices or
			// the page setting type has changed, so need to update the parameter destination choices
			d->clear(NotificationType::dontSendNotification);
			if (newSelectionType < 0 )
				d->addItem("NONE", 1);
			else if (newSelectionType == 0)
			{
				for (int i = 0; i < globalDestination.size(); i++)
				{
					d->addItem(globalDestination[i].toUpperCase(), 1 + i);
				}
			}
			else if (newSelectionType == 1)
			{
				for (int i = 0; i < pageDestinations.size(); i++)
				{
					d->addItem(pageDestinations[i].toUpperCase(), 1 + i);
				}
			}
			else
			{
				for (int i = 0; i < patchSetupDestination.size(); i++)
				{
					d->addItem(patchSetupDestination[i].toUpperCase(), 1 + i);
				}
			}

			if (updatePatchData)
			{
				holdingWindow->mainComponent->setCCParam(0, (uint8_t) index, 0);
				// reset to first choice in the param destination
				d->setSelectedItemIndex(0, NotificationType::dontSendNotification);
			}
		}

		if (!updatePatchData)
		{
			lastIndexChosenArray.set(index, page); // store this new setting
		}
		else
		{
			DBG("Update CC map. Page: " + String(c->getSelectedItemIndex()) + " dest: "+ String(d->getSelectedItemIndex()));

			lastIndexChosenArray.set(index, c->getSelectedItemIndex()); // store this new setting
			holdingWindow->mainComponent->setCCPage(0, (uint8_t) index,(uint8_t) c->getSelectedItemIndex());
			holdingWindow->mainComponent->setCCParam(0, (uint8_t) index, (uint8_t)d->getSelectedItemIndex());
		}
	}



}
