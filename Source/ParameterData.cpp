
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
 * ParameterData.cpp

 */


#include "./Headers/ParameterData.h"

#include "./Headers/PanelControl.h"

ParameterData::ParameterData(int subId)
: button(nullptr), comboBox(nullptr), slider(nullptr),
  subId(subId), panelControlId(PanelControl::PANEL_CONTROL_NONE), panelValue(0),
  isPitch(false), max(4095), min(0), msb(0), lsb(0), parameterPage(0),
  parameterType(0), isParameterPageFixed(false)

{

	defaultValue = 0;
	incrementStep = 0;

}

static const StringArray HPF_option1Text = {"MIX", "POLE-1","POLE-2", "POLE-4", "POLE-5", "POLE-6", "NOISE", "LPF2"};
static const StringArray HardSyncOSC1_option1Text = {"S:OFF", "S:2", "S:3", "S:2+3"};
static const StringArray HardSyncOSC2_option1Text = {"S:OFF", "S:1", "S:3", "S:1+3"};
static const StringArray OSC_option1Text = {"LST", "1", "2", "3", "HI", "MID", "LOW"};
static const StringArray OSC_option23Text = {"LST", "1", "2", "3", "HI", "MID", "LOW", "INF", "ULT"};
static const StringArray OSC3PM_option1Text = {"OSC1", "OSC2"};
static const StringArray Noise_option1Text = {"WHITE", "PINK"};
static const StringArray MIX_option1Text = {"NORMAL", "BOOST"};
static const StringArray LPF1Input_option1Text = {"MIX", "LPF2", "HPF", "NOISE", "PULS1", "SAW2", "TRI3", "SUB2"};
static const StringArray LPF1Res_option1Text = {"POLE-4", "POLE-6", "HPF"};
static const StringArray LPF1Output_option1Text = {"BYPASS", "POLE-1", "POLE-2", "POLE-3", "POLE-4", "POLE-5", "POLE-6"};
static const StringArray HPFInput_option1Text = {"MIX", "POLE-1", "POLE-2", "POLE-4", "POLE-5", "POLE-6", "NOISE", "LPF2"};
static const StringArray LPF2Input_option1Text = {"MIX", "POLE-1", "POLE-2", "POLE-4", "POLE-5", "POLE-6", "NOISE", "HPF"};
static const StringArray VCAOut_option1Text = {"DRY", "SOFT CLIP", "DRY+SOFT CLIP", "HARD CLIP", "DRY+HARD CLIP", "SOFT+HARD CLIP", "DRY+SOFT+HARD"};


static const StringArray LFOSync_option1Text = {"off",	"1/32.",  "1/32", "1/32t",
			"1/16.","1/16", "1/16t", "1/8.", "1/8", "1/8t",
			"1/4.",	"1/4",	"1/4t",	"1/2.",	"1/2", 	"1/2t","1.",
			"1", "1t","2.",	"2", 		"2t",
			"4.","4","4t","8.",	"8", 	"8t",
			"16.","16", "16t","32.","32", 		"32t"
};


static const StringArray LFOs_optionText = {"LFO1", "LFO2", "LFO3", "LFO4"};
static const StringArray LFOTrackPitch_optionText = {"free", "note track", "pitch track", "clk sync"};
static const StringArray LFOReset_optionText = {"OFF", "FIRST", "ALL"};
static const StringArray LFOMode_optionText = {"OFF", "xEG", "xLFOx"};
static const StringArray EGTrigger_optionText = {"NOTE ", "1ST NOTE", "2ND NOTE", "3RD NOTE", "EVERY", "HIGHEST", "MIDDLE", "LOWEST",
		"TRIG-1", "TRIG-2", "TRIG-3", "TRIG-4", "TRIG-5", "TRIG-6", "TRIG-7", "TRIG-8"};
static const StringArray EGChoice_optionText = {"EG", "EG1", "EG2", "EG3", "EG4"};

static const StringArray LFOWaveform_optionText = {
		"SIN",
		"SIN-90",
		"TRI",
		"TRI-90",
		"SQUARE",
		"PULSE1",
		"PULSE2",
		"PULSE2",
		"RAMP",
		"RAMP90",
		"RMPHLD",
		"3-STEP",
		"4-STEP",
		"S+H",
		"NOISE",
		"RANTRI",
		"PLSE1R",
		"PLSE2R",
		"PLSE3R",
		"RMPH-R",
		"BOUNCE",
		"BOING",
		"RETARD",
		"SINDEC",
		"SININV",
		"BURST",
		"BURST2",
		"SIN+2",
		"SIN+3",
		"SINTRI",
		"TRI/2",
		"BEAT",
		"BEAT2",
		"CHROM",
		"MAJOR",
		"MINOR",
		"AUG",
		"DIM",
		"DIM7",
		"2DIM7",
		"MIN7",
		"MINMAJ",
		"DOM7",
		"MAJ7",
		"AUG7",
		"AMAJ7",
		"SEQ1",
		"SEQ2",
		"SEQ3",
		"SEQ4",
		"S+H SL",
		"CUST1",
		"CUST2",
		"CUST3",
		"CUST4",
};

static const StringArray ModDest_optionText = {"1 LFOX LEVEL","2 LFO FREQ","3 LFO LEVEL",
		"4 EG TIME","5 EG ATTACK","6 EG DECAY","7 EG SUSTAIN","8 EG RELEASE","9 EG LEVEL"};

static const StringArray EGRestart_optionText = {"GATED", "GATED-LOOP", "TRIGGER"};


void ParameterData::populateComboBox(ComboBox* c, int type)
{
	const StringArray* s = nullptr;
	switch (type)
	{
	case 0:
		s = &OSC_option23Text;
		break;
	case 100:
		s = &OSC_option1Text;
		break;
	case 1:
		s = &HardSyncOSC1_option1Text;
		break;
	case 2:
		s = &HardSyncOSC2_option1Text;
		break;
	case 3:
		s = &Noise_option1Text;
		break;
	case 4:
		s = &MIX_option1Text;
		break;

	case 5:
		s = &LPF1Input_option1Text;
		break;
	case 6:
		s = &LPF1Res_option1Text;
		break;
	case 7:
		s = &LPF1Output_option1Text;
		break;
	case 8:
		s = &HPFInput_option1Text;
		break;
	case 9:
		s = &LPF2Input_option1Text;
		break;
	case 10:
		s = &VCAOut_option1Text;
		break;

	case 20:
		s = &LFOSync_option1Text;
		break;

	case 21:
		s = &LFOs_optionText;
		break;
	case 22:
		s = &LFOTrackPitch_optionText;
		break;
	case 23:
		s = &LFOReset_optionText;
		break;
	case 24:
		s = &LFOMode_optionText;
		break;
	case 25:
		s = &EGTrigger_optionText;
		break;
	case 26:
		s = &EGChoice_optionText;
		break;
	case 30:
		s = &ModDest_optionText;
		break;
	case 31:
		s = &LFOWaveform_optionText;
		break;
	case 32:
		s = &EGRestart_optionText;
		break;
	default:
		break;
	}
	if (s != nullptr)
	{
		for (int i = 0; i < s->size(); i++)
		{
			c->addItem((*s)[i], i+1);
		}
	}
}


/**
 * Compute the NRPN message to send based on the parameter data coming in
 */
NRPNmessage_t ParameterData::convertParameterDataToNRPN()
{

	NRPNmessage_t n;
	n.parameterMSB = (uint8_t) this->getPage();
	n.parameterLSB = (uint8_t) this->getParameter();
	n.valueMSB =  this->getMSB();
	n.valueLSB = this->getLSB();
	return n;

}


uint8_t ParameterData::getMSB() const
{

	return this->msb;
}

uint8_t ParameterData::getLSB() const
{
	return this->lsb;
}

void ParameterData::initialise(int panelCId,  int page, int param, String controlName)
{

	this->name = controlName;
	this->panelControlId = panelCId;
	this->parameterPage = page;
	this->parameterType = param;

	if (parameterType == PanelControl::CONTROL_PARAM_OFFSET) // if is for controlling offset directly
	{
		if (parameterPage == PanelControl::CONTROL_OSC1_PITCH
				|| parameterPage == PanelControl::CONTROL_OSC2_PITCH
				|| parameterPage == PanelControl::CONTROL_OSC3_PITCH)
		{
			// if page is for pitch don't offset
			this->max = 12000;
			this->isPitch = true;
			defaultValue = 6000;
		}
		else if (parameterPage == PanelControl::CONTROL_MASTER_PITCH) // master pitch
		{
			this->min = -4095;
		}
	}
	if (subId == PanelControl::PARAM_TYPE_MAIN_COMBO)
	{
	//	if (panelControlId == PanelControl::PANEL_CONTROL_LFOX_FREQ)
	//		this->min = -33;
	//	else
			this->min = 0;
	}
	switch (parameterType)
	{
		default:
			break;
		case PanelControl::CONTROL_PARAM_EG_MOD_DEPTH:
		case PanelControl::CONTROL_PARAM_LFOX_MOD_DEPTH:
		case PanelControl::CONTROL_PARAM_LFO_MOD_DEPTH:
		case PanelControl::CONTROL_PARAM_M1_LEVEL:
		case PanelControl::CONTROL_PARAM_M1_ALT_LEVEL:
		case PanelControl::CONTROL_PARAM_M2_LEVEL:
		case PanelControl::CONTROL_PARAM_M2_ALT_LEVEL:
		case PanelControl::CONTROL_PARAM_M3_LEVEL:
		case PanelControl::CONTROL_PARAM_M3_ALT_LEVEL:
		case PanelControl::CONTROL_PARAM_M4_LEVEL:
		case PanelControl::CONTROL_PARAM_M4_ALT_LEVEL:
		case PanelControl::CONTROL_PARAM_NOTE_LEVEL:
		case PanelControl::CONTROL_PARAM_NOTE_ALT_LEVEL:
		case PanelControl::CONTROL_PARAM_VELOCITY_LEVEL:
		case PanelControl::CONTROL_PARAM_VELOCITY_ALT_LEVEL:

			this->min = -4095;
			break;
		case PanelControl::CONTROL_PARAM_EG_ATTACK:
		case PanelControl::CONTROL_PARAM_EG_DECAY:
		case PanelControl::CONTROL_PARAM_EG_RELEASE:
		case PanelControl::CONTROL_PARAM_EG_SUSTAIN:
			defaultValue = 4095;
			break;
		case PanelControl::CONTROL_PARAM_EG_SHAPE_MULTI:
		{
			this->max = 64;
			this->min = -64;
			defaultValue = 0;
		}
			break;
		case PanelControl::CONTROL_PARAM_LFO2_EG_FREQ_MOD:
		case PanelControl::CONTROL_PARAM_LFO2_LFOX_FREQ_MOD:
		{
			this->max = 4095;
			this->min = -4095;
			defaultValue = 0;
		}
		break;

	}

	switch (panelControlId)
	{
		default:
			//if (parameterType)
			incrementStep = 75;//(int32_t) ( 4095.0f / 12.2f);
			break;
		case PanelControl::PANEL_CONTROL_MASTER_PITCH:
			incrementStep = 75;
			break;
		case PanelControl::PANEL_CONTROL_OSC1_PITCH:
		case PanelControl::PANEL_CONTROL_OSC2_PITCH:
		case PanelControl::PANEL_CONTROL_OSC3_PITCH:
			incrementStep = 100;
			break;
		case PanelControl::PANEL_CONTROL_LPF1:
		case PanelControl::PANEL_CONTROL_LPF2:
			incrementStep = 75;// VCF_STEPS_PER_OCTAVE;
			break;

	}
	if (page > 0)
	{
		if (parameterType >= PanelControl::CONTROL_PARAM_PAGE_OPTION1)
			incrementStep = 1;
	}

}


void ParameterData::setValueFromPatch(int32_t val)
{

	//int32_t incomingVal = val;
//	if (inverted)
//		val = max - val + min;

	if (val > max)
		val = max;
	if (val < min)
		val = min;

	if (slider != nullptr)
	{
		if (panelControlId == PanelControl::PANEL_CONTROL_LFOX_FREQ)
		{
	/*		if (incomingVal < 0)
				slider->setEnabled(false);
			else
				slider->setEnabled(true);*/
		}
		slider->setValue(val, NotificationType::dontSendNotification);

	} else if (comboBox != nullptr)
	{

		uint8_t numItems = (uint8_t) comboBox->getNumItems();
		uint8_t newVal = (uint8_t) val;

		//DBG("Set Combo: " + String(val) + "min: " + String(min));

		if (panelControlId == PanelControl::PANEL_CONTROL_LFOX_FREQ)
		{
			if (subId == PanelControl::PARAM_TYPE_MAIN_COMBO)
		/*	{
				if (val < 0)
					newVal = (uint8_t) (-val);
				if (newVal >= numItems)
					newVal = numItems - 1;
				if (val < 0)
					comboBox->setSelectedId(newVal + 1, NotificationType::dontSendNotification);
				else
					comboBox->setSelectedId(1, NotificationType::dontSendNotification);
			} else*/
			{
				if (val <= 0)
					newVal = 0;
				if (newVal >= numItems)
					newVal = numItems - 1;
				comboBox->setSelectedId(newVal + 1, NotificationType::dontSendNotification);
			}
		} else
		{
			if (val <= 0)
				newVal = 0;
			if (newVal >= numItems)
				newVal = numItems - 1;
			comboBox->setSelectedId(newVal + 1, NotificationType::dontSendNotification);

		}

	} else if (button != nullptr)
	{
		if (val > 0)
		{
			//val = 1;
			button->setToggleState(true, NotificationType::dontSendNotification);
		}
		else
		{
			button->setToggleState(false, NotificationType::dontSendNotification);
			//val = 0;
		}
	}
	setValue(val);
}
void ParameterData::setValue(int32_t val)
{
	if (val < min)
		val = min;
	if (val > max)
		val = max;


//	if (inverted)
//		val = max - val + min;

	this->panelValue = val;

	if (panelControlId == PanelControl::PANEL_CONTROL_MASTER_PITCH && parameterType == 1)
	{
		DBG("Panel value set to: " + String(val));
	}

	//this->valueForPatch = val;

	if (!isPitch)
	{
		val += (128 * 128) / 2; // offset to positive values
	}
	//DBG("Output val set to: " + String(val));
	//this->msblsb = val;
	msb = (uint8_t) (val / 128);
	lsb = (uint8_t) ((uint32_t) val % 128);
}

int ParameterData::getPage() const
{
	return this->parameterPage;
}

int ParameterData::getParameter() const
{
	return this->parameterType;
}

int ParameterData::getMax()
{
	return max;
}

void ParameterData::setMax(int maxValue)
{
	max = maxValue;
}



int ParameterData::getMin()
{
	return min;
}

void ParameterData::setParameter(int param)
{
	this->parameterType = param;
}

void ParameterData::setPage(int page)
{
	this->parameterPage = page;
}

int32_t ParameterData::getPanelValue()
{
	return this->panelValue;
}

int ParameterData::getPanelControlId() const
{
	return this->panelControlId;
}

String ParameterData::getName() const
{

	return name;
}

int ParameterData::getSubId() const
{
	return this->subId;
}

/*
void ParameterData::setInverted(bool inverted)
{
	this->inverted = inverted;
}

*/

void ParameterData::setSlider(Slider* s)
{
	this->slider = s;
}

void ParameterData::setComboBox(ComboBox* c)
{
	this->comboBox = c;

	//this->min = 0;
	this->max = c->getNumItems() - 1;

	incrementStep = 1;
}

void ParameterData::setButton(Button* b)
{
	this->button = b;
	this->min = 0;
	this->max = 4;
}

Slider* ParameterData::getSlider()
{
	return this->slider;
}

String ParameterData::getComboText()
{
	String altText;
	if (comboBox != nullptr)
	{
		return comboBox->getText();
	} else
	{

		int32_t val = getPanelValue();
		int param = this->getParameter();
		int page = this->getPage();
		const StringArray* comboText = nullptr;


		if (page != 0)
		{
			if (param == NRPN_VALUE_PAGE_OPTION1)
			{
				switch (page)
				{
				default:
					break;
				case 26:
					comboText = &HPF_option1Text;
					break;
				case 3:
					comboText = &HardSyncOSC1_option1Text;
					break;
				case 10:
					comboText = &HardSyncOSC2_option1Text;
					break;
				case 15:
					comboText = &OSC3PM_option1Text;
					break;
				case 20:
					comboText = &Noise_option1Text;
					break;
				case 21:
					comboText = &MIX_option1Text;
					break;
				case 22:
					comboText = &LPF1Input_option1Text;
					break;
				case 23:
					comboText = &LPF1Res_option1Text;
					break;
				case 24:
					comboText = &LPF1Output_option1Text;
					break;
				case 25:
					comboText = &HPFInput_option1Text;
					break;
				case 29:
					comboText = &LPF2Input_option1Text;
					break;
				case 33:
					comboText = &VCAOut_option1Text;
					break;


				}

			}
			else if (param == NRPN_VALUE_PAGE_OPTION2)
			{
				switch (page)
				{
				default:
					break;

				case 3:
				case 10:
				case 17:
					comboText = &OSC_option1Text;
					break;


				}

			}
		}


		if (page != 0)
		{
			switch(param)
			{
			default:
				break;
			case NRPN_VALUE_PAGE_EG1_SOURCE:
				if (val > 3)
					altText = EGChoice_optionText[0];
				else
					altText = EGChoice_optionText[val+1];
				break;
			case NRPN_VALUE_PAGE_EG1_RESTART_ON_TRIGGER:
				comboText = &EGTrigger_optionText;
				break;
			case NRPN_VALUE_PAGE_LFO2_WAVEFORM:
				comboText = &LFOWaveform_optionText;
				break;
			case NRPN_VALUE_PAGE_LFO1_SOURCE:
				comboText = &LFOs_optionText;
				break;
			case NRPN_VALUE_PAGE_LFO2_TRIGGER:
				comboText = &LFOReset_optionText;
				break;
			case NRPN_VALUE_PAGE_LFO2_OUTPUT_MODE:
				comboText = &LFOMode_optionText;
				break;
			case NRPN_VALUE_PAGE_LFO2_PITCH_TRACK:
				comboText = &LFOTrackPitch_optionText;
				break;
			case NRPN_VALUE_PAGE_MOD1_DEST:
			case NRPN_VALUE_PAGE_MOD2_DEST:
			case NRPN_VALUE_PAGE_MOD3_DEST:
			case NRPN_VALUE_PAGE_MOD4_DEST:
			case NRPN_VALUE_PAGE_NOTE_DEST:
			case NRPN_VALUE_PAGE_VELOCITY_DEST:
				comboText = &ModDest_optionText;
				break;



			}
		} else
		{
			switch(param)
			{
			case NRPN_VALUE_GLOBAL_LFO1_FREQ:
			case NRPN_VALUE_GLOBAL_LFO2_FREQ:

			case NRPN_VALUE_GLOBAL_LFO3_FREQ:

			case NRPN_VALUE_GLOBAL_LFO4_FREQ:

				val = -val;
				comboText = &LFOSync_option1Text;
				break;
			case NRPN_VALUE_GLOBAL_LFO1_WAVEFORM:
			case NRPN_VALUE_GLOBAL_LFO2_WAVEFORM:
			case NRPN_VALUE_GLOBAL_LFO3_WAVEFORM:
			case NRPN_VALUE_GLOBAL_LFO4_WAVEFORM:

				comboText = &LFOWaveform_optionText;
				break;
			case NRPN_VALUE_GLOBAL_EG1_RESTART_ON_TRIGGER:
			case NRPN_VALUE_GLOBAL_EG2_RESTART_ON_TRIGGER:
			case NRPN_VALUE_GLOBAL_EG3_RESTART_ON_TRIGGER:
			case NRPN_VALUE_GLOBAL_EG4_RESTART_ON_TRIGGER:
				comboText = &EGTrigger_optionText;
				break;
			case NRPN_VALUE_GLOBAL_LFO1_PITCH_TRACK:
			case NRPN_VALUE_GLOBAL_LFO2_PITCH_TRACK:
			case NRPN_VALUE_GLOBAL_LFO3_PITCH_TRACK:
			case NRPN_VALUE_GLOBAL_LFO4_PITCH_TRACK:
				comboText = &LFOTrackPitch_optionText;
				break;
			}
		}



		String text;

		if (comboText != nullptr && val < comboText->size())
		{
			text = (*comboText)[val];
		} else if (!altText.isEmpty())
		{
			text = altText;
		}
		else
		{
			text = String(val);
			//text = String("Hello");
		}

		return text;
	}
}

int32_t ParameterData::getIncrementStep(bool bigStep)
{

	int32_t inc;
	if (bigStep)
		inc = incrementStep;
	else
		inc = 1;

//	if (inverted)
//		inc = -inc;

	return inc;
}

void ParameterData::setIncrementStep(int32_t stepValue)
{
	incrementStep = stepValue;
}

void ParameterData::setToDefault()
{

	setValueFromPatch(defaultValue);
}

NRPNmessage_t ParameterData::convertBufferToNRPN(uint8_t* buf)
{
	NRPNmessage_t n;

	n.parameterMSB = buf[0];
	n.parameterLSB = buf[1];
	n.valueMSB = buf[2];
	n.valueLSB = buf[3];
	return n;

}

void ParameterData::setPanelValue(int32_t val)
{
	this->panelValue = val;
}

String ParameterData::getDisplayHumanText()
{

		String disp = "?";
		int32_t value = getPanelValue();
		int page = -1;
		int displayMode = -1;
		bool showPolarityPlus = false;
		if (getPage() > 0) // not a global
		{
			switch (getParameter())
			{
				default:
					displayMode = DISPLAY_MODE_NUMERIC; // just show the number
					break;

				case PanelControl::CONTROL_PARAM_EG_SHAPE_MULTI:
						displayMode = DISPLAY_MODE_SHAPE;
						break;
				case PanelControl::CONTROL_PARAM_LFO_WAVE:
				case PanelControl::CONTROL_PARAM_LFO_SOURCE:
				case PanelControl::CONTROL_PARAM_LFO_OUTPUT_MODE:
				case PanelControl::CONTROL_PARAM_LFO2_PITCH_TRACK:
				case PanelControl::CONTROL_PARAM_LFO_TRIGGER:
				case PanelControl::CONTROL_PARAM_M1_DESTINATION:
				case PanelControl::CONTROL_PARAM_M2_DESTINATION:
				case PanelControl::CONTROL_PARAM_M3_DESTINATION:
				case PanelControl::CONTROL_PARAM_M4_DESTINATION:
				case PanelControl::CONTROL_PARAM_NOTE_DESTINATION:
				case PanelControl::CONTROL_PARAM_VELOCITY_DESTINATION:
				//case PanelControl::CONTROL_PARAM_EG_SHAPE:
				case PanelControl::CONTROL_PARAM_EG_SOURCE:
				case PanelControl::CONTROL_PARAM_EG_RETRIG:
				case PanelControl::CONTROL_PARAM_PAGE_OPTION1:
				case PanelControl::CONTROL_PARAM_PAGE_OPTION2:
					displayMode = DISPLAY_MODE_COMBO;
					break;
				case PanelControl::CONTROL_PARAM_EG_DELAY:
					displayMode = DISPLAY_MODE_DELAY_TIME;
					break;
				case PanelControl::CONTROL_PARAM_EG_ATTACK:
				case PanelControl::CONTROL_PARAM_EG_DECAY:
				case PanelControl::CONTROL_PARAM_EG_RELEASE:
					displayMode = DISPLAY_MODE_EG_TIME;
					break;
				case PanelControl::CONTROL_PARAM_LFO_FREQ:
					displayMode = DISPLAY_MODE_LFO_RATE;
					break;

				case PanelControl::CONTROL_PARAM_EG_SUSTAIN:
					displayMode = DISPLAY_MODE_PERCENT; // always show % even on freq/VCF pages
					break;
				case PanelControl::CONTROL_PARAM_M1_ALT_LEVEL:
				case PanelControl::CONTROL_PARAM_M2_ALT_LEVEL:
				case PanelControl::CONTROL_PARAM_M3_ALT_LEVEL:
				case PanelControl::CONTROL_PARAM_M4_ALT_LEVEL:
				case PanelControl::CONTROL_PARAM_VELOCITY_ALT_LEVEL:
				case PanelControl::CONTROL_PARAM_NOTE_ALT_LEVEL:
					showPolarityPlus = true;
					displayMode = DISPLAY_MODE_PERCENT; // always show % even on freq/VCF pages
					break;
				case PanelControl::CONTROL_PARAM_OFFSET:
					displayMode = DISPLAY_MODE_PARAMETER_OFFSET;
					break;
				case PanelControl::CONTROL_PARAM_EG_MOD_DEPTH:
				case PanelControl::CONTROL_PARAM_LFOX_MOD_DEPTH:
				case PanelControl::CONTROL_PARAM_LFO_MOD_DEPTH:
				case PanelControl::CONTROL_PARAM_M1_LEVEL:
				case PanelControl::CONTROL_PARAM_M2_LEVEL:
				case PanelControl::CONTROL_PARAM_M3_LEVEL:
				case PanelControl::CONTROL_PARAM_M4_LEVEL:
				case PanelControl::CONTROL_PARAM_VELOCITY_LEVEL:
				case PanelControl::CONTROL_PARAM_NOTE_LEVEL:
					showPolarityPlus = true;
					displayMode = DISPLAY_MODE_DEFAULT; // sometimes %, sometimes pitch
					break;
			}
			switch (getPage())
			{
				default:
					page = PAGE_DEFAULT;
					break;
				case PanelControl::CONTROL_MASTER_PITCH:
					page = PAGE_MASTER_FREQ;
					break;

				case PanelControl::CONTROL_OSC1_PITCH:
				case PanelControl::CONTROL_OSC2_PITCH:
				case PanelControl::CONTROL_OSC3_PITCH:
					page = PAGE_OSC_FREQ;
					break;
				case PanelControl::CONTROL_LPF1:
					page = PAGE_LPF1_FREQ;
					break;
				case PanelControl::CONTROL_LPF2:
					page = PAGE_LPF2_FREQ;
					break;
				case PanelControl::CONTROL_HPF:
					page = PAGE_HPF_FREQ;
					break;

			}
		} else // globals, no page involved
		{
			switch (getParameter())
			{
				default:
					displayMode = DISPLAY_MODE_NUMERIC; // just show the number
					break;
				case PanelControl::CONTROL_GLOBAL_EGX1_DELAY:
				case PanelControl::CONTROL_GLOBAL_EGX2_DELAY:
				case PanelControl::CONTROL_GLOBAL_EGX3_DELAY:
				case PanelControl::CONTROL_GLOBAL_EGX4_DELAY:
					displayMode = DISPLAY_MODE_DELAY_TIME;
					break;
				case PanelControl::CONTROL_GLOBAL_LFOX1_TRIGGER:
				case PanelControl::CONTROL_GLOBAL_LFOX1_WAVEFORM:
				case PanelControl::CONTROL_GLOBAL_LFOX2_TRIGGER:
				case PanelControl::CONTROL_GLOBAL_LFOX2_WAVEFORM:
				case PanelControl::CONTROL_GLOBAL_LFOX3_TRIGGER:
				case PanelControl::CONTROL_GLOBAL_LFOX3_WAVEFORM:
				case PanelControl::CONTROL_GLOBAL_LFOX4_TRIGGER:
				case PanelControl::CONTROL_GLOBAL_LFOX4_WAVEFORM:
				case PanelControl::CONTROL_GLOBAL_EGX1_SHAPE:
				case PanelControl::CONTROL_GLOBAL_EGX1_TRIGGER:
				case PanelControl::CONTROL_GLOBAL_EGX2_SHAPE:
				case PanelControl::CONTROL_GLOBAL_EGX2_TRIGGER:
				case PanelControl::CONTROL_GLOBAL_EGX3_SHAPE:
				case PanelControl::CONTROL_GLOBAL_EGX3_TRIGGER:
				case PanelControl::CONTROL_GLOBAL_EGX4_SHAPE:
				case PanelControl::CONTROL_GLOBAL_EGX4_TRIGGER:
				case PanelControl::CONTROL_GLOBAL_LFO1_PITCH_TRACK:
				case PanelControl::CONTROL_GLOBAL_LFO2_PITCH_TRACK:
				case PanelControl::CONTROL_GLOBAL_LFO3_PITCH_TRACK:
				case PanelControl::CONTROL_GLOBAL_LFO4_PITCH_TRACK:
					displayMode = DISPLAY_MODE_COMBO;
					break;
				case PanelControl::CONTROL_GLOBAL_EGX1_SHAPE_MULTI:
				case PanelControl::CONTROL_GLOBAL_EGX2_SHAPE_MULTI:
				case PanelControl::CONTROL_GLOBAL_EGX3_SHAPE_MULTI:
				case PanelControl::CONTROL_GLOBAL_EGX4_SHAPE_MULTI:
					displayMode = DISPLAY_MODE_SHAPE;
					break;
				case PanelControl::CONTROL_GLOBAL_EGX1_ATTACK:
				case PanelControl::CONTROL_GLOBAL_EGX2_ATTACK:
				case PanelControl::CONTROL_GLOBAL_EGX3_ATTACK:
				case PanelControl::CONTROL_GLOBAL_EGX4_ATTACK:
				case PanelControl::CONTROL_GLOBAL_EGX1_DECAY:
				case PanelControl::CONTROL_GLOBAL_EGX2_DECAY:
				case PanelControl::CONTROL_GLOBAL_EGX3_DECAY:
				case PanelControl::CONTROL_GLOBAL_EGX4_DECAY:
				case PanelControl::CONTROL_GLOBAL_EGX1_RELEASE:
				case PanelControl::CONTROL_GLOBAL_EGX2_RELEASE:
				case PanelControl::CONTROL_GLOBAL_EGX3_RELEASE:
				case PanelControl::CONTROL_GLOBAL_EGX4_RELEASE:
					displayMode = DISPLAY_MODE_EG_TIME;
					break;
				case PanelControl::CONTROL_GLOBAL_LFOX1_FREQ:
				case PanelControl::CONTROL_GLOBAL_LFOX2_FREQ:
				case PanelControl::CONTROL_GLOBAL_LFOX3_FREQ:
				case PanelControl::CONTROL_GLOBAL_LFOX4_FREQ:
					displayMode = DISPLAY_MODE_LFO_RATE;
					break;
				case PanelControl::CONTROL_GLOBAL_EGX1_SUSTAIN:
				case PanelControl::CONTROL_GLOBAL_EGX2_SUSTAIN:
				case PanelControl::CONTROL_GLOBAL_EGX3_SUSTAIN:
				case PanelControl::CONTROL_GLOBAL_EGX4_SUSTAIN:
					displayMode = DISPLAY_MODE_PERCENT;
					break;
				case PanelControl::CONTROL_PARAM_LFO2_LFOX_FREQ_MOD:
				case PanelControl::CONTROL_PARAM_LFO2_EG_FREQ_MOD:

					displayMode = DISPLAY_MODE_PERCENT;
					break;
			}
		}


		switch (displayMode)
		{
			case DISPLAY_MODE_SHAPE:
				disp = String(value);
				 if (value >= 40)
					 disp += String(" LOG-F");
				 else if (value >= 15)
					 disp += String(" LOG");
				 else if (value >= -14)
					disp += String(" LINEAR");
				 else if (value > -39)
					disp += String(" EXP");
				 else
					 disp += String(" EXP-F");
				break;

			case DISPLAY_MODE_NUMERIC:
				disp = String(value);

				break;
			case DISPLAY_MODE_COMBO:
				disp = getComboText();
				break;
			case DISPLAY_MODE_EG_TIME:
				value = (int32_t) Utilities::convertEGValueToTime(value);
				disp = 	Utilities::getTimeString((100 * EG_RATE_MAX_VALUE) / (uint32_t) value);
				break;
			case DISPLAY_MODE_DELAY_TIME:
				disp =  Utilities::getTimeString((( 100000 * (uint32_t) value ) / PARAMETER_UPDATE_RATE_IN_HZ));
				break;
			case DISPLAY_MODE_LFO_RATE:
			{
				// multiply by 1000 to deal with decimal point
				uint32_t freq = (uint32_t) Utilities::convertLFOrate(value);
				if (value >= 0)
				{
					if (freq < 4096)
						freq = (((uint32_t) (1000 / 2) * PARAMETER_UPDATE_RATE_IN_HZ * freq)) / LFO_MAX_AMPLITUDE ;
					else if (freq < 4096 * 16)
						freq = (((uint32_t) (1000 / (2 * 4)) * PARAMETER_UPDATE_RATE_IN_HZ * (freq / 4))) / (LFO_MAX_AMPLITUDE / 16) ;
					// halve since LFO goes from LFO_MAX_AMPLITUDE to -LFO_MAX_AMPLITUDE
					else
						freq = (((uint32_t) (1000 / (2 * 4)) * PARAMETER_UPDATE_RATE_IN_HZ * (freq / (4 * 64)))) / (LFO_MAX_AMPLITUDE / (16 * 64));

					if (freq > 100000)
					{
						disp = String(freq / 1000) + " Hz";
					}
					else if (freq > 10000)
					{
						disp = String(((float) (freq / 100)) / 10.0f, 1) + " Hz";
					}
					else if (freq > 1000)
					{
						disp = String(((float) (freq / 10)) / 100.0f, 2) + " Hz";
					} else
					{
						disp = String(((float) freq) / 1000.0f, 3) + " Hz";
					}
				} else
				{
					disp =  getComboText();
				}
				break;
			}
			default:
				if (page == PAGE_OSC_FREQ || page == PAGE_MASTER_FREQ)
				{
					if (displayMode == DISPLAY_MODE_PERCENT)
					{
						int32_t percent = (100*value) / MAX_LEVEL_VALUE;
						disp = String(percent) + " %";
						if (percent > 0 && showPolarityPlus)
							disp = "+" + disp;
					}
					else
					{
						int32_t semitones = (100*value) / (300 / 4); // x 100
						if (displayMode == DISPLAY_MODE_PARAMETER_OFFSET)
						{
							if (page == PAGE_MASTER_FREQ)
								semitones  = (100*(value)) / (300 / 4); // divide by 4 since pitch has rescale factor 4 at low level
							else
							{
								value *= 3;
								semitones  = (100*(value - (MAX_LEVEL_VALUE_FREQ / 2))) /300; // 300 units per semitone
							}
						}
						if (semitones >= 100 || semitones <= -100)
						{
							disp = String(((float) semitones) / 100.0f , 2) + " semi-tones";
						}
						else
						{
							disp = String( semitones) + " cents";
						}
						if (semitones > 0 )
							disp = "+" + disp;

					}
				} else if (page == PAGE_LPF1_FREQ
						|| page == PAGE_LPF2_FREQ
						|| page == PAGE_HPF_FREQ)
				{
					switch (displayMode)
					{
						case DISPLAY_MODE_PERCENT:
							{
								int32_t percent = (100*value) / MAX_LEVEL_VALUE;
								disp = String( percent) + " %";
								if (value > 0)
									disp = "+" + disp;
							}
							break;
						default:
							{
								int32_t octave = (100 * value) / 333; // x 100

								if (octave >= 1000 || octave  <= -1000)
									disp = String(((float) (octave / 10)) / 10.0f , 1) + " octave";
								else
									disp = String(((float) octave) / 100.0f, 2) + " octave";

								if (octave > 0)
									disp = "+" + disp;
							}
							break;
						case DISPLAY_MODE_PARAMETER_OFFSET:
							if (value <= 4095)
							{
								float b =  logf(2.0f) / VCF_STEPS_PER_OCTAVE; // this parameter is from the scaling factors from DAC to exponential current generation circuitry.
								float a;
								switch (page)
								{
									default:
									case PAGE_LPF1_FREQ:
										a = 110.0f / expf(VCF1_VALUE_FOR_110Hz * (logf(2.0f) / VCF_STEPS_PER_OCTAVE)); // the frequency when the voltage from DAC is zero volts.
										break;
									case PAGE_LPF2_FREQ:
										a = 110.0f / expf(VCF2_VALUE_FOR_110Hz * (logf(2.0f) / VCF_STEPS_PER_OCTAVE));
										break;
									case PAGE_HPF_FREQ:
										a = 110.0f / expf(HPF_VALUE_FOR_110Hz * (logf(2.0f) / VCF_STEPS_PER_OCTAVE));
										break;
								}
								int32_t freq = (int32_t) (a * expf(b * value));
								if (freq >= 24000)
									freq = 24000;
								if (freq > 10000)
								{
									disp = String(((float) (freq / 100)) / 10.0f , 1) + " kHz";
								}
								else if (freq > 1000)
								{
									disp = String(((float) (freq / 10 )) / 100.0f , 2) + " kHz";
								}
								else
								{
									disp = String(freq) + " Hz";
								}
							}
							break;
					}
				}
				else
				{
					// just show percent with 1% resolution
					int32_t percent = (100*value) / MAX_LEVEL_VALUE;
					disp = String(percent) + " %";

					if (percent > 0 && showPolarityPlus)
						disp = "+" + disp;
				}
				break;
		}
		return disp;
}


