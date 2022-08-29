
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

    PatchPatternSequence.cpp


  ==============================================================================
*/


#include "./Headers/PatchPatternSequence.h"
#include "./Headers/utilities.h"
#include "./Headers/crc.h"
#include "./Headers/CustomLook.h"
#include "./Headers/HoldingWindowComponent.h"

#include "Headers/MIDIProcessor.h"
#include "Headers/MainComponent.h"
#include "../binaryResources/MotasEditResources.h"






SIZEOF_STATIC_ASSERT( sizeof(t_modOfMod) == 8 );
SIZEOF_STATIC_ASSERT( sizeof(singleParam) == 108 );
SIZEOF_STATIC_ASSERT( sizeof(tArpegSettings) == 8 );
SIZEOF_STATIC_ASSERT( sizeof(t_customLFOWave) == 68 );


SIZEOF_STATIC_ASSERT( sizeof(tAllParams) == 4096 );


//==============================================================================
PatchPatternSequence::PatchPatternSequence(HoldingWindowComponent* hw, LookAndFeel* laf)
: holdingWindow(hw)
{
	patchSelection = 0;
	patternSelection = 0;
	sequenceSelection = 0;



	this->patchBuffer = nullptr;
	this->sequenceBuffer = nullptr;
	this->patternBuffer = nullptr;


	this->currentModePatchPatternBank = 0;
	this->isModeBank = false;
	//this->midiComms = nullptr;

	this->currentListBoxRow = 0;

	patchesButton = std::make_unique<TextButton>("Patches");
	patternsButton = std::make_unique< TextButton>("Pattern");
	sequencesButton = std::make_unique< TextButton>("Sequences");

	requestButton = std::make_unique< TextButton>("Request");
	sendButton = std::make_unique< TextButton>("Send");


	singleButton.addListener(this);
	bankButton.addListener(this);
	saveButton.addListener(this);
	loadButton.addListener(this);

	clearSlotsButton.addListener(this);
	bankButton.setButtonText("Bank");
	singleButton.setButtonText("Single");
	saveButton.setButtonText("Save");
	loadButton.setButtonText("Load");
	clearSlotsButton.setButtonText("Clear slots");
#if BUILD_STANDALONE_EXECUTABLE == 0
	patchPresetsLabel.setText("Plugin patch presets", NotificationType::dontSendNotification);
#else
	patchPresetsLabel.setText("Patch presets", NotificationType::dontSendNotification);
#endif

	listLabel.setText("Patches to load/save", NotificationType::dontSendNotification);

	copyAllPatchesToPresets.addListener(this);
	copyAllPatchesToPresets.setButtonText("Copy all -->");
	copyAllPatchesFromPresets.addListener(this);
	copyAllPatchesFromPresets.setButtonText("<-- Copy all");
	copyAllPatchesFromPresets.setLookAndFeel(laf);
	copyAllPatchesToPresets.setLookAndFeel(laf);
	patchPresetsLabel.setLookAndFeel(laf);
	listLabel.setLookAndFeel(laf);


	patchesButton->addListener(this);
	patternsButton->addListener(this);
	sequencesButton->addListener(this);
	requestButton->addListener(this);
	sendButton->addListener(this);



	patchesButton->setLookAndFeel(laf);
	patternsButton->setLookAndFeel(laf);
	sequencesButton->setLookAndFeel(laf);
	requestButton->setLookAndFeel(laf);
	sendButton->setLookAndFeel(laf);
	singleButton.setLookAndFeel(laf);
	bankButton.setLookAndFeel(laf);
	saveButton.setLookAndFeel(laf);
	loadButton.setLookAndFeel(laf);
	clearSlotsButton.setLookAndFeel(laf);

	addAndMakeVisible(saveButton);
	addAndMakeVisible(loadButton);
	addAndMakeVisible(bankButton);
	addAndMakeVisible(singleButton);
	addAndMakeVisible(clearSlotsButton);
	addAndMakeVisible(copyAllPatchesFromPresets);
	addAndMakeVisible(copyAllPatchesToPresets);
	addAndMakeVisible(patchPresetsLabel);
	addAndMakeVisible(listLabel);

	listLabel.setTooltip("Drag and drop from patch presets list (right) to make a bank for saving to disk or transferring to Motas bank memory");
	patchPresetsLabel.setTooltip("Drag and drop from patches list (left) to populate the presets bank");

	requestButton->setTooltip("Press to request from Motas. For 'single' items the request is always for the current active item");
	sendButton->setTooltip("Press to send to Motas. Choose the destination from the combobox below");
	bankButton.setTooltip("Click to move to bank mode");
	singleButton.setTooltip("Click to set single mode");
	loadButton.setTooltip("Click to load from disk");
	saveButton.setTooltip("Click to save to disk");
	clearSlotsButton.setTooltip("Click to erase list entries");


	copyAllPatchesToPresets.setTooltip("Copy all the patches from the save/load patches list above to the presets list");
	copyAllPatchesFromPresets.setTooltip("Copy all the patches from the presets list above to the save/load patches list");

	patternsButton->setTooltip("Click to switch to pattern mode for saving/loading/requesting/sending pattern data");
	patchesButton->setTooltip("Click to switch to patch mode for saving/loading/requesting/sending patch data");
	sequencesButton->setTooltip("Click to switch to sequence mode for saving/loading/requesting/sending sequence data");


	comboBoxDestination.setTooltip("Choose send destination on Motas");
	addAndMakeVisible(comboBoxDestination);

	factoryCombo.setTooltip("Choose factory presets to load");
	addAndMakeVisible(factoryCombo);

	addAndMakeVisible(*patchesButton);
	patchesButton->setClickingTogglesState(true);
	patchesButton->setRadioGroupId(1, NotificationType::dontSendNotification);
	addAndMakeVisible(*patternsButton);
	patternsButton->setRadioGroupId(1, NotificationType::dontSendNotification);
	patternsButton->setClickingTogglesState(true);

	addAndMakeVisible(*sequencesButton);
	sequencesButton->setRadioGroupId(1, NotificationType::dontSendNotification);
	sequencesButton->setClickingTogglesState(true);


	singleButton.setRadioGroupId(2, NotificationType::dontSendNotification);
	singleButton.setClickingTogglesState(true);

	bankButton.setRadioGroupId(2, NotificationType::dontSendNotification);
	bankButton.setClickingTogglesState(true);



	patchesButton->setToggleState(true, NotificationType::dontSendNotification);
	singleButton.setToggleState(true, NotificationType::dontSendNotification);

	addAndMakeVisible(*requestButton);
	addAndMakeVisible(*sendButton);
	//requestButton->setClickingTogglesState(true);


	nameEdit = std::make_unique<TextEditor>();
	nameEdit->setMultiLine(false);
	nameEdit->setInputRestrictions(32);

	nameEdit->setColour (TextEditor::backgroundColourId, Colours::black);
	nameEdit->setColour (TextEditor::textColourId, Utilities::textYellow);


	nameEdit->addListener(this);
	nameEdit->setLookAndFeel(laf);

		//Font font (getTextButtonFont (button, button.getHeight()));
	nameEdit->setFont(24.0f );
	nameEdit->setTooltip("Enter new name here");

	addAndMakeVisible(*nameEdit);

	tabbedComponent = std::make_unique< TabbedComponent>(TabbedButtonBar::Orientation::TabsAtTop);
	Colour c(0xB0, 0xB0, 0xB0);

	tabbedComponent->addTab("Patches", c, nullptr, false);







	for (int i = 0; i < BANK_OF_PATCHES_LENGTH; i++)
	{

		bankOfPatches[i] = new uint8[PATCH_LENGTH_BYTES_UNPACKED];
		memset(bankOfPatches[i], 0, PATCH_LENGTH_BYTES_UNPACKED);
	}
	for (int i = 0; i < BANK_OF_PATTERNS_LENGTH; i++)
	{

		bankOfPatterns.add(new uint8[PATTERN_LENGTH_BYTES_UNPACKED]);
		memset(bankOfPatterns[i], 0, PATTERN_LENGTH_BYTES_UNPACKED);
	}

	for (int i = 0; i < BANK_OF_SEQUENCES_LENGTH; i++)
	{

		bankOfSequences.add(new uint8[SEQUENCE_LENGTH_BYTES_UNPACKED]);
		memset(bankOfSequences[i], 0, SEQUENCE_LENGTH_BYTES_UNPACKED);

	}

	listBox = std::make_unique< CustomListBox>(String("patchPatternSeq"), *this, 0);
	listBoxPresets = std::make_unique< CustomListBox>(String("presets"), *this, 1);


	// add all currently plugged-in devices to the device list

	//PatternListEntry* entry = new PatternListEntry("hi");

	//PatternListEntry::Ptr entry = findDeviceWithName (newDeviceName, isInput);

	addAndMakeVisible (listBox.get());
	addAndMakeVisible(*listBoxPresets);
	updatePresetsDisplay();
	updateCurrentDisplayData();

	comboBoxDestination.setSelectedId(1, NotificationType::dontSendNotification);



	int i = 0;
	factoryCombo.setTextWhenNothingSelected("Choose factory patch bank...");
	//factoryCombo.addItem("Choose a factory preset...", ++i);
	factoryCombo.addItem("Factory bank 1", ++i);
	factoryCombo.addItem("Factory bank 2", ++i);
	factoryCombo.addItem("Factory bank 3", ++i);
	factoryCombo.addItem("Factory bank 4", ++i);
	factoryCombo.addItem("Factory bank 5", ++i);
	//factoryCombo.setSelectedItemIndex(0, NotificationType::dontSendNotification);
	factoryCombo.addListener(this);
	//listBox->syncSelectedItemsWithDeviceList (listBoxItems);

	//this->addAndMakeVisible(listBox);
}


void PatchPatternSequence::updatePresetPatch(int row)
{
	//uint8_t* buf = holdingWindow->appCommon->patchPresetsEditedArray[row];


	String	itemName = holdingWindow->appCommon->getPatchName(row);

	ListEntry* item = listBoxPresetsItems[row];

	if (row < 9)
		item->name = String((String("[0") + String(1 + row) + "] " + itemName));
	else
		item->name = String((String("[") + String(1 + row) + "] " + itemName));

	listBoxPresets->repaint();

}

void PatchPatternSequence::updatePatchUI(int row, bool dataAdjusted, bool final, int type)
{


//	uint32_t pat = PATTERN_IDENTIFIER;

    //qDebug() << "Pattern ident: " << pat;
	uint32_t identifierType = 0;
	uint8_t* buf = nullptr;
    uint32_t itemLength = 0;

    switch (type)
    {
        case MIDI_SYSEX_PATCH_TRANSFER:
        case MIDI_SYSEX_PATCH_BANK_TRANSFER:
            patchBuffer =  bankOfPatches[row];

            buf = patchBuffer;
            identifierType = PATCH_IDENTIFIER;
            itemLength = PATCH_LENGTH_BYTES_UNPACKED;

            updatePresetPatch(row);

            break;
        case MIDI_SYSEX_PATTERN_TRANSFER:
        case MIDI_SYSEX_PATTERN_BANK_TRANSFER:
            patternBuffer = bankOfPatterns[row];
            buf = patternBuffer;
            identifierType = PATTERN_IDENTIFIER;
            itemLength = PATTERN_LENGTH_BYTES_UNPACKED;
            break;
        case MIDI_SYSEX_SEQUENCE_TRANSFER:
        case MIDI_SYSEX_SEQUENCE_BANK_TRANSFER:
            sequenceBuffer = bankOfSequences[row];
            buf = sequenceBuffer;
            identifierType = SEQUENCE_IDENTIFIER;
            itemLength = SEQUENCE_LENGTH_BYTES_UNPACKED;
            break;
        default:
            return;
    }



    uint32_t identifier  = Utilities::read32(0, buf);
    String itemName = " ";

 //   DBG("Itentifiers: " + String(identifier)  + " " + String(identifierType));
    if (identifier == identifierType)
    {
        if (final)
        {
        	//uint32_t CRC32  = Utilities::read32(4, buf);
            // offset of 8 to miss the CRC and identifier bytes
        	uint32_t crcVal = crcCalculator::crcFastSTMF4(buf + 8, itemLength - 8U); // calculate CRC of all data (excluding first 8 bytes, and final 4)

            if (dataAdjusted)
            {
                Utilities::write32(crcVal, 4, buf); // write new CRC to the patchbuffer
               // CRC32 = crcVal;
            }
        }

		// populate the name field
		itemName =  Utilities::getName(buf);
    } else
    {
        switch (type)
        {
            case MIDI_SYSEX_PATCH_TRANSFER:
            case MIDI_SYSEX_PATCH_BANK_TRANSFER:
                memset(buf, 0, PATCH_LENGTH_BYTES_UNPACKED);
                break;
            case MIDI_SYSEX_PATTERN_TRANSFER:
            case MIDI_SYSEX_PATTERN_BANK_TRANSFER:
                memset(buf, 0, PATTERN_LENGTH_BYTES_UNPACKED);
                break;
            case MIDI_SYSEX_SEQUENCE_TRANSFER:
            case MIDI_SYSEX_SEQUENCE_BANK_TRANSFER:
                memset(buf, 0, SEQUENCE_LENGTH_BYTES_UNPACKED);
                break;
        }
    }

    //String itemName = holdingWindow->appCommon->getPatchName(buf);
    ListEntry* item = listBoxItems[row];

	if (final && !dataAdjusted)
	{
		this->nameEdit->setText(itemName);
	}

    if (row < 9)
        item->name = String((String("[0") + String(1 + row) + "] " + itemName));
    else
        item->name = String((String("[") + String(1 + row) + "] " + itemName));

    if (final)
    {
    	//listBox->syncSelectedItemsWithDeviceList (listBoxItems);
    	listBox->repaint();
    }
}


PatchPatternSequence::~PatchPatternSequence()
{
	listBoxItems.clear();
	listBox.reset();
	listBoxPresetsItems.clear();
	listBoxPresets.reset();


	for (int i = 0; i < BANK_OF_PATCHES_LENGTH; i++)
	{
		delete[] bankOfPatches[i];
	}
	//bankOfPatches.clear();

	for (int i = 0; i < bankOfPatterns.size(); i++)
		{
			delete[] bankOfPatterns[i];
		}
	bankOfPatterns.clear();

	for (int i = 0; i < bankOfSequences.size(); i++)
		{
			delete[] bankOfSequences[i];
		}


	bankOfSequences.clear();
}

void PatchPatternSequence::paint (Graphics& )
{

}

void PatchPatternSequence::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..


	int border = 10;
	auto r = getLocalBounds().reduced(border);

	int h = r.getHeight();
	int w = r.getWidth();


	const int buttonWidth = w / 4;
	int buttonHeight = h / 12;
	//const int areaLeft = 0;

	const int listBoxWidth = (w -  buttonWidth * 2) /2;

	int rowHeight = h / 25;

	if (buttonHeight < 20)
		buttonHeight = 20;
	if (rowHeight < 20)
		rowHeight = 20;

    int areaRight = 0;//buttonWidth*2;
    int offsetLeft = 0;//listBoxWidth/2;
	if (this->currentModePatchPatternBank == 0)
	{
		listLabel.setText("Patches to load/save", NotificationType::dontSendNotification);
		patchPresetsLabel.setSize(listBoxWidth, rowHeight);
		patchPresetsLabel.setTopLeftPosition(areaRight + border + offsetLeft +  2*buttonWidth + listBoxWidth, border);
		listBoxPresets->setRowHeight(rowHeight);
		listBoxPresets->setSize(listBoxWidth, h - 2 * rowHeight);
		listBoxPresets->setTopLeftPosition(areaRight + border + offsetLeft +  2*buttonWidth + listBoxWidth, border + rowHeight);
		listBoxPresets->setVisible(true);
		patchPresetsLabel.setVisible(true);
		copyAllPatchesToPresets.setVisible(true);
		copyAllPatchesFromPresets.setVisible(true);
		//transferPatchToEditButton.setTopLeftPosition(border+ offsetLeft, buttonHeight * 7 + border);
		//transferPatchFromEditButton.setTopLeftPosition(border+ offsetLeft+buttonWidth, buttonHeight * 7 + border);

		copyAllPatchesToPresets.setTopLeftPosition(areaRight + border + offsetLeft +  2*buttonWidth,
				h - rowHeight);
		copyAllPatchesFromPresets.setTopLeftPosition(areaRight + border + offsetLeft +  2*buttonWidth + listBoxWidth,
				h - rowHeight);

		listBox->setRowHeight(rowHeight);
		listBox->setSize(listBoxWidth, h - 2*rowHeight);
		listBox->setTopLeftPosition(areaRight + border + offsetLeft +  2*buttonWidth, border + rowHeight);





	} else
	{
		if (currentModePatchPatternBank == 1)
			listLabel.setText("Patterns to load/save", NotificationType::dontSendNotification);
		else
			listLabel.setText("Sequences to load/save", NotificationType::dontSendNotification);
		listBoxPresets->setVisible(false);
		patchPresetsLabel.setVisible(false);
		//areaRight = buttonWidth*2;
		//offsetLeft = 0.0f;
		listBox->setRowHeight(rowHeight);
		listBox->setSize(listBoxWidth, h - rowHeight);
		listBox->setTopLeftPosition(areaRight + border + offsetLeft +  2*buttonWidth, border + rowHeight);

	}

	listLabel.setSize(listBoxWidth, rowHeight);
	listLabel.setTopLeftPosition(areaRight + border + offsetLeft +  2*buttonWidth , border);


	auto thirdWidth = (buttonWidth * 2) / 3;




	patchesButton->setSize(thirdWidth, (int) (buttonHeight*0.8f));
	patternsButton->setSize(thirdWidth, (int) (buttonHeight*0.8f));
	sequencesButton->setSize(thirdWidth, (int) (buttonHeight*0.8));

	requestButton->setSize(buttonWidth, buttonHeight);
	sendButton->setSize(buttonWidth, buttonHeight);
	bankButton.setSize(buttonWidth, (int) (buttonHeight*0.8f));
	singleButton.setSize(buttonWidth, (int) ( buttonHeight*0.8f));
	saveButton.setSize(buttonWidth, buttonHeight);
	loadButton.setSize(buttonWidth, buttonHeight);
	clearSlotsButton.setSize(buttonWidth, buttonHeight);

	copyAllPatchesToPresets.setSize(buttonWidth, buttonHeight);
	copyAllPatchesFromPresets.setSize(buttonWidth, buttonHeight);

	comboBoxDestination.setSize(buttonWidth*2, buttonHeight);


	patchesButton->setTopLeftPosition(border + offsetLeft, border);
	patternsButton->setTopLeftPosition(border + offsetLeft+ thirdWidth, border);
	sequencesButton->setTopLeftPosition(border + offsetLeft+thirdWidth*2, border);

	singleButton.setTopLeftPosition(border+ offsetLeft, buttonHeight + border);
	bankButton.setTopLeftPosition(border+ offsetLeft +buttonWidth, buttonHeight+ border);



	saveButton.setTopLeftPosition(border+ offsetLeft +buttonWidth, buttonHeight * 2 + border);
	loadButton.setTopLeftPosition(border+ offsetLeft, buttonHeight * 2 + border);
	requestButton->setTopLeftPosition(border+ offsetLeft, buttonHeight * 4 + border);
	sendButton->setTopLeftPosition(border+ offsetLeft +buttonWidth, buttonHeight * 4 + border);

	clearSlotsButton.setTopLeftPosition(border+ offsetLeft, buttonHeight * 6 + border);





	if (!isModeBank)
	{
		nameEdit->setSize(buttonWidth * 2, buttonHeight * 1);
		nameEdit->setTopLeftPosition(border+ offsetLeft, buttonHeight * 3 + border);
		factoryCombo.setVisible(false);
	} else
	{
		if (currentModePatchPatternBank == 0)
		{
			factoryCombo.setVisible(true);
			factoryCombo.setSize(buttonWidth, buttonHeight);
			factoryCombo.setTopLeftPosition(border+ offsetLeft, buttonHeight * 3 + border);
		} else
		{
			factoryCombo.setVisible(false);
		}
	}
	comboBoxDestination.setTopLeftPosition(border+ offsetLeft, buttonHeight*5+ border);
}



void PatchPatternSequence::changeListBoxRow(int newRow)
{

	currentListBoxRow = newRow;
	switch (this->currentModePatchPatternBank)
	{
		default:
		case 0:
			updatePatchUI(newRow, false, true, MIDI_SYSEX_PATCH_TRANSFER);
			patchSelection = newRow;
			break;
		case 1:
			updatePatchUI(newRow, false, true, MIDI_SYSEX_PATTERN_TRANSFER);
			patternSelection = newRow;
			break;
		case 2:
			updatePatchUI(newRow, false, true, MIDI_SYSEX_SEQUENCE_TRANSFER);
			sequenceSelection = newRow;
			break;
	}
	//this->nameEdit->setText(listBoxItems[newRow]->name);
}

void PatchPatternSequence::sendData(uint8_t bank, uint8_t preset)
{

	uint32_t bytesToSend  = 0;
	uint8_t model = holdingWindow->midiProcessor->motasModel;
	uint8_t channel = (uint8_t) holdingWindow->midiProcessor->getMIDIChannel();
	switch(this->currentModePatchPatternBank)
	{

		case 0:
			bytesToSend = Utilities::prepareSysExPacketsFromUnpacked(holdingWindow->midiProcessor->rawTransferBuffer,
					  patchBuffer, PATCH_LENGTH_BYTES_UNPACKED, MIDI_SYSEX_PATCH_TRANSFER, bank, preset, 0, model, channel);
			break;
		case 1:
			bytesToSend = Utilities::prepareSysExPacketsFromUnpacked(holdingWindow->midiProcessor->rawTransferBuffer,
				  patternBuffer, PATTERN_LENGTH_BYTES_UNPACKED, MIDI_SYSEX_PATTERN_TRANSFER, bank, preset, 0, model, channel);
			break;
		case 2:
			bytesToSend = Utilities::prepareSysExPacketsFromUnpacked(holdingWindow->midiProcessor->rawTransferBuffer,
			  sequenceBuffer, SEQUENCE_LENGTH_BYTES_UNPACKED, MIDI_SYSEX_SEQUENCE_TRANSFER, bank, preset, 0, model, channel);
			break;
		default:
			break;
	}
	//DBG("bytes to send (in MIDI form):"  + String( bytesToSend));
	holdingWindow->midiProcessor->sendDataOverMIDIDirect(holdingWindow->midiProcessor->rawTransferBuffer, bytesToSend, 0);
}



void PatchPatternSequence::buttonClicked(Button* b)
{

	if (b == this->patchesButton.get())
	{
		this->currentModePatchPatternBank = 0;
	}
	else if (b == this->patternsButton.get())
	{
		this->currentModePatchPatternBank = 1;
	}
	else if (b == this->sequencesButton.get())
	{
		this->currentModePatchPatternBank = 2;
	}
	else if (b == &bankButton)
	{
		DBG("Bank button pressed");
		if (b->getToggleState())
		{
			isModeBank = true;
			nameEdit->setVisible(false);
		} else
		{
			nameEdit->setVisible(true);
		}
	}else if (b == &copyAllPatchesFromPresets)
	{

		//memcpy(this->patchBuffer, holdingWindow->appCommon->getActivePatch(), PATCH_LENGTH_BYTES_UNPACKED);
		//updatePatchUI(currentListBoxRow, true, true, MIDI_SYSEX_PATCH_TRANSFER);
		for (int i = 0 ; i < BANK_OF_PATCHES_LENGTH; i++)
		{
			copyPatchToBank(i, i);
		}


	} else if (b == &copyAllPatchesToPresets)
	{
		//holdingWindow->midiProcessor->populatePatch(this->patchBuffer);
		//holdingWindow->setActiveTab(0);
		for (int i = 0 ; i < BANK_OF_PATCHES_LENGTH; i++)
		{
			copyPatchToPresets(i, i);
		}

	}
	else if (b == &singleButton)
	{
		if (b->getToggleState() )
			isModeBank = false;
	}
	else if (b == this->sendButton.get())
	{
		if (isModeBank)
		{
			// send all patches to Motas, to be stored in FLASH
			uint32 bytesToSend = 0;
			uint8* outBuffer = nullptr;

			uint8 model = holdingWindow->midiProcessor->motasModel;
			uint8 channel = (uint8_t) holdingWindow->midiProcessor->getMIDIChannel();
			uint8_t bankIndex = (uint8_t) this->comboBoxDestination.getSelectedItemIndex();
			switch (currentModePatchPatternBank)
			{
				case 0:
				{
					DBG("Sending patches mode: " + String(model) +  " ch:" + String(channel));
					outBuffer = new uint8[PATCH_LENGTH_BYTES_UNPACKED * BANK_OF_PATCHES_LENGTH *2];
					uint8 buffer[PATCH_LENGTH_BYTES_UNPACKED * BANK_OF_PATCHES_LENGTH];

					for (int i = 0; i < BANK_OF_PATCHES_LENGTH; i++)
					{
						memcpy(&buffer[(uint32_t) i * PATCH_LENGTH_BYTES_UNPACKED], bankOfPatches[i], PATCH_LENGTH_BYTES_UNPACKED);
					}
					bytesToSend
							= Utilities::prepareSysExPacketsFromUnpacked(outBuffer,buffer,
							PATCH_LENGTH_BYTES_UNPACKED * BANK_OF_PATCHES_LENGTH,
							MIDI_SYSEX_PATCH_BANK_TRANSFER,bankIndex, 0, 0, model, channel);

				}
					break;
				case 1:
				{
					outBuffer = new uint8[PATTERN_LENGTH_BYTES_UNPACKED * BANK_OF_PATTERNS_LENGTH *2];
					uint8 buffer[PATTERN_LENGTH_BYTES_UNPACKED * BANK_OF_PATTERNS_LENGTH];

					for (int i = 0; i < BANK_OF_PATTERNS_LENGTH; i++)
					{
						memcpy(&buffer[(uint32_t)i * PATTERN_LENGTH_BYTES_UNPACKED], bankOfPatterns[i], PATTERN_LENGTH_BYTES_UNPACKED);
					}
					bytesToSend
							= Utilities::prepareSysExPacketsFromUnpacked(outBuffer,buffer,
							PATTERN_LENGTH_BYTES_UNPACKED * BANK_OF_PATTERNS_LENGTH,
							MIDI_SYSEX_PATTERN_BANK_TRANSFER, bankIndex, 0, 0, model, channel);

				}
					break;
				case 2:
				{
					outBuffer = new uint8[SEQUENCE_LENGTH_BYTES_UNPACKED * BANK_OF_SEQUENCES_LENGTH *2];
					uint8 buffer[SEQUENCE_LENGTH_BYTES_UNPACKED * BANK_OF_SEQUENCES_LENGTH];

					for (int i = 0; i < BANK_OF_SEQUENCES_LENGTH; i++)
					{
						memcpy(&buffer[(uint32_t)i * SEQUENCE_LENGTH_BYTES_UNPACKED], bankOfSequences[i], SEQUENCE_LENGTH_BYTES_UNPACKED);
					}
					bytesToSend
							= Utilities::prepareSysExPacketsFromUnpacked(outBuffer,buffer,
							SEQUENCE_LENGTH_BYTES_UNPACKED * BANK_OF_SEQUENCES_LENGTH,
							MIDI_SYSEX_SEQUENCE_BANK_TRANSFER, bankIndex, 0, 0, model, channel);

				}
				break;
			}

			holdingWindow->midiProcessor->sendDataOverMIDIDirect(outBuffer, bytesToSend, 0);

			delete[] outBuffer;
			outBuffer = nullptr;


		} else
		{
			this->sendData(0x7E , (uint8_t) this->comboBoxDestination.getSelectedItemIndex()); // only send to RAM
		}

	}
	else if (b == this->requestButton.get())
	{
#if BUILD_STANDALONE_EXECUTABLE == 0
	MIDIProcessor* mp = holdingWindow->midiProcessor;
#else
	MIDIProcessor* mp = holdingWindow->midiProcessor.get();
#endif


		uint8_t location = (uint8_t) this->comboBoxDestination.getSelectedItemIndex();
		if (holdingWindow->midiComms != nullptr)
		{
			if (isModeBank)
			{
				 switch (currentModePatchPatternBank)
				{
					case 0:
						mp->commandRequest(MIDIProcessor::COMMAND_REQUEST_PATCH_BANK, &location, 1);
						//this->holdingWindow->midiProcessor->sendSysEx("get bank patches cmd", MIDI_SYSEX_REQUEST_PATCHES_BANK, location );
						break;
					case 1:
						mp->commandRequest(MIDIProcessor::COMMAND_REQUEST_PATTERN_BANK, &location, 1);
						//this->holdingWindow->midiProcessor->sendSysEx("get patterns bank cmd", MIDI_SYSEX_REQUEST_PATTERN_BANK, location );
						break;
					case 2:
						mp->commandRequest(MIDIProcessor::COMMAND_REQUEST_SEQUENCE_BANK, &location, 1);
						//this->holdingWindow->midiProcessor->sendSysEx("get sequence bank cmd", MIDI_SYSEX_REQUEST_SEQUENCE_BANK, location );
						break;
					default:
						break;
				}
			} else
			{
				// Motas ignores request for certain RAM location, it always sends the active one
				switch (currentModePatchPatternBank)
				{
					case 0:
						mp->commandRequest(MIDIProcessor::COMMAND_REQUEST_PATCH, nullptr, 0);
						//this->holdingWindow->midiProcessor->sendSysEx("get patch cmd", MIDI_SYSEX_REQUEST_PATCH, 0);
						break;
					case 1:
						mp->commandRequest(MIDIProcessor::COMMAND_REQUEST_PATTERN, nullptr, 0);
						//this->holdingWindow->midiProcessor->sendSysEx("get pattern cmd", MIDI_SYSEX_REQUEST_PATTERN, 0);
						break;
					case 2:
						mp->commandRequest(MIDIProcessor::COMMAND_REQUEST_SEQUENCE, nullptr, 0);
						//this->holdingWindow->midiProcessor->sendSysEx("get sequence cmd", MIDI_SYSEX_REQUEST_SEQUENCE, 0);
						break;
					default:
						break;
				}
			}
		}

		//DBG("Request from Motas");
	} else if (b == &saveButton || b== &loadButton)
	{
		String itemName;
		if (isModeBank)
		{
			itemName = "bank_" + String(comboBoxDestination.getSelectedId() - 1);
		} else
		{
			itemName = nameEdit->getText().trim();
		}
		loadSave(b == &saveButton, itemName );
	} else if (b == & clearSlotsButton)
	{
		switch (this->currentModePatchPatternBank)
		{
			case 0:
				for (int i = 0; i < BANK_OF_PATCHES_LENGTH; i++)
				 {
					  uint8* d = bankOfPatches[i];
					  memset(d, 0, PATCH_LENGTH_BYTES_UNPACKED); // 'erase' patch
				 }
				break;
			case 1:
				for (int i = 0; i < bankOfPatterns.size(); i++)
				{
					uint8* d = bankOfPatterns[i];;
					memset(d, 0, PATTERN_LENGTH_BYTES_UNPACKED);
				}
				break;
			case 2:
				for (int i = 0; i < bankOfSequences.size(); i++)
				{
					uint8* d = bankOfSequences[i];;
					memset(d, 0, SEQUENCE_LENGTH_BYTES_UNPACKED);
				}
				break;
		}
		updateCurrentDisplayData();
	}

	DBG( "isModeBank = " + String((int) isModeBank ));
	if (b != this->requestButton.get() && b != sendButton.get())
	{
		updateCurrentDisplayData();
	}

	if (currentModePatchPatternBank == 0)
	{
		copyAllPatchesToPresets.setVisible(true);
		copyAllPatchesFromPresets.setVisible(true);
	} else
	{
		copyAllPatchesToPresets.setVisible(false);
		copyAllPatchesFromPresets.setVisible(false);
	}
}


void PatchPatternSequence::loadSave(bool isSaving, String itemName)
{
	std::unique_ptr<WildcardFileFilter> wildcardFilter;// ("*.*", String(), "Foo files");
	String summaryText;

	String text;
	int mode;
	if (isSaving)
	{
		summaryText = "Save ";
		text = "Please choose the filename and location...";
		mode = FileBrowserComponent::saveMode | FileBrowserComponent::canSelectFiles;
	}
	else
	{
		summaryText = "Load ";
		text = "Please choose the file that you want to load...";
		mode = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;
	}
	String extension;
	switch (this->currentModePatchPatternBank)
	{
		case 0:
			if (isModeBank)
			{
				extension = ".bpatch";
				wildcardFilter = std::make_unique<WildcardFileFilter>("*" + extension, String(), "Bank of patches file");
				summaryText += "patch bank";
			}
			else
			{
				extension = ".patch";
				wildcardFilter = std::make_unique<WildcardFileFilter>("*" + extension, String(), "Patch file");
				summaryText += "patch";
			}
			break;
		case 1:
			if (isModeBank)
			{
				extension = ".bpatt";
				wildcardFilter = std::make_unique<WildcardFileFilter>("*" + extension, String(), "Bank of patterns file");
				summaryText += "pattern bank";
			}
			else
			{
				extension = ".patt";
				wildcardFilter = std::make_unique<WildcardFileFilter>("*" + extension, String(), "Pattern file");
				summaryText += "pattern";
			}
			break;
		case 2:
			if (isModeBank)
			{
				extension = ".bseq";
				wildcardFilter = std::make_unique<WildcardFileFilter>("*" + extension, String(), "Bank of sequences file");
				summaryText += "sequence bank";
			}
			else
			{
				extension = ".seq";
				wildcardFilter = std::make_unique<WildcardFileFilter>("*" + extension, String(), "Sequence file");
				summaryText += "sequence";
			}
			break;
	}



	FileBrowserComponent browser (mode,	File(holdingWindow->getDefaultPath()),	  wildcardFilter.get(),  nullptr);
	browser.setFileName(itemName + extension);
	FileChooserDialogBox dialogBox (summaryText,	text,browser,	true,Colours::darkgrey);
	if (dialogBox.show())
	{
		// if file chosen
		File f = browser.getSelectedFile (0);



		if (isSaving)
		{
			DBG("Save, filename: " + f.getFileName());
			f.deleteFile();
			FileOutputStream fileStream(f);

			if (!fileStream.openedOk())
			{
#if JUCE_MODAL_LOOPS_PERMITTED
				AlertWindow::showMessageBox	(AlertWindow::WarningIcon , "Error", "Problem opening file", "OK", nullptr);
#endif
				return;
			}
			//holdingWindow->updatePath(f.getFullPathName());
			holdingWindow->updatePath(f.getParentDirectory().getFullPathName());
			switch(this->currentModePatchPatternBank)
			{
				case 0:
					if (isModeBank)
					{
						for (int i = 0; i < BANK_OF_PATCHES_LENGTH; i++)
						{
							fileStream.write(bankOfPatches[i], PATCH_LENGTH_BYTES_UNPACKED);
						}
					}
					else
					{
						fileStream.write(this->patchBuffer, PATCH_LENGTH_BYTES_UNPACKED);
					}
					break;
				case 1:
					if (isModeBank)
					{
						for (int i = 0; i < BANK_OF_PATTERNS_LENGTH; i++)
						{
							fileStream.write(bankOfPatterns[i], PATTERN_LENGTH_BYTES_UNPACKED);
						}
					}
					else
					{
						fileStream.write(this->patternBuffer, PATTERN_LENGTH_BYTES_UNPACKED);
					}
					break;
				case 2:
					if (isModeBank)
					{
						for (int i = 0; i < BANK_OF_SEQUENCES_LENGTH; i++)
						{
							fileStream.write(bankOfSequences[i], SEQUENCE_LENGTH_BYTES_UNPACKED);
						}
					}
					else
					{
						fileStream.write(this->sequenceBuffer, SEQUENCE_LENGTH_BYTES_UNPACKED);
					}
					break;
				default:
					break;
			}
		}
		else
		{
			DBG("Load, filename: " + f.getFileName());

			//    QFileInfo fileInfo(fileName);
			//    fileNameLocationDefault = fileInfo.absolutePath() ;


			    //QFile f(fileName);
			 //   qDebug() << fileName << "file size: " << f.size();
			int size = (int) f.getSize();
			DBG("File size " + String(size));
			//holdingWindow->updatePath(f.getFullPathName());
			holdingWindow->updatePath(f.getParentDirectory().getFullPathName());

			switch (this->currentModePatchPatternBank)
			{
				case 0:
					if (isModeBank)
					{
						if (size != PATCH_LENGTH_BYTES_UNPACKED * BANK_OF_PATCHES_LENGTH)
						{
#if JUCE_MODAL_LOOPS_PERMITTED
							//QMessageBox::warning(this, "Error", "ERROR IN FILE SIZE");
							AlertWindow::showMessageBox	(AlertWindow::WarningIcon , "Error", "Patch bank file size error", "OK", nullptr);

#endif
						} else
						{
							FileInputStream fileStream(f);
							if (fileStream.openedOk())
							{
								for (int i = 0; i < BANK_OF_PATCHES_LENGTH; i++)
								{
									fileStream.read(bankOfPatches[i], PATCH_LENGTH_BYTES_UNPACKED);
									convertPatchToLatestVersion((tAllParams*) bankOfPatches[i]);
									updatePatchUI(i, false, i == 0, MIDI_SYSEX_PATCH_TRANSFER);
								}
							}
						}
					} else
					{
						if (size != PATCH_LENGTH_BYTES_UNPACKED)
						{
#if JUCE_MODAL_LOOPS_PERMITTED
							//QMessageBox::warning(this, "Error", "ERROR IN FILE SIZE");
							AlertWindow::showMessageBox	(AlertWindow::WarningIcon , "Error", "Patch file size error", "OK", nullptr);
#endif
						} else
						{
							FileInputStream fileStream(f);
							if (fileStream.openedOk())
							{
								fileStream.read(this->patchBuffer, PATCH_LENGTH_BYTES_UNPACKED);
								convertPatchToLatestVersion((tAllParams*) this->patchBuffer);
								updatePatchUI(currentListBoxRow, false, true, MIDI_SYSEX_PATCH_TRANSFER);
							}
						}
					}
					break;
				case 1:
					if (isModeBank)
					{
						if (size != PATTERN_LENGTH_BYTES_UNPACKED * BANK_OF_PATTERNS_LENGTH)
						{
							//QMessageBox::warning(this, "Error", "ERROR IN FILE SIZE");
#if JUCE_MODAL_LOOPS_PERMITTED
							AlertWindow::showMessageBox	(AlertWindow::WarningIcon , "Error", "Pattern bank file size error", "OK", nullptr);
#endif
						} else
						{
							FileInputStream fileStream(f);
							if (fileStream.openedOk())
							{
								for (int i = 0; i < BANK_OF_PATTERNS_LENGTH; i++)
								{
									fileStream.read(bankOfPatterns[i], PATTERN_LENGTH_BYTES_UNPACKED);
									updatePatchUI(i, false, i == 0, MIDI_SYSEX_PATTERN_TRANSFER);
								}
							}
						}
					} else
					{
						if (size != PATTERN_LENGTH_BYTES_UNPACKED)
						{
							//QMessageBox::warning(this, "Error", "ERROR IN FILE SIZE");
#if JUCE_MODAL_LOOPS_PERMITTED
							AlertWindow::showMessageBox	(AlertWindow::WarningIcon , "Error", "Pattern file size error", "OK", nullptr);
#endif
						} else
						{
							FileInputStream fileStream(f);
							if (fileStream.openedOk())
							{
								fileStream.read(this->patternBuffer, PATTERN_LENGTH_BYTES_UNPACKED);
								updatePatchUI(currentListBoxRow, false, true, MIDI_SYSEX_PATTERN_TRANSFER);
							}
						}
					}
					break;
				case 2:
					if (isModeBank)
					{
						if (size != SEQUENCE_LENGTH_BYTES_UNPACKED * BANK_OF_SEQUENCES_LENGTH)
						{
							//QMessageBox::warning(this, "Error", "ERROR IN FILE SIZE");
#if JUCE_MODAL_LOOPS_PERMITTED
							AlertWindow::showMessageBox	(AlertWindow::WarningIcon , "Error", "Sequence bank file size error", "OK", nullptr);
#endif
						} else
						{
							FileInputStream fileStream(f);
							if (fileStream.openedOk())
							{
								for (int i = 0; i < BANK_OF_SEQUENCES_LENGTH; i++)
								{
									fileStream.read(bankOfSequences[i], SEQUENCE_LENGTH_BYTES_UNPACKED);
									updatePatchUI(i, false, i == 0, MIDI_SYSEX_SEQUENCE_TRANSFER);
								}
							}
						}
					} else
					{
						if (size != SEQUENCE_LENGTH_BYTES_UNPACKED)
						{
							//QMessageBox::warning(this, "Error", "ERROR IN FILE SIZE");
#if JUCE_MODAL_LOOPS_PERMITTED
							AlertWindow::showMessageBox	(AlertWindow::WarningIcon , "Error", "Sequence file size error", "OK", nullptr);
#endif
						} else
						{
							FileInputStream fileStream(f);
							if (fileStream.openedOk())
							{
								fileStream.read( this->sequenceBuffer, SEQUENCE_LENGTH_BYTES_UNPACKED);
								updatePatchUI(currentListBoxRow, false, true, MIDI_SYSEX_SEQUENCE_TRANSFER);
							}

						}

					}
					break;
				default:
					break;
			}
		}
	}


}
void PatchPatternSequence::refreshListBox()
{
	int end;
	int type;
	switch (currentModePatchPatternBank)
	{
		default:
		case 0:
			end = BANK_OF_PATCHES_LENGTH;
			type = MIDI_SYSEX_PATCH_TRANSFER;
			break;
		case 1:
			end = BANK_OF_PATTERNS_LENGTH;
			type = MIDI_SYSEX_PATTERN_TRANSFER;
			break;
		case 2:
			end = BANK_OF_SEQUENCES_LENGTH;
			type = MIDI_SYSEX_SEQUENCE_TRANSFER;
			break;
	}
	for (int i = 0; i < end; i++)
	{
		//updatePatchUI(i, false, i == end - 1, type);
		updatePatchUI(i, false, false, type);
	}
	updatePatchUI(currentListBoxRow, false, true, type);
	listBox->repaint();
}


void PatchPatternSequence::populateCurrentPatch(uint8_t* buffer)
{
	memcpy(bankOfPatches[currentListBoxRow], buffer, PATCH_LENGTH_BYTES_UNPACKED);
	updatePatchUI(currentListBoxRow, false, true, MIDI_SYSEX_PATCH_TRANSFER);
}

void PatchPatternSequence::populateAllPatches(uint8_t* buffer)
{
	DBG("Populating all patches");
	for (uint32_t i = 0; i < BANK_OF_PATCHES_LENGTH; i++)
	{
		memcpy(bankOfPatches[i], &buffer[i * PATCH_LENGTH_BYTES_UNPACKED], PATCH_LENGTH_BYTES_UNPACKED);
	}
	refreshListBox();
}


void PatchPatternSequence::populateCurrentPattern(uint8_t* buffer)
{
	if (currentListBoxRow > 0 && currentListBoxRow < bankOfPatterns.size())
	{
		memcpy(bankOfPatterns[currentListBoxRow], buffer, PATTERN_LENGTH_BYTES_UNPACKED);
		updatePatchUI(currentListBoxRow, false, true, MIDI_SYSEX_PATTERN_TRANSFER);
	}
}

void PatchPatternSequence::populateAllPatterns(uint8_t* buffer)
{
	for (uint32_t i = 0; i < BANK_OF_PATTERNS_LENGTH; i++)
	{
		memcpy(bankOfPatterns[(int)i], &buffer[(int) (i * PATTERN_LENGTH_BYTES_UNPACKED)], (int32_t)  PATTERN_LENGTH_BYTES_UNPACKED);

	}
	refreshListBox();
}

void PatchPatternSequence::populateCurrentSequence(uint8_t* buffer)
{
	if (currentListBoxRow > 0 && currentListBoxRow < bankOfSequences.size())
	{
		memcpy(bankOfSequences[currentListBoxRow], buffer, SEQUENCE_LENGTH_BYTES_UNPACKED);
		updatePatchUI(currentListBoxRow, false, true, MIDI_SYSEX_SEQUENCE_TRANSFER);
	}
}

void PatchPatternSequence::populateAllSequences(uint8_t* buffer)
{

	for (uint32_t i = 0; i < BANK_OF_SEQUENCES_LENGTH; i++)
	{
		memcpy(bankOfSequences[(int)i], &buffer[(int)(i * SEQUENCE_LENGTH_BYTES_UNPACKED)], (int32_t) SEQUENCE_LENGTH_BYTES_UNPACKED);
	}
	refreshListBox();
}

void PatchPatternSequence::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
	if (comboBoxThatHasChanged == &factoryCombo)
	{
		int id = factoryCombo.getSelectedItemIndex();
		if (id >= 0)
		{
			this->getFactoryPatches(id);
		}
		factoryCombo.setSelectedItemIndex(-1, NotificationType::dontSendNotification);
	}
}

void PatchPatternSequence::textEditorTextChanged(TextEditor& t)
{
	///DBG("Text edited");

	String newName = t.getText();



	// clear the name
	switch (this->currentModePatchPatternBank)
	{
		case 0:
			//memset(&patchBuffer[8], 32, NAME_LENGTH);
			Utilities::setName(newName, patchBuffer);
			break;
		case 1:
			//memset(&patternBuffer[8], 32, NAME_LENGTH);
			Utilities::setName(newName, patternBuffer);
			break;
		case 2:
			//memset(&sequenceBuffer[8], 32, NAME_LENGTH);
			Utilities::setName(newName, sequenceBuffer);
			break;
	}


/*
	// write the new name
	for (int i = 0; i < len ; i++)
	{
		uint8_t val = b[i];
		if (val < 32 || val >= 0x7F)
			val = 32;
		switch (this->currentModePatchPatternBank)
		{
			case 0:
				patchBuffer[i + 8] = val;
				break;
			case 1:
				patternBuffer[i + 8] = val;
				break;
			case 2:
				sequenceBuffer[i + 8] = val;
			   break;
		}
	}
*/
	switch (this->currentModePatchPatternBank)
	{
		case 0:
			updatePatchUI(currentListBoxRow, true, true, MIDI_SYSEX_PATCH_TRANSFER);
			break;
		case 1:
			updatePatchUI(currentListBoxRow, true, true, MIDI_SYSEX_PATTERN_TRANSFER);
			break;
		case 2:
			updatePatchUI(currentListBoxRow, true, true, MIDI_SYSEX_SEQUENCE_TRANSFER);
			break;
	}



	//delete[] b;
	//updateCurrentDisplayData();
}


void PatchPatternSequence::updateCurrentDisplayData()
{


	int row;

	comboBoxDestination.clear();
	listBoxItems.clear();

	int numItems;
	listBox->setHoldingType(this->currentModePatchPatternBank);
	switch (this->currentModePatchPatternBank)
	{
	  default:
	  case 0:
		  row = patchSelection;
		  numItems = BANK_OF_PATCHES_LENGTH;
		  break;
	  case 1:
		  row = patternSelection;
		  numItems = BANK_OF_PATTERNS_LENGTH;
		  break;
	  case 2:
		  row = sequenceSelection;
		  numItems = BANK_OF_SEQUENCES_LENGTH;
		  break;
	}


    for (int i = 0; i < numItems; i++)
	{
		String name;
		if (i < 9)
			name = "[0" + String(i + 1) + "]";
		else
			name = "[" + String(i + 1) + "]";
		ListEntry::Ptr entry = new ListEntry(name);
		listBoxItems.add(entry);
	}

	// update the selection status of the combo-box

  // this->currentListBoxRow = 0;
    SparseSet<int> selectedRows;
	Range<int> r(row,row + 1);
	selectedRows.addRange(r);
	listBox->setSelectedRows(selectedRows, NotificationType::dontSendNotification);




	DBG("currentListBoxRow: " + String(currentListBoxRow));

	currentListBoxRow = row;
	 refreshListBox();
	 listBox->scrollToEnsureRowIsOnscreen(row);
  // listBox->syncSelectedItemsWithDeviceList (listBoxItems);
	//changeListBoxRow(row);

    switch (this->currentModePatchPatternBank)
    {
        case 0:
        	if (isModeBank)
        	{
        		requestButton->setButtonText("Request patches");
        		sendButton->setButtonText("Send patches");
        		saveButton.setButtonText("Save patches");
        		loadButton.setButtonText("Load patches");
				for (int i = 0; i < 10; i++)
				{
					String text = String("FLASH bank: ") + String(i + 1);
					comboBoxDestination.addItem(text, i + 1);
				}
        	} else
        	{
        		requestButton->setButtonText("Request patch");
        		sendButton->setButtonText("Send patch");
        		saveButton.setButtonText("Save patch");
        		loadButton.setButtonText("Load patch");
        		for (int i = 0; i < 5; i++)
				{
        			String text = "Preset RAM: " + String(i + 1);
        			comboBoxDestination.addItem(text, i + 1);
				}
        	}
        //    updatePatchUI(currentListBoxRow, true, true, MIDI_SYSEX_PATCH_TRANSFER);

            break;
        case 1:
        	if (isModeBank)
        	{
				requestButton->setButtonText("Request patterns");
				sendButton->setButtonText("Send patterns");
				saveButton.setButtonText("Save patterns");
				loadButton.setButtonText("Load patterns");
				for (int i = 0; i < 7; i++)
				{
					String text = "FLASH bank: "+  String(i + 1);
					comboBoxDestination.addItem(text, i + 1);
				}
        	} else
        	{
        		requestButton->setButtonText("Request pattern");
        		sendButton->setButtonText("Send pattern");
        		saveButton.setButtonText("Save pattern");
        		loadButton.setButtonText("Load pattern");
				for (int i = 0; i < 15; i++)
				{
					char val = 'A'  + (char) i;
					String text = "Location: "  + String::charToString(val);
					comboBoxDestination.addItem(text ,i + 1);
				}
        	}
          //  updatePatchUI(currentListBoxRow, true, true, MIDI_SYSEX_PATTERN_TRANSFER);

            break;
        case 2:
        	if (isModeBank)
        	{
        		requestButton->setButtonText("Request sequences");
        		sendButton->setButtonText("Send sequences");
        		saveButton.setButtonText("Save sequences");
        		loadButton.setButtonText("Load sequences");
				for (int i = 0; i < 2; i++)
				{
					String text =  "FLASH bank: "+  String(i + 1);
					comboBoxDestination.addItem(text, i + 1);
				}
        	}
        	else
        	{
        		requestButton->setButtonText("Request sequence");
        		sendButton->setButtonText("Send sequence");
        		saveButton.setButtonText("Save sequence");
        		loadButton.setButtonText("Load sequence");

				for (int i = 0; i < 1; i++)
				{
					comboBoxDestination.addItem("Current RAM", i + 1);
				}
        	}
        //	updatePatchUI(currentListBoxRow, true, true, MIDI_SYSEX_SEQUENCE_TRANSFER);
            break;
    }
    comboBoxDestination.setSelectedItemIndex(0);

    resized();
}

void PatchPatternSequence::saveData(int type, uint8_t* buffer)
{
	std::unique_ptr<WildcardFileFilter> wildcardFilter;// ("*.*", String(), "Foo files");
	String summaryText;

	String text;
	int mode;

	summaryText = "Save ";
	text = "Please choose the file that you want to save...";
	mode = FileBrowserComponent::saveMode | FileBrowserComponent::canSelectFiles;

	String ext;
	switch (type)
	{
		default:
		case MIDI_SYSEX_REQUEST_BULK_FLASH_DATA_SEND:
			wildcardFilter = std::make_unique<WildcardFileFilter>("*.dat", String(), "Motas bulk data file");
			summaryText += "bulk data";
			ext = "/*.dat";
			text += " (*.dat)";
			break;
		case MIDI_SYSEX_GLOBALS_TRANSFER:
			wildcardFilter = std::make_unique<WildcardFileFilter>("*.setup", String(), "Motas setup data file");
			summaryText += "setup data";
			ext = "/*.setup";
			text += " (*.setup)";
			break;
	}
	DBG("Save data function");
	FileBrowserComponent browser (mode,	File(holdingWindow->getDefaultPath() + ext),	  wildcardFilter.get(),  nullptr);
	FileChooserDialogBox dialogBox (summaryText,	text,browser,	true,Colours::darkgrey);
	if (dialogBox.show())
	{
		// if file chosen
		File f = browser.getSelectedFile (0);
		//holdingWindow->updatePath(f.getFullPathName());
		holdingWindow->updatePath(f.getParentDirectory().getFullPathName());



		DBG("Save, filename: " + f.getFileName());
		f.deleteFile();
		FileOutputStream fileStream(f);

		if (!fileStream.openedOk())
		{
#if JUCE_MODAL_LOOPS_PERMITTED
			AlertWindow::showMessageBox	(AlertWindow::WarningIcon , "Error", "Problem opening file", "OK", nullptr);
#endif
			return;
		}

		switch(type)
		{
			case MIDI_SYSEX_REQUEST_BULK_FLASH_DATA_SEND:
				fileStream.write(buffer, MASS_FLASH_TRANSFER_LENGTH_BYTES);
				break;
			case MIDI_SYSEX_GLOBALS_TRANSFER:
				fileStream.write(buffer, SETUP_DATA_LENGTH_BYTES);
				break;
			default:
				break;
		}

	}
}
/*
void PatchPatternSequence::dragOperationStarted(
		const DragAndDropTarget::SourceDetails&)
{
	DBG("Drag started");
}*/

void PatchPatternSequence::mouseDrag(const MouseEvent& )
{
	DBG("PatchPatternSequence mouse Drag started");
}

void PatchPatternSequence::updatePresetsDisplay()
{

	int row = 1;

	listBoxPresetsItems.clear();


	for (int i = 0; i < 50; i++)
	{
		String name;
		if (i < 9)
			name = "...[0" + String(i + 1) + "]";
		else
			name = "..[" + String(i + 1) + "]";
		ListEntry::Ptr entry = new ListEntry(name);
		listBoxPresetsItems.add(entry);
		updatePresetPatch(i);
	}

	SparseSet<int> selectedRows;
	Range<int> r(row,row + 1);
	selectedRows.addRange(r);
	listBoxPresets->setSelectedRows(selectedRows, NotificationType::dontSendNotification);




//	DBG("currentListBoxRow: " + String(currentListBoxRow));

//	currentListBoxRow = row;
	//refreshListBox();
	listBoxPresets->scrollToEnsureRowIsOnscreen(row);

	listBoxPresets->repaint();


}

void PatchPatternSequence::copyPatchToPresets(int source, int dest)
{


	DBG("Copy patch to presets, source: " + String(source) + " dest: " + String(dest));

	// copy from the patch list to the plugin presets list
	// copy into the Edit and normal presets list

	if (source >= 0  && source < BANK_OF_PATCHES_LENGTH)
	{
		holdingWindow->appCommon->updatePatch(dest, this->bankOfPatches[source]);


		// update the displayed name
		updatePresetPatch(dest);
		holdingWindow->mainComponent->updatePresetComboBox(false);
	}


}

void PatchPatternSequence::copyPatchToBank(int source, int dest)
{

	DBG("Copy patch to bank, source: " + String(source) + " dest: " + String(dest));

	if (source >= 0  && source < BANK_OF_PATCHES_LENGTH
			&& dest >=0 && dest < BANK_OF_PATCHES_LENGTH)
	{
		memcpy(this->bankOfPatches[dest], holdingWindow->appCommon->patchPresetsEditedArray[source], PATCH_LENGTH_BYTES_UNPACKED);

		updatePatchUI(dest, false, true, MIDI_SYSEX_PATCH_TRANSFER);
	}




}

int32_t PatchPatternSequence::convertEGTimeToValue_CONVERSION(int32_t value)
{

	// take 1260ns
	#define EG_MULT_LOG 394.76f
	if (value < 1) //  protect against corrupted data
		value = 1;

	// minimum value should be is 8

	//int32_t val =  4095 - (int32_t) (393.9f * logf((float) EG_RATE_MAX_VALUE / value ) );
	int32_t val =  (int32_t) (EG_MULT_LOG * logf((float) EG_RATE_MAX_VALUE / value ) );
	if (val < 0)
		val = 0;
	else if (val > 4095)
		val = 4095;
	return val;
}

void PatchPatternSequence::resetCustomLFOsInPatch(tAllParams* p)
{
	int j;
	for (j = 0; j < CUSTOM_LFO_MAX_NUMBER; j++)
	{
		t_customLFOWave* clfo = &p->params.paramNames.customLFOs[j];
		// corrupted values / invalid, initialise shape
		int i;
		for (i = 0; i < CUSTOM_LFO_VERTICES; i++)
		{

			// draw the LFO value
			clfo->values[i] = 0;
			clfo->meta[i].mode = CUSTOM_LFO_MODE_STEPPED;
			clfo->meta[i].active = 0;
		}
		clfo->meta[0].active = 1;// 1 active node
		clfo->version = CUSTOM_LFO_VERSION_NUMBER;
	}
	//gEditIndex = 0;
}


/*
 *
 * This clears the advanced modulation settings for this slot
 */
void PatchPatternSequence::clearAdvancedMod(t_modOfMod* m)
{
	memset(m, 0, sizeof(t_modOfMod));
}

/*
 *
 *
 * This clears all the advanced modulation settings for the patch
 */
void PatchPatternSequence::clearAdvancedModPatch(tAllParams* p)
{
	p->advancedModulationMode = 0; // turn off advanced modulation
	// this will prevent any amod changes to the patch, since when amod run operates will see is disabled
	int i;
	for (i = 0; i < PARAM_END; i++)
	{
		t_modOfMod* m = &p->params.singleParams[i].more.lfo2Mod_And_ModofMod.modOfmodulation;
		clearAdvancedMod(m);
	}
}


void PatchPatternSequence::convertPatchToLatestVersion(tAllParams* source)
{





	if (source->header.versionNumber < T_ALL_PARAMS_VERSION_8)
	{
		DBG("Convert patch t0 v8, currently  is version " + String(source->header.versionNumber));

		// changed way in which LFO freq works for LFO clock sync
		int i;
		for (i = 0; i < NUM_GLOBAL_EGS; i++)
		{
			int32_t f = source->commonLFOs[i].freq;
			if (f < 0) // if was in sync mode
			{
				// range was -33... -1 => 0 ... 4095
				// was in clock sync mode
				// old code: int change  = 32 - (-1 - LFO->freq) -> 0 .. 32
				// new code does: int change  = 32 - (1 + LFO->freq) / 128;
				uint32_t ff = (uint32_t) ((128 *(-f - 1)));
				if (ff > 4095)
					ff = 4095;
				source->commonLFOs[i].freq = (int16_t) ff;
				source->commonLFOs[i].extras.pitchTrack = LFO_MODE_CLK_SYNC;

			}

		}
		if (source->header.versionNumber < T_ALL_PARAMS_VERSION_7)
		{
			clearAdvancedModPatch(source);

			for (i = 0; i < PARAM_END; i++)
			{
				// definition of EG time storage  has changed!
				// convert the raw EG values previous stored, into the 0.. 4095 values now stored in this version
				source->params.singleParams[i].EG0.attack = convertEGTimeToValue_CONVERSION(source->params.singleParams[i].EG0.attack);
				source->params.singleParams[i].EG0.decay = convertEGTimeToValue_CONVERSION(source->params.singleParams[i].EG0.decay);
				source->params.singleParams[i].EG0.release = convertEGTimeToValue_CONVERSION(source->params.singleParams[i].EG0.release);
			}
			for (i = 0; i < NUM_GLOBAL_EGS; i++)
			{
				source->commonEGs[i].attack = convertEGTimeToValue_CONVERSION(source->commonEGs[i].attack);
				source->commonEGs[i].decay = convertEGTimeToValue_CONVERSION(source->commonEGs[i].decay);
				source->commonEGs[i].release = convertEGTimeToValue_CONVERSION(source->commonEGs[i].release);
			}

			if (source->header.versionNumber < T_ALL_PARAMS_VERSION_6)
			{
				for (i = 0; i < NUMBER_OF_EG_TRIGGER_SOURCES; i++)
				{
					source->egTriggerSource[i] = MOD_SOURCE_MODULATION_WHEEL;
				}
				if (source->header.versionNumber < T_ALL_PARAMS_VERSION_5)
				{
					// remap the older modulation settings, we added the "off" option in T_ALL_PARAMS_VERSION_5
					source->MOD1Source++;
					source->MOD2Source++;
					source->MOD3Source++;
					source->MOD4Source++;

					if (source->header.versionNumber < T_ALL_PARAMS_VERSION_4)
					{

						resetCustomLFOsInPatch(source);
						if (source->header.versionNumber < T_ALL_PARAMS_VERSION_3)
						{
							if (source->header.versionNumber < T_ALL_PARAMS_VERSION_2)
							{
								source->header.microTuneOption = 0; // force equal tuning on old patch definition
							}
							// remap old EG shape definitions. Previously 0 = LOG, 1 = LINEAR, 2 = EXP
							int8_t* p;

							for (i = 0; i < NUM_GLOBAL_EGS; i++)
							{
								p = &source->commonEGs[i].egShape;

								if (*p == 1)
									*p = 0;
								else if (*p == 2)
									*p = -32;
								else
									*p = 32;
							}
							for (i = 0; i < PARAM_END; i++)
							{
								p = &source->params.singleParams[i].EG0.egShape;
								if (*p == 1)
									*p = 0;
								else if (*p == 2)
									*p = -32;
								else
									*p = 32;
							}
						}
					}
				}
			}
		}

		// set as new version now

		source->header.versionNumber = T_ALL_PARAMS_VERSION_8;
		//t->header.bitfield.writeProtected = 0; // turn off write protect


		// now re-calculate CRC,

		uint8_t* buf = (uint8_t*) source;

		uint32_t crcVal = crcCalculator::crcFastSTMF4(buf + 8, PATCH_LENGTH_BYTES_UNPACKED - 8U); // calculate CRC of all data (excluding first 8 bytes, and final 4)


		Utilities::write32(crcVal, 4, buf); // write new CRC to the patchbuffer
	}
}


void PatchPatternSequence::getFactoryPatches(int bankNum)
{

	for (uint32_t i = 0; i < BANK_OF_PATCHES_LENGTH; i++)
	{

		switch (bankNum)
		{
		default:
		case 0:
			memcpy(bankOfPatches[i], MotasEditResources::FACTORY_BANK0_FINAL_rev1_bpatch + (i * PATCH_LENGTH_BYTES_UNPACKED),
								PATCH_LENGTH_BYTES_UNPACKED);
			break;
		case 1:
			memcpy(bankOfPatches[i], MotasEditResources::FACTORY_BANK1_FINAL_rev1_bpatch + (i * PATCH_LENGTH_BYTES_UNPACKED),
								PATCH_LENGTH_BYTES_UNPACKED);
			break;

		case 2:
			memcpy(bankOfPatches[i], MotasEditResources::MOTAS_BANK_TONAL_AXiS_bpatch + (i * PATCH_LENGTH_BYTES_UNPACKED),
								PATCH_LENGTH_BYTES_UNPACKED);
			break;
		case 3:
			memcpy(bankOfPatches[i], MotasEditResources::MOTAS_BANK_TONAL_AXiS_NUM2_bpatch + (i * PATCH_LENGTH_BYTES_UNPACKED),
								PATCH_LENGTH_BYTES_UNPACKED);
			break;
		case 4:
			memcpy(bankOfPatches[i], MotasEditResources::FACTORY_BANK4_rev1_bpatch + (i * PATCH_LENGTH_BYTES_UNPACKED),
								PATCH_LENGTH_BYTES_UNPACKED);
			break;
		}
		convertPatchToLatestVersion((tAllParams*) bankOfPatches[i]);
	}




	refreshListBox();




}
