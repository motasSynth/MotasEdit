
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
 * advancedMod.cpp
 *
 *
 */



#include "Headers/advancedMod.h"

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
#include "Headers/PanelControl.h"
#include "Headers/CustomSlider.h"
#include "Headers/pageParams.h"

static char name[MAX_ADVANCED_MOD_NAME_LENGTH + 4];






static void addNumeric(char* c, uint8_t val)
{

	if (val > 9)
	{
		c[0] = '0' + val / 10;
		c[1] = '0' + val % 10;
		c[0 + 2] = 0;
	} else
	{
		c[0] = '0' + val;
		c[0 + 1] = 0;
	}
}


static const char fixedValuesText[MAX_FIXED_SOURCE_VALUES][MAX_ADVANCED_MOD_NAME_LENGTH] =
{

		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"10",
		"11",
		"12",
		"13",
		"14",
		"15",
		"16",
		"32",
		"64",
		"128",
	//	"256",
	//	"512",
	//	"1024",


};


static const char noteText[7][MAX_ADVANCED_MOD_NAME_LENGTH] =
{
		"last note",
		"1st note",
		"2nd note",
		"3rd note",
		"highest note",
		"middle note",
		"lowest note"
};

static const char pageNames[PARAM_END][MAX_ADVANCED_MOD_NAME_LENGTH] =
{
		"master pitch",
		"osc-1:level",
		"osc-1:pitch",
		"osc-1:tri",
		"osc-1:saw",
		"osc-1:pl mod",
		"osc-1:pl lev",

		"osc-2:ph mod",
		"osc-2:level",
		"osc-2:pitch",
		"osc-2:tri",
		"osc-2:saw",
		"osc-2:square",
		"osc-2:subosc",

		"osc-3:ph mod",
		"osc-3:level",
		"osc-3:pitch",
		"osc-3:tri",
		"osc-3:saw",
		"osc-3:pl mod",
		"osc-3:pl lev",

		"mixer:noise",
		"mixer:level",

		"lpf1:freq",
		"lpf1:res",
		"lpf1:out",

		"hpf:freq",
		"hpf:out",


		"mix:fb/ext",


		"lpf2:freq",
		"lpf2:res",
		"lpf2:out",


		"output",

};



static const String getSourceParamText(int page, uint8_t param)
{
	const char* t;

	switch(page)
	{
		default:
			t = "off";
			break;
		case 	ADVANCED_MOD_SOURCE_PAGE_VIRTUAL:
		//case 	ADVANCED_MOD_SOURCE_PAGE_VIRTUAL_PRE_GAIN:
			if (param >= ADVANCED_MOD_SOURCE_PARAM_VIRTUAL && param <= ADVANCED_MOD_SOURCE_PARAM_VIRTUAL_END)
			{
				memcpy(name , "#", 1);
				addNumeric(&name[1], param + 1);
				t = name;
			}
			else
			{
				t = "off";
			}

			break;
		case ADVANCED_MOD_SOURCE_PAGE_GLOBAL:
			switch (param)
			{
				default:
					if (param >= ADVANCED_MOD_SOURCE_PARAM_GLOBAL_LFO_OUTPUT && param <= ADVANCED_MOD_SOURCE_PARAM_GLOBAL_LFO_OUTPUT_END)
					{
						memcpy(name , "lfo", 3);
						addNumeric(&name[3], 1 + param - ADVANCED_MOD_SOURCE_PARAM_GLOBAL_LFO_OUTPUT);
						t = name;
					}else if (param >= ADVANCED_MOD_SOURCE_PARAM_GLOBAL_EG_OUTPUT && param <= ADVANCED_MOD_SOURCE_PARAM_GLOBAL_EG_OUTPUT)
					{
						memcpy(name , "eg", 2);
						addNumeric(&name[2], 1 + param - ADVANCED_MOD_SOURCE_PARAM_GLOBAL_EG_OUTPUT);
						t = name;
					}
					else
					{
						t = "off";
					}
					break;

				case ADVANCED_MOD_SOURCE_PARAM_GLOBAL_VELOCITY:
					t = "velocity";
					break;
				case ADVANCED_MOD_SOURCE_PARAM_GLOBAL_PITCH:
					t = "note";
					break;
				case ADVANCED_MOD_SOURCE_PARAM_GLOBAL_M1:
					t = "m1";
					break;
				case ADVANCED_MOD_SOURCE_PARAM_GLOBAL_M2:
					t = "m2";
					break;
				case ADVANCED_MOD_SOURCE_PARAM_GLOBAL_M3:
					t = "m3";
					break;
				case ADVANCED_MOD_SOURCE_PARAM_GLOBAL_M4:
					t = "m4";
					break;
				case ADVANCED_MOD_SOURCE_PARAM_GLOBAL_CV1:
					t = "cv1";
					break;
				case ADVANCED_MOD_SOURCE_PARAM_GLOBAL_CV2:
					t = "cv2";
					break;
				case ADVANCED_MOD_SOURCE_PARAM_GLOBAL_CV3:
					t = "cv3";
					break;
				case ADVANCED_MOD_SOURCE_PARAM_GLOBAL_CV4:
					t = "cv4";
					break;
			/*	case ADVANCED_MOD_SOURCE_PARAM_GLOBAL_LFO_OUTPUT ... ADVANCED_MOD_SOURCE_PARAM_GLOBAL_LFO_OUTPUT_END:
					memcpy(name , "lfo", 3);
					addNumeric(&name[3], 1 + param - ADVANCED_MOD_SOURCE_PARAM_GLOBAL_LFO_OUTPUT);
					t = name;
					break;
				case ADVANCED_MOD_SOURCE_PARAM_GLOBAL_EG_OUTPUT ... ADVANCED_MOD_SOURCE_PARAM_GLOBAL_EG_OUTPUT_END :
					memcpy(name , "eg", 2);
					addNumeric(&name[2], 1 + param - ADVANCED_MOD_SOURCE_PARAM_GLOBAL_EG_OUTPUT);
					t = name;
					break;*/
			}
			break;
		case ADVANCED_MOD_SOURCE_PAGE_PAGE_OFFSET:
		case ADVANCED_MOD_SOURCE_PAGE_PAGE_KNOB:
		case ADVANCED_MOD_SOURCE_PAGE_PAGE_LFO:
		case ADVANCED_MOD_SOURCE_PAGE_PAGE_EG:

			if (param >= ADVANCED_MOD_SOURCE_PARAM_PAGE_FIRST && param <= ADVANCED_MOD_SOURCE_PARAM_PAGE_LAST)
			{
				memcpy(&name[0], pageNames[param - ADVANCED_MOD_SOURCE_PARAM_PAGE_FIRST], MAX_ADVANCED_MOD_NAME_LENGTH);
				t = name;
			}
			else
			{
				t = "off";
			}
			break;
			/*
			switch (param)
			{
				default:
					t = "off";
					break;
				case ADVANCED_MOD_SOURCE_PARAM_PAGE_FIRST ... ADVANCED_MOD_SOURCE_PARAM_PAGE_LAST:
					memcpy(&name[0], pageNames[param - ADVANCED_MOD_SOURCE_PARAM_PAGE_FIRST], MAX_ADVANCED_MOD_NAME_LENGTH);
					t = name;
					break;
			}
			break;*/
		case ADVANCED_MOD_SOURCE_PAGE_MISC:
			switch (param)
			{
				default:
					if (param >= ADVANCED_MOD_SOURCE_PARAM_MISC_PITCH_NOTE_OPTION_LATEST_NOTE && param <= ADVANCED_MOD_SOURCE_PARAM_MISC_PITCH_NOTE_OPTION_LOWEST_NOTE)
					{
						t = noteText[param - ADVANCED_MOD_SOURCE_PARAM_MISC_PITCH_NOTE_OPTION_LATEST_NOTE];
					}else if (param >= ADVANCED_MOD_SOURCE_PARAM_MISC_FIXED_VALUE_1 && param <= ADVANCED_MOD_SOURCE_PARAM_MISC_FIXED_VALUE_128)
					{
						memcpy(name, "const ", 6);
						memcpy(&name[6], fixedValuesText[param - ADVANCED_MOD_SOURCE_PARAM_MISC_FIXED_VALUE_1], 4);
						t = name;
					}
					else
					{
						t = "off";
					}
					break;

				case ADVANCED_MOD_SOURCE_PARAM_MISC_TEMPO_CLOCK:
					t = "tempo clk";
					break;
				case ADVANCED_MOD_SOURCE_PARAM_MISC_GLOBAL_NOTES_PRESSED:
					t = "notes down";
					break;
			/*	case ADVANCED_MOD_SOURCE_PARAM_MISC_PITCH_NOTE_OPTION_LATEST_NOTE ... ADVANCED_MOD_SOURCE_PARAM_MISC_PITCH_NOTE_OPTION_LOWEST_NOTE:
					t = noteText[param - ADVANCED_MOD_SOURCE_PARAM_MISC_PITCH_NOTE_OPTION_LATEST_NOTE];
					break;
				case ADVANCED_MOD_SOURCE_PARAM_MISC_FIXED_VALUE_1 ... ADVANCED_MOD_SOURCE_PARAM_MISC_FIXED_VALUE_128:
					memcpy(name, "const ", 6);
					memcpy(&name[6], fixedValuesText[param - ADVANCED_MOD_SOURCE_PARAM_MISC_FIXED_VALUE_1], 4);
					t = name;
					break;*/
		/*		case ADVANCED_MOD_SOURCE_PARAM_MISC_FIXED_NUMBER_2 ... ADVANCED_MOD_SOURCE_PARAM_MISC_FIXED_NUMBER_16:
					memcpy(name, "value ", 6);
					memcpy(&name[6], fixedValuesText[param - ADVANCED_MOD_SOURCE_PARAM_MISC_FIXED_NUMBER_2], 4);
					t = name;
					break;*/
			}
			break;
	}
	return String(t);

}



static const String getAlgorithmText(int index)
{
	const char* t;


	switch (index)
	{
		default:
		case MOD_ALGORITHM_ZERO:
			t = "off";
			break;
		case MOD_ALGORITHM_SRC1:
			t = "s1 only";
			break;
		case MOD_ALGORITHM_SRC1_NEGATED:
			t = "-s1 only";
			break;
		case MOD_ALGORITHM_SRC2:
			t = "s2 only";
			break;
		case MOD_ALGORITHM_SUM:
			t = "add";
			break;
		case MOD_ALGORITHM_MEAN:
			t = "average";
			break;
		case MOD_ALGORITHM_DIFF:
			t = "diff";
			break;

		case MOD_ALGORITHM_SUBTRACT:
			t = "subtract";
			break;
		case MOD_ALGORITHM_MIN:
			t = "min";
			break;
		case MOD_ALGORITHM_MAX:
			t = "max";
			break;
		case MOD_ALGORITHM_MULT:
			t = "multiply";
			break;
		case MOD_ALGORITHM_DIVIDE:
			t = "divide";
			break;
		case MOD_ALGORITHM_QUANTISE:
			t = "quantise";
			break;
		case MOD_ALGORITHM_SLEW:
			t = "slew";
			break;
		case MOD_ALGORITHM_SAMPLE_HOLD:
			t = "s/hold";
			break;
		case MOD_ALGORITHM_TIME_DIVIDE:
			t = "f-divide";
			break;
			// 'boolean' functions
		case MOD_ALGORITHM_LESS_THAN:
			t = "less";
			break;
		case MOD_ALGORITHM_GREATER_THAN:
			t = "greater";
			break;
		case MOD_ALGORITHM_BOOLEAN_AND:
			t = " and";
			break;
		case MOD_ALGORITHM_BOOLEAN_OR:
			t = " or";
			break;
		case MOD_ALGORITHM_BOOLEAN_XOR:
			t = " xor";
			break;
	//	case MOD_ALGORITHM_OFFSET:
	//		t = "off";
	//		break;
	}


	return String(t);
}



static const String getSourcePageText(int index)
{
	const char* t;

	switch(index)
	{
		default:
			t = "off";
			break;
		case 	ADVANCED_MOD_SOURCE_PAGE_VIRTUAL:
			t = "slot";
			break;
	//	case 	ADVANCED_MOD_SOURCE_PAGE_VIRTUAL_PRE_GAIN:
	//		t = "pre-gain slot";
	//		break;
		case ADVANCED_MOD_SOURCE_PAGE_GLOBAL:
			t = "global";
			break;
			break;
		case ADVANCED_MOD_SOURCE_PAGE_PAGE_OFFSET:
			t = "page";
			break;
		case ADVANCED_MOD_SOURCE_PAGE_PAGE_KNOB:
			t = "knob";
			break;
		case ADVANCED_MOD_SOURCE_PAGE_PAGE_LFO:
			t = "lfo";
			break;
		case ADVANCED_MOD_SOURCE_PAGE_PAGE_EG:
			t = "eg";
			break;
		case ADVANCED_MOD_SOURCE_PAGE_MISC:
			t = "misc";
			break;
	}


	return String(t);
}


AdvancedModSetup::AdvancedModSetup(HoldingWindowComponent* parent, LookAndFeel* l) //: midiKeyboard (keyboardState, MidiKeyboardComponent::horizontalKeyboard)
: holdingWindow(parent)
{

	DBG("Create advanced mod");
	aModOnOffButton.setButtonText("OFF");
	addAndMakeVisible(aModOnOffButton);
	aModOnOffButton.addListener(this);
    aModOnOffButton.setTooltip("Toggle advanced modulation on/off");
	aModOnOffButton.setLookAndFeel(l);

	aModEnabled = false;

	for (int i = 0; i < NUMBER_OF_MODULATION_OF_MODULATIONS; i++)
	{

		String labelText = String("# ") + String(i + 1);

		Label* label = new Label(labelText, labelText);
		label->setTooltip("Slot number for advanced modulation");
		addAndMakeVisible(label);
		slotArray.add(label);


		ComboBox* c = mappingSource1Page.add(new ComboBox());
		//lastIndexChosenArray.add(0);
		for (uint8_t j = 0; j < ADVANCED_MOD_SOURCE_PAGE_END ; j++)
		{
			c->addItem(getSourcePageText(j).toUpperCase(), 1 + j);
		}
		c->setSelectedItemIndex(0, NotificationType::dontSendNotification);
		c->setTooltip("source 1 for slot " + String(i + 1));
		c->addListener(this);
		addAndMakeVisible(c);



		c = mappingSource1Param.add(new ComboBox());
		for (uint8_t j = 0; j < NUMBER_OF_MODULATION_OF_MODULATIONS; j++)
		{
			//c->addItem(getSourceParamText(0, j).toUpperCase(), 1 + j);
			c->addItem(getSourceParamText(0, j).toUpperCase(), 1 + j);
		}
		c->setSelectedItemIndex(0, NotificationType::dontSendNotification);
		c->setTooltip("source 1 for slot " + String(i + 1));
		c->addListener(this);
		addAndMakeVisible(c);



		c = mappingSource2Page.add(new ComboBox());
		for (uint8_t j = 0; j < ADVANCED_MOD_SOURCE_PAGE_END; j++)
		{
			c->addItem(getSourcePageText(j).toUpperCase(), 1 + j);
		}
		c->setSelectedItemIndex(0, NotificationType::dontSendNotification);
		c->setTooltip("source 2 for slot " + String(i + 1));
		c->addListener(this);
		addAndMakeVisible(c);

		c = mappingSource2Param.add(new ComboBox());
		for (uint8_t j = 0; j < NUMBER_OF_MODULATION_OF_MODULATIONS; j++)
		{
			c->addItem(getSourceParamText(0, j).toUpperCase(), 1 + j);
		}
		c->setSelectedItemIndex(0, NotificationType::dontSendNotification);
		c->setTooltip("source 2 for slot " + String(i + 1));
		c->addListener(this);
		addAndMakeVisible(c);


		c = mappingAlgorithm.add(new ComboBox());
		for (uint8_t j = 0; j < MOD_ALGORITHM_ENTRIES_MAX; j++)
		{
			c->addItem(getAlgorithmText(j).toUpperCase(), 1 + j);
		}
		c->setSelectedItemIndex(0, NotificationType::dontSendNotification);
		c->setTooltip("function for slot " + String(i + 1));
		c->addListener(this);
		addAndMakeVisible(c);


		c = mappingDestPage.add(new ComboBox());
		for (uint8_t j = 0; j < pageSettings.size(); j++)
		{
			c->addItem(pageSettings[j].toUpperCase(), 1 + j);
		}
		c->setSelectedItemIndex(0, NotificationType::dontSendNotification);
		c->setTooltip("destination for slot " + String(i + 1));
		c->addListener(this);
		addAndMakeVisible(c);

		c = mappingDestParam.add(new ComboBox());
		for (uint8_t j = 0; j < globalDestination.size(); j++)
		{
			c->addItem(globalDestination[j].toUpperCase(), 1 + j);
		}
		c->setSelectedItemIndex(0, NotificationType::dontSendNotification);
		c->setTooltip("destination for slot " + String(i + 1));
		c->addListener(this);
		addAndMakeVisible(c);



		c = mappingUnipolar.add(new ComboBox());
		c->addItem("bipolar", 1);
		c->addItem("unipolar", 2);
		c->setSelectedItemIndex(0, NotificationType::dontSendNotification);
		c->setTooltip("bipolar/unipolar ");
		c->addListener(this);
		addAndMakeVisible(c);


		Slider* gain = mappingGain.add(new Slider(Slider::LinearBarVertical, Slider::TextBoxBelow));
	//	CustomSlider* gain = mappingGain.add(new CustomSlider(0, 0));
		gain->addListener(this);
		gain->setPopupMenuEnabled(true);
		gain->setRange(-MOD_DEST_LIMIT_MAX, MOD_DEST_LIMIT_MAX-1, 1);
		gain->setTooltip("gain");
		addAndMakeVisible(gain);
		gain->setValue(0, NotificationType::dontSendNotification);



	}
	addAndMakeVisible(advancedModLabel);
	addAndMakeVisible(slotLabel);
	addAndMakeVisible(source1Label);
	addAndMakeVisible(source1ParamLabel);
	addAndMakeVisible(source2Label);
	addAndMakeVisible(source2ParamLabel);
	addAndMakeVisible(algorithmLabel);
	addAndMakeVisible(destLabel);
	//addAndMakeVisible(destParamLabel);
	//addAndMakeVisible(bipolarLabel);
	addAndMakeVisible(gainLabel);


	initPanelSettings(); // DO THIS LAST after combo boxes set up etc
}

AdvancedModSetup::~AdvancedModSetup()
 {
	//setLookAndFeel (nullptr);

	DBG("AdvancedModSetup destructor");



 }


void AdvancedModSetup::changeListenerCallback(ChangeBroadcaster* )
{

}

void AdvancedModSetup::sendNRPNChange(uint8_t index, uint8_t type, uint8_t msb, uint8_t lsb)
{
	if (index < NUMBER_OF_MODULATION_OF_MODULATIONS && type < AMOD_NRPN_TYPE_END)
	{
		NRPNmessage_t n;
		n.parameterMSB = NRPN_MSB_GLOBAL_ADVANCED_MODULATION;
		n.parameterLSB = (index << 3) | type;

		n.valueMSB = msb & 0x7F;
		n.valueLSB = lsb & 0x7F;

		holdingWindow->midiProcessor->commandRequest(MIDIProcessor::COMMAND_REQUEST_PARAMETER_CHANGED, n);
	}
}

void AdvancedModSetup::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
	DBG("Amod combo box changed");

	uint8_t selectionIndex = (uint8_t) comboBoxThatHasChanged->getSelectedItemIndex();
	tAllParams* p = holdingWindow->mainComponent->getPatch();

	int index =  mappingSource1Page.indexOf(comboBoxThatHasChanged);
	if (index >= 0 && index < NUMBER_OF_MODULATION_OF_MODULATIONS)
	{

		p->params.singleParams[index].more.lfo2Mod_And_ModofMod.modOfmodulation.source1Page = selectionIndex;
		// re-populate the source 1 param
		updateComboMappingGeneric(AMOD_NRPN_TYPE_SOURCE1, (uint8_t) index, true);
	}
	index =  mappingSource1Param.indexOf(comboBoxThatHasChanged);
	if (index >= 0 && index < NUMBER_OF_MODULATION_OF_MODULATIONS)
	{

		p->params.singleParams[index].more.lfo2Mod_And_ModofMod.modOfmodulation.source1Param = selectionIndex;
		sendNRPNChange((uint8_t) index, AMOD_NRPN_TYPE_SOURCE1, (uint8_t) mappingSource1Page[index]->getSelectedItemIndex(), selectionIndex);
	}


	index =  mappingSource2Page.indexOf(comboBoxThatHasChanged);
	if (index >= 0 && index < NUMBER_OF_MODULATION_OF_MODULATIONS)
	{

		p->params.singleParams[index].more.lfo2Mod_And_ModofMod.modOfmodulation.source2Page = selectionIndex;
		updateComboMappingGeneric(AMOD_NRPN_TYPE_SOURCE2, (uint8_t) index, true);
	}
	index =  mappingSource2Param.indexOf(comboBoxThatHasChanged);
	if (index >= 0 && index < NUMBER_OF_MODULATION_OF_MODULATIONS)
	{

		p->params.singleParams[index].more.lfo2Mod_And_ModofMod.modOfmodulation.source2Param = selectionIndex;
		sendNRPNChange((uint8_t)index, AMOD_NRPN_TYPE_SOURCE2, (uint8_t) mappingSource2Page[index]->getSelectedItemIndex(),selectionIndex);
	}

	index =  mappingDestPage.indexOf(comboBoxThatHasChanged);
	if (index >= 0 && index < NUMBER_OF_MODULATION_OF_MODULATIONS)
	{

		p->params.singleParams[index].more.lfo2Mod_And_ModofMod.modOfmodulation.destinationPage = selectionIndex;
		updateComboMappingGeneric(AMOD_NRPN_TYPE_DEST, index, true);
	}

	index =  mappingDestParam.indexOf(comboBoxThatHasChanged);
	if (index >= 0 && index < NUMBER_OF_MODULATION_OF_MODULATIONS)
	{

		p->params.singleParams[index].more.lfo2Mod_And_ModofMod.modOfmodulation.destinationParam = selectionIndex;
		sendNRPNChange((uint8_t)index, AMOD_NRPN_TYPE_DEST, (uint8_t) mappingDestPage[index]->getSelectedItemIndex(),selectionIndex);
	}

	index = mappingAlgorithm.indexOf(comboBoxThatHasChanged);
	if (index >= 0 && index < NUMBER_OF_MODULATION_OF_MODULATIONS)
	{

		p->params.singleParams[index].more.lfo2Mod_And_ModofMod.modOfmodulation.algorithm = selectionIndex;
		sendNRPNChange((uint8_t)index, AMOD_NRPN_TYPE_ALGORITHM, 0, selectionIndex);
	}

	index =  mappingUnipolar.indexOf(comboBoxThatHasChanged);
	if (index >= 0 && index < NUMBER_OF_MODULATION_OF_MODULATIONS)
	{

		p->params.singleParams[index].more.lfo2Mod_And_ModofMod.modOfmodulation.unipolar = selectionIndex;
		sendNRPNChange((uint8_t)index, AMOD_NRPN_TYPE_UNIPOLAR, 0, selectionIndex);
	}

}




//==============================================================================
void AdvancedModSetup::addLabelAndSetStyle (Label& label)
{
    label.setFont (Font (15.00f, Font::plain));
    label.setJustificationType (Justification::centredLeft);
    label.setEditable (false, false, false);
    label.setColour (TextEditor::textColourId, Colours::black);
    label.setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (label);
}


void AdvancedModSetup::resized()
{
	localBoundsRect = getLocalBounds().reduced(10);


	int w = localBoundsRect.getWidth();
    boundsHeight = (float) localBoundsRect.getHeight();


    fieldHeight = boundsHeight / (NUMBER_OF_MODULATION_OF_MODULATIONS  + 2);

    if (fieldHeight > 40)
    {
    	fieldHeight = 30;
    }
	auto r2 = localBoundsRect.removeFromTop((int) fieldHeight);
	//

    int buttonWidth = (int) (w *0.138f);



	 widthMatrix = (int) ( w / 4);

	 advancedModLabel.setBounds (r2.removeFromLeft (widthMatrix /2));
	 aModOnOffButton.setBounds (r2.removeFromLeft ((int)buttonWidth/2));


	 r2 = localBoundsRect.removeFromTop((int)(fieldHeight));
	 slotLabel.setBounds (r2.removeFromLeft (widthMatrix /6));
	 source1Label.setBounds (r2.removeFromLeft (widthMatrix /3));
	 source1ParamLabel.setBounds (r2.removeFromLeft (widthMatrix /2));
	 source2Label.setBounds (r2.removeFromLeft (widthMatrix /3));
	 source2ParamLabel.setBounds (r2.removeFromLeft (widthMatrix /2));
	 algorithmLabel.setBounds (r2.removeFromLeft (widthMatrix /3));

	 gainLabel.setBounds (r2.removeFromLeft (widthMatrix /4));
	 r2.removeFromLeft (widthMatrix /4);
	 destLabel.setBounds (r2.removeFromLeft (widthMatrix /2));
	 //destParamLabel.setBounds (r2.removeFromLeft (widthMatrix /2));


	for (int i = 0; i < NUMBER_OF_MODULATION_OF_MODULATIONS; i++)
	{
		r2 = localBoundsRect.removeFromTop((int)(fieldHeight));

		slotArray[i]->setBounds (r2.removeFromLeft (widthMatrix /6));


		mappingSource1Page[i]->setBounds (r2.removeFromLeft (widthMatrix /3));
		mappingSource1Param[i]->setBounds(r2.removeFromLeft (widthMatrix/2));
		mappingSource2Page[i]->setBounds (r2.removeFromLeft (widthMatrix /3));
		mappingSource2Param[i]->setBounds(r2.removeFromLeft (widthMatrix/2));
		mappingAlgorithm[i]->setBounds(r2.removeFromLeft (widthMatrix/3));


		mappingGain[i]->setBounds(r2.removeFromLeft (widthMatrix/4));

		mappingUnipolar[i]->setBounds(r2.removeFromLeft (widthMatrix/4));


		mappingDestPage[i]->setBounds(r2.removeFromLeft (widthMatrix/2));
		mappingDestParam[i]->setBounds(r2.removeFromLeft (widthMatrix/2));

	}


}



void AdvancedModSetup::buttonClicked(Button* b)
{
	if (b == &aModOnOffButton)
	{

		aModEnabled = !aModEnabled;

		if (aModEnabled)
		{
			b->setButtonText("ON");
			b->setToggleState(true, NotificationType::dontSendNotification);
		}
		else
		{
			b->setButtonText("OFF");
			b->setToggleState(false, NotificationType::dontSendNotification);
		}


		tAllParams* p = holdingWindow->mainComponent->getPatch();
		p->advancedModulationMode = aModEnabled;


		sendNRPNChange(0, AMOD_NRPN_TYPE_MODE, 0, aModEnabled & 0x01);
	}

}



void AdvancedModSetup::initPanelSettings()
{


#if BUILD_STANDALONE_EXECUTABLE == 0
	guiSizeCombo.setSelectedId(holdingWindow->appCommon->
			loadAppPropertyInteger(ApplicationCommon::APP_PROPERTY_PLUGIN_GUI_SIZE),
			NotificationType::dontSendNotification);
#endif


}

void AdvancedModSetup::updateAdvancedModSettings(int index, const t_modOfMod* modSettings, bool aModMode)
{


	if (modSettings == nullptr)
		return;
	// set the GUI settings to match the provided settings.
	DBG("updateAdvancedModSettings ");
	if (index >= 0 && index < NUMBER_OF_MODULATION_OF_MODULATIONS)
	{

		mappingSource1Page[index]->setSelectedItemIndex(modSettings->source1Page , NotificationType::dontSendNotification);
		updateComboMappingGeneric(AMOD_NRPN_TYPE_SOURCE1, index,  false);
		mappingSource1Param[index]->setSelectedItemIndex(modSettings->source1Param , NotificationType::dontSendNotification);

		mappingSource2Page[index]->setSelectedItemIndex(modSettings->source2Page , NotificationType::dontSendNotification);

		updateComboMappingGeneric(AMOD_NRPN_TYPE_SOURCE2, index,  false);

		mappingSource2Param[index]->setSelectedItemIndex(modSettings->source2Param , NotificationType::dontSendNotification);

		mappingGain[index]->setValue(modSettings->gain, NotificationType::dontSendNotification);
		mappingAlgorithm[index]->setSelectedItemIndex(modSettings->algorithm , NotificationType::dontSendNotification);
		mappingUnipolar[index]->setSelectedItemIndex(modSettings->unipolar , NotificationType::dontSendNotification);

		mappingDestPage[index]->setSelectedItemIndex(modSettings->destinationPage , NotificationType::dontSendNotification);


		updateComboMappingGeneric(AMOD_NRPN_TYPE_DEST, index,  false);

		mappingDestParam[index]->setSelectedItemIndex(modSettings->destinationParam , NotificationType::dontSendNotification);

		if (aModMode != aModEnabled)
		{
			buttonClicked(&this->aModOnOffButton);
		}

	}

	// now update the GUI to reflect the choices (for page -> param)

}



void AdvancedModSetup::paint(Graphics&)
{


}
void  AdvancedModSetup::updateComboMappingGeneric(int type, int index, bool updatePatch)
{
	DBG("updateComboMappingGeneric ");

	// update the secondary source/dest param displayed according to the selected page source/dest
	if (index >= 0 && index < NUMBER_OF_MODULATION_OF_MODULATIONS)
	{
	//	DBG("index ");
		ComboBox* a;
		ComboBox* b;

		switch (type)
		{
			default:
			case AMOD_NRPN_TYPE_SOURCE1:
				a = mappingSource1Page[index];
				b = mappingSource1Param[index];
				break;
			case AMOD_NRPN_TYPE_SOURCE2:
				a = mappingSource2Page[index];
				b = mappingSource2Param[index];
				break;
			case AMOD_NRPN_TYPE_DEST:
				a = mappingDestPage[index];
				b = mappingDestParam[index];
				break;

		}
		uint8_t mainSelection = (uint8_t) a->getSelectedItemIndex();
		b->clear(NotificationType::dontSendNotification);

		switch (type)
		{
			default:
			case AMOD_NRPN_TYPE_SOURCE1:
			case AMOD_NRPN_TYPE_SOURCE2:
				for (uint8_t j = 0; j < ADVANCED_MOD_SOURCE_PARAM_PAGE_LAST + 1; j++) // loop beyond required range for most pages
				{
					String text = getSourceParamText(mainSelection, j);

			//		DBG("updateComboMappingGeneric " + text);
					if (!text.equalsIgnoreCase("off"))
						b->addItem(text.toUpperCase(), 1 + j);
				}
				break;
			case AMOD_NRPN_TYPE_DEST:
				switch (mainSelection)
				{
					case 0:
						b->addItem("NONE", 1);
						break;
					case 1:
						for (int i = 0; i < globalDestination.size(); i++)
						{
							b->addItem(globalDestination[i].toUpperCase(), 1 + i);
						}
						break;
					default:
						for (int i = 0; i < pageDestinations.size(); i++)
						{
							b->addItem(pageDestinations[i].toUpperCase(), 1 + i);
						}
						break;
					case 35:
						for (int i = 0; i < patchSetupDestination.size(); i++)
						{
							b->addItem(patchSetupDestination[i].toUpperCase(), 1 + i);
						}
						break;
				}

				break;
		}

		if (updatePatch && type < 3 && holdingWindow->mainComponent)
		{
			tAllParams* p = holdingWindow->mainComponent->getPatch();
			switch (type)
			{
			default:
				break;
			case AMOD_NRPN_TYPE_SOURCE1:
				p->params.singleParams[index].more.lfo2Mod_And_ModofMod.modOfmodulation.source1Param = 0;
				break;
			case AMOD_NRPN_TYPE_SOURCE2:
				p->params.singleParams[index].more.lfo2Mod_And_ModofMod.modOfmodulation.source2Param = 0;
				break;
			case AMOD_NRPN_TYPE_DEST:
				p->params.singleParams[index].more.lfo2Mod_And_ModofMod.modOfmodulation.destinationParam = 0;
				break;
			}
			sendNRPNChange((uint8_t) index, (uint8_t) type, mainSelection,  0);
		}
		b->setSelectedItemIndex(0, NotificationType::dontSendNotification);
	}
}



void AdvancedModSetup::sliderValueChanged(Slider* slider)
{

	int index =  mappingGain.indexOf((Slider*) slider);
	if (index >= 0)
	{
		uint16_t value = (uint16_t) slider->getValue() + 8192; // convert to transmit 14 bits.
		uint8_t msb = (value >> 7) & 0x7F;
		uint8_t lsb = value & 0x7F;

		sendNRPNChange((uint8_t)index, AMOD_NRPN_TYPE_GAIN, msb, lsb);
	}

}
