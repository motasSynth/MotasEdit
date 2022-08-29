/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/


#include "./Headers/PluginProcessor.h"

#include "./Headers/HoldingWindowComponent.h"

#include "Headers/FifoMidi.h"
#include "Headers/MIDIProcessor.h"
#include "Headers/MotasPluginParameter.h"
#include "Headers/MainComponent.h"


#define  LAMBDA_FUNCTION_CALL [paramPage, param, this](int val, int maxLength) {return getReadableValue(paramPage, param, val, maxLength);}



static int32_t ControllerChangeGlobalGetMinValue(int32_t parameterLSB)
{
	int32_t returnVal;
	switch (parameterLSB)
	{
		case NRPN_VALUE_GLOBAL_LFO1_FREQ:
		case NRPN_VALUE_GLOBAL_LFO2_FREQ:
		case NRPN_VALUE_GLOBAL_LFO3_FREQ:
		case NRPN_VALUE_GLOBAL_LFO4_FREQ:
			returnVal = MIN_VALUE_GLOBAL_LFO;
			break;
		case NRPN_VALUE_GLOBAL_EG1_SHAPE_MULTI:
		case NRPN_VALUE_GLOBAL_EG2_SHAPE_MULTI:
		case NRPN_VALUE_GLOBAL_EG3_SHAPE_MULTI:
		case NRPN_VALUE_GLOBAL_EG4_SHAPE_MULTI:
			returnVal = -64;
			break;
		default:
			returnVal = 0;
	}
	return returnVal;
}
static int32_t ControllerChangeLocalGetDefaultValue(int32_t parameterLSB, bool VCOPage)
{
	int32_t returnVal;
	switch (parameterLSB)
	{
		default:
			returnVal = 0;
			break;
		case NRPN_VALUE_PAGE_OFFSET:
		{
			if (VCOPage)
			{
				returnVal = ((MAX_LEVEL_VALUE_FREQ / 2) / 3);
			} else
			{
				returnVal = 0;
			}
			break;
		}

		case NRPN_VALUE_PAGE_EG1_SOURCE:
			returnVal = NUM_GLOBAL_EGS; // default is local
			break;

	}
	return returnVal;

}
static int32_t ControllerChangeLocalGetMinValue(int32_t parameterLSB, bool masterPitchPage)
{
	int32_t returnVal;
	switch (parameterLSB)
	{
		case NRPN_VALUE_PAGE_OFFSET:
			if (masterPitchPage)
				returnVal = 0;
			else
				returnVal = 0;//-4095;
			break;

		case NRPN_VALUE_PAGE_EG1_MOD_DEPTH:

		case NRPN_VALUE_PAGE_LFO1_MOD_DEPTH:

		case NRPN_VALUE_PAGE_LFO2_MOD_DEPTH:

		case NRPN_VALUE_PAGE_MOD1_MOD_DEPTH:
		case NRPN_VALUE_PAGE_MOD1_MOD_DEPTH_ALTERNATE:
		case NRPN_VALUE_PAGE_MOD2_MOD_DEPTH:
		case NRPN_VALUE_PAGE_MOD2_MOD_DEPTH_ALTERNATE:
		case NRPN_VALUE_PAGE_MOD3_MOD_DEPTH:
		case NRPN_VALUE_PAGE_MOD3_MOD_DEPTH_ALTERNATE:
		case NRPN_VALUE_PAGE_MOD4_MOD_DEPTH:
		case NRPN_VALUE_PAGE_MOD4_MOD_DEPTH_ALTERNATE:
		case NRPN_VALUE_PAGE_VELOCITY_MOD_DEPTH:
		case NRPN_VALUE_PAGE_VELOCITY_MOD_DEPTH_ALTERNATE:
		case NRPN_VALUE_PAGE_NOTE_MOD_DEPTH:
		case NRPN_VALUE_PAGE_NOTE_MOD_DEPTH_ALTERNATE:
		case NRPN_VALUE_PAGE_LFO2_LFOX_FREQ_MOD:
		case NRPN_VALUE_PAGE_LFO2_EG_FREQ_MOD:
			returnVal = -4095;
			break;
		case NRPN_VALUE_PAGE_EG1_SHAPE_MULTI:
			returnVal = -64;
			break;
		default:
			returnVal = 0;
			break;
		}
	return returnVal;
}


static int32_t ControllerChangeGlobalGetDefaultValue(int32_t )
{
	int32_t returnVal = 0;

	return returnVal;

}


static int32_t ControllerChangeGlobalGetMaxValue(int32_t parameterLSB)
{
	int32_t returnVal;
	switch (parameterLSB)
	{
		default:
			returnVal = 4095;
			break;

		case NRPN_VALUE_GLOBAL_PORTAMENTO_MODE:
			returnVal = 2;
			break;
		case NRPN_VALUE_GLOBAL_PORTAMENTO_TIME_OR_RATE:
			returnVal = 1;
			break;
		case NRPN_VALUE_GLOBAL_PORTAMENTO_TIME:
			returnVal = 2000;
			break;
		case NRPN_VALUE_GLOBAL_LFO_SYNC_MIDI_START:
			returnVal = 1;
			break;
		case NRPN_VALUE_GLOBAL_MOD1_SOURCE:
		case NRPN_VALUE_GLOBAL_MOD2_SOURCE:
		case NRPN_VALUE_GLOBAL_MOD3_SOURCE:
		case NRPN_VALUE_GLOBAL_MOD4_SOURCE:
			returnVal = MAX_MIDI_CONTROL_VALUE;
			break;
		case NRPN_VALUE_GLOBAL_LFO1_TRIGGER:
		case NRPN_VALUE_GLOBAL_LFO2_TRIGGER:
		case NRPN_VALUE_GLOBAL_LFO3_TRIGGER:
		case NRPN_VALUE_GLOBAL_LFO4_TRIGGER:
			returnVal = LFO_TRIGGER_MAX_STATE - 1;
			break;
		case NRPN_VALUE_GLOBAL_LFO1_SINGLE_SHOT:
		case NRPN_VALUE_GLOBAL_LFO2_SINGLE_SHOT:
		case NRPN_VALUE_GLOBAL_LFO3_SINGLE_SHOT:
		case NRPN_VALUE_GLOBAL_LFO4_SINGLE_SHOT:
			returnVal = LFO2_EDIT_SINGLE_SHOT_MAX_VALUE;
			break;
		case NRPN_VALUE_GLOBAL_LFO1_WAVEFORM:
		case NRPN_VALUE_GLOBAL_LFO2_WAVEFORM:
		case NRPN_VALUE_GLOBAL_LFO3_WAVEFORM:
		case NRPN_VALUE_GLOBAL_LFO4_WAVEFORM:
			returnVal = LFO1_MAX_CHOICE_VALUE;
			break;
		case NRPN_VALUE_GLOBAL_LFO1_PITCH_TRACK:
		case NRPN_VALUE_GLOBAL_LFO2_PITCH_TRACK:
		case NRPN_VALUE_GLOBAL_LFO3_PITCH_TRACK:
		case NRPN_VALUE_GLOBAL_LFO4_PITCH_TRACK:
			returnVal = LFO_MODE_MAX-1;
			break;
		case NRPN_VALUE_GLOBAL_EG1_TRIGGER:
		case NRPN_VALUE_GLOBAL_EG2_TRIGGER:
		case NRPN_VALUE_GLOBAL_EG3_TRIGGER:
		case NRPN_VALUE_GLOBAL_EG4_TRIGGER:
			returnVal = EG_EDIT_KEYTRACK_MAX_VALUE;
			break;
		case NRPN_VALUE_GLOBAL_EG1_RESTART_ON_TRIGGER:
		case NRPN_VALUE_GLOBAL_EG2_RESTART_ON_TRIGGER:
		case NRPN_VALUE_GLOBAL_EG3_RESTART_ON_TRIGGER:
		case NRPN_VALUE_GLOBAL_EG4_RESTART_ON_TRIGGER:
			returnVal =  3;
			break;
		case NRPN_VALUE_GLOBAL_EG1_SHAPE:
		case NRPN_VALUE_GLOBAL_EG2_SHAPE:
		case NRPN_VALUE_GLOBAL_EG3_SHAPE:
		case NRPN_VALUE_GLOBAL_EG4_SHAPE:
			returnVal = 2;
			break;
		case NRPN_VALUE_GLOBAL_EG1_SHAPE_MULTI:
		case NRPN_VALUE_GLOBAL_EG2_SHAPE_MULTI:
		case NRPN_VALUE_GLOBAL_EG3_SHAPE_MULTI:
		case NRPN_VALUE_GLOBAL_EG4_SHAPE_MULTI:
			returnVal = EG_EDIT_SHAPE_MAX_VALUE;
			break;


		case NRPN_VALUE_GLOBAL_EG1_UNIPOLAR:
		case NRPN_VALUE_GLOBAL_EG2_UNIPOLAR:
		case NRPN_VALUE_GLOBAL_EG3_UNIPOLAR:
		case NRPN_VALUE_GLOBAL_EG4_UNIPOLAR:
			returnVal = EG_EDIT_UNIPOLAR_MAX_VALUE;
			break;
	}
	return returnVal;
}

static int32_t ControllerChangeLocalGetMaxValue(int32_t parameterLSB, bool VCOPage)
{
	int32_t returnVal;
	switch (parameterLSB)
	{
			default:
				returnVal = 4095;
				break;
			case NRPN_VALUE_PAGE_OFFSET:
			{
				#if MOTAS_VERSION == 6
					if (VCOPage)
					//if (pageButton == BUTTON_VCO1_FREQ || pageButton == BUTTON_VCO2_FREQ || pageButton == BUTTON_VCO3_FREQ)
					{
						returnVal = ((MAX_LEVEL_VALUE_FREQ) / 3) ;
					} else
					{
						returnVal = 4095;
					}

				#else

					if (pageButton == BUTTON_VCO1_FREQ || pageButton == BUTTON_VCO2_FREQ || pageButton == BUTTON_VCO3_FREQ)
					{
						p->basicOffset = checkValuePitch(msg);
					} else
					{
						p->basicOffset = checkNRPNValue(-4095, 4095, msg); // subtract current pot value so value is corrected elsewhere in interrupt routine encoder.c
					}

				#endif

				break;
			}
			case NRPN_VALUE_PAGE_MOD1_MOD_UNIPOLAR:
			case NRPN_VALUE_PAGE_MOD2_MOD_UNIPOLAR:
			case NRPN_VALUE_PAGE_MOD3_MOD_UNIPOLAR:
			case NRPN_VALUE_PAGE_MOD4_MOD_UNIPOLAR:
			case NRPN_VALUE_PAGE_VELOCITY_MOD_UNIPOLAR:
			case NRPN_VALUE_PAGE_NOTE_MOD_UNIPOLAR:
				returnVal = 1;
				break;
			case NRPN_VALUE_PAGE_OPTION1:
				if (VCOPage)
					returnVal = 3;
				else
					returnVal = 7;

				break;
			case NRPN_VALUE_PAGE_OPTION2:
				if (VCOPage)
					returnVal = 6;
				else
					returnVal = 3;
				break;


			case NRPN_VALUE_PAGE_EG1_SOURCE:
				returnVal = NUM_GLOBAL_EGS;//0xFF;
				break;
			case NRPN_VALUE_PAGE_EG1_RETRIG:
				returnVal = EG_EDIT_KEYTRACK_MAX_VALUE;
				break;
			case NRPN_VALUE_PAGE_EG1_RESTART_ON_TRIGGER:
				returnVal = 3;
				break;

			case NRPN_VALUE_PAGE_EG1_SHAPE: // old version
				returnVal = 2;
				break;
			case NRPN_VALUE_PAGE_EG1_SHAPE_MULTI:
				returnVal = 64;
				break;
			case NRPN_VALUE_PAGE_EG1_UNIPOLAR:
				returnVal = EG_EDIT_UNIPOLAR_MAX_VALUE;
				break;



			case NRPN_VALUE_PAGE_LFO1_SOURCE:
				returnVal = 3;
				break;



			case NRPN_VALUE_PAGE_LFO2_WAVEFORM:
				returnVal = LFO2_MAX_CHOICE_VALUE;
				break;
			case NRPN_VALUE_PAGE_LFO2_TRIGGER:
				returnVal = LFO_TRIGGER_MAX_STATE - 1;
				break;
			case NRPN_VALUE_PAGE_LFO2_SINGLE_SHOT:
				returnVal = LFO2_EDIT_SINGLE_SHOT_MAX_VALUE;
				break;
			case NRPN_VALUE_PAGE_LFO2_OUTPUT_MODE:
				returnVal = LFO2_EDIT_OUTPUT_MODE_MAX_VALUE;
				break;
			case NRPN_VALUE_PAGE_LFO2_PITCH_TRACK:
				returnVal = LFO2_EDIT_PITCH_TRACK_MAX_VALUE;
				break;


			case NRPN_VALUE_PAGE_MOD1_DEST:
			case NRPN_VALUE_PAGE_MOD2_DEST:
			case NRPN_VALUE_PAGE_MOD3_DEST:
			case NRPN_VALUE_PAGE_MOD4_DEST:
			case NRPN_VALUE_PAGE_VELOCITY_DEST:
			case NRPN_VALUE_PAGE_NOTE_DEST:
				returnVal = MOD_DEST_END - 1;
				break;

	}
	return returnVal;
}



//==============================================================================
MotasEditPluginAudioProcessor::MotasEditPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ) , appCommonPtr(&appCommon)//, parametersTreeState (*this, nullptr)
#endif
{



	this->midiProcessor = std::make_unique<MIDIProcessor>(this);
	appCommonPtr->setMIDIProcessor(midiProcessor.get());
	midiProcessor->updateMIDISettings();





	MIDIoutSampleCount = 0;

	int page = 0;


	addPluginParamsForPage("MASTER Pitch", ++page, 1);


	addPluginParamsForPage("OSC1 Level", ++page);
	addPluginParamsForPage("OSC1 Pitch", ++page);
	addPluginParamsForPage("OSC1 Tri Level", ++page);
	addPluginParamsForPage("OSC1 Saw Level", ++page);
	addPluginParamsForPage("OSC1 PWM", ++page);
	addPluginParamsForPage("OSC1 PWM Level", ++page);

	addPluginParamsForPage("OSC2 PMod Depth", ++page);
	addPluginParamsForPage("OSC2 Level", ++page);
	addPluginParamsForPage("OSC2 Pitch", ++page);
	addPluginParamsForPage("OSC2 Tri Level", ++page);
	addPluginParamsForPage("OSC2 Saw Level",++page);
	addPluginParamsForPage("OSC2 Square Level", ++page);
	addPluginParamsForPage("OSC2 Sub Level", ++page);

	addPluginParamsForPage("OSC3 PMod Depth", ++page);
	addPluginParamsForPage("OSC3 Level", ++page);
	addPluginParamsForPage("OSC3 Pitch", ++page);
	addPluginParamsForPage("OSC3 Tri Level", ++page);
	addPluginParamsForPage("OSC3 Saw Level", ++page);
	addPluginParamsForPage("OSC3 PWM", ++page);
	addPluginParamsForPage("OSC3 PWM Level", ++page);

	addPluginParamsForPage("Noise Level",++page);
	addPluginParamsForPage("MIX Level", ++page);
	addPluginParamsForPage("LPF1 Freq", ++page, 1);
	addPluginParamsForPage("LPF1 Res Amount", ++page);
	addPluginParamsForPage("LPF1 Out Level", ++page);
	addPluginParamsForPage("HPF Freq", ++page);
	addPluginParamsForPage("HPF Out Level", ++page);
	addPluginParamsForPage("FDBK/EXT Level", ++page);
	++page;
	addPluginParamsForPage("LPF2 Freq", ++page, 1);
	addPluginParamsForPage("LPF2 Res Amount", ++page);
	addPluginParamsForPage("LPF2 Out Level", ++page);
	++page;
	addPluginParamsForPage("OUTPUT Level", ++page, 1);


	addPluginGlobalParam("LFO1 Wave", NRPN_VALUE_GLOBAL_LFO1_WAVEFORM);
	addPluginGlobalParam("LFO2 Wave", NRPN_VALUE_GLOBAL_LFO2_WAVEFORM);
	addPluginGlobalParam("LFO3 Wave", NRPN_VALUE_GLOBAL_LFO3_WAVEFORM);
	addPluginGlobalParam("LFO4 Wave", NRPN_VALUE_GLOBAL_LFO4_WAVEFORM);



	addPluginGlobalParam("LFO1 Freq", NRPN_VALUE_GLOBAL_LFO1_FREQ);
	addPluginGlobalParam("LFO2 Freq", NRPN_VALUE_GLOBAL_LFO2_FREQ);
	addPluginGlobalParam("LFO3 Freq", NRPN_VALUE_GLOBAL_LFO3_FREQ);
	addPluginGlobalParam("LFO4 Freq", NRPN_VALUE_GLOBAL_LFO4_FREQ);

	addPluginGlobalParam("LFO1 Trigger", NRPN_VALUE_GLOBAL_LFO1_TRIGGER);
	addPluginGlobalParam("LFO2 Trigger", NRPN_VALUE_GLOBAL_LFO2_TRIGGER);
	addPluginGlobalParam("LFO3 Trigger", NRPN_VALUE_GLOBAL_LFO3_TRIGGER);
	addPluginGlobalParam("LFO4 Trigger", NRPN_VALUE_GLOBAL_LFO4_TRIGGER);

	addPluginGlobalParam("LFO1 SS", NRPN_VALUE_GLOBAL_LFO1_SINGLE_SHOT);
	addPluginGlobalParam("LFO2 SS", NRPN_VALUE_GLOBAL_LFO2_SINGLE_SHOT);
	addPluginGlobalParam("LFO3 SS", NRPN_VALUE_GLOBAL_LFO3_SINGLE_SHOT);
	addPluginGlobalParam("LFO4 SS", NRPN_VALUE_GLOBAL_LFO4_SINGLE_SHOT);

	addPluginGlobalParam("LFO1 Pitch", NRPN_VALUE_GLOBAL_LFO1_PITCH_TRACK);
	addPluginGlobalParam("LFO2 Pitch", NRPN_VALUE_GLOBAL_LFO2_PITCH_TRACK);
	addPluginGlobalParam("LFO3 Pitch", NRPN_VALUE_GLOBAL_LFO3_PITCH_TRACK);
	addPluginGlobalParam("LFO4 Pitch", NRPN_VALUE_GLOBAL_LFO4_PITCH_TRACK);


	addPluginGlobalParam("EG1 Delay", NRPN_VALUE_GLOBAL_EG1_DELAY);
	addPluginGlobalParam("EG1 Attack", NRPN_VALUE_GLOBAL_EG1_ATTACK);
	addPluginGlobalParam("EG1 Decay", NRPN_VALUE_GLOBAL_EG1_DECAY);
	addPluginGlobalParam("EG1 Sustain", NRPN_VALUE_GLOBAL_EG1_SUSTAIN);
	addPluginGlobalParam("EG1 Release", NRPN_VALUE_GLOBAL_EG1_RELEASE);
	addPluginGlobalParam("EG1 Trigger", NRPN_VALUE_GLOBAL_EG1_TRIGGER);
	addPluginGlobalParam("EG1 Restart", NRPN_VALUE_GLOBAL_EG1_RESTART_ON_TRIGGER);
	addPluginGlobalParam("EG1 Unipolar", NRPN_VALUE_GLOBAL_EG1_UNIPOLAR);
	addPluginGlobalParam("EG1 Shape", NRPN_VALUE_GLOBAL_EG1_SHAPE_MULTI);

	addPluginGlobalParam("EG2 Delay", NRPN_VALUE_GLOBAL_EG2_DELAY);
	addPluginGlobalParam("EG2 Attack", NRPN_VALUE_GLOBAL_EG2_ATTACK);
	addPluginGlobalParam("EG2 Decay", NRPN_VALUE_GLOBAL_EG2_DECAY);
	addPluginGlobalParam("EG2 Sustain", NRPN_VALUE_GLOBAL_EG2_SUSTAIN);
	addPluginGlobalParam("EG2 Release", NRPN_VALUE_GLOBAL_EG2_RELEASE);
	addPluginGlobalParam("EG2 Trigger", NRPN_VALUE_GLOBAL_EG2_TRIGGER);
	addPluginGlobalParam("EG2 Restart", NRPN_VALUE_GLOBAL_EG2_RESTART_ON_TRIGGER);
	addPluginGlobalParam("EG2 Unipolar", NRPN_VALUE_GLOBAL_EG2_UNIPOLAR);
	addPluginGlobalParam("EG2 Shape", NRPN_VALUE_GLOBAL_EG2_SHAPE_MULTI);



	addPluginGlobalParam("EG3 Delay", NRPN_VALUE_GLOBAL_EG3_DELAY);
	addPluginGlobalParam("EG3 Attack", NRPN_VALUE_GLOBAL_EG3_ATTACK);
	addPluginGlobalParam("EG3 Decay", NRPN_VALUE_GLOBAL_EG3_DECAY);
	addPluginGlobalParam("EG3 Sustain", NRPN_VALUE_GLOBAL_EG3_SUSTAIN);
	addPluginGlobalParam("EG3 Release", NRPN_VALUE_GLOBAL_EG3_RELEASE);
	addPluginGlobalParam("EG3 Trigger", NRPN_VALUE_GLOBAL_EG3_TRIGGER);
	addPluginGlobalParam("EG3 Restart", NRPN_VALUE_GLOBAL_EG3_RESTART_ON_TRIGGER);
	addPluginGlobalParam("EG3 Unipolar", NRPN_VALUE_GLOBAL_EG3_UNIPOLAR);
	addPluginGlobalParam("EG3 Shape", NRPN_VALUE_GLOBAL_EG3_SHAPE_MULTI);


	addPluginGlobalParam("EG4 Delay", NRPN_VALUE_GLOBAL_EG4_DELAY);
	addPluginGlobalParam("EG4 Attack", NRPN_VALUE_GLOBAL_EG4_ATTACK);
	addPluginGlobalParam("EG4 Decay", NRPN_VALUE_GLOBAL_EG4_DECAY);
	addPluginGlobalParam("EG4 Sustain", NRPN_VALUE_GLOBAL_EG4_SUSTAIN);
	addPluginGlobalParam("EG4 Release", NRPN_VALUE_GLOBAL_EG4_RELEASE);
	addPluginGlobalParam("EG4 Trigger", NRPN_VALUE_GLOBAL_EG4_TRIGGER);
	addPluginGlobalParam("EG4 Restart", NRPN_VALUE_GLOBAL_EG4_RESTART_ON_TRIGGER);
	addPluginGlobalParam("EG4 Unipolar", NRPN_VALUE_GLOBAL_EG4_UNIPOLAR);
	addPluginGlobalParam("EG4 Shape", NRPN_VALUE_GLOBAL_EG4_SHAPE_MULTI);

	addPluginGlobalParam("Porta mode", NRPN_VALUE_GLOBAL_PORTAMENTO_MODE);
	addPluginGlobalParam("Porta time", NRPN_VALUE_GLOBAL_PORTAMENTO_TIME);
	addPluginGlobalParam("Porta time/rate", NRPN_VALUE_GLOBAL_PORTAMENTO_TIME_OR_RATE);

	addPluginGlobalParam("MOD1 source", NRPN_VALUE_GLOBAL_MOD1_SOURCE);
	addPluginGlobalParam("MOD2 source", NRPN_VALUE_GLOBAL_MOD2_SOURCE);
	addPluginGlobalParam("MOD3 source", NRPN_VALUE_GLOBAL_MOD3_SOURCE);
	addPluginGlobalParam("MOD4 source", NRPN_VALUE_GLOBAL_MOD4_SOURCE);



}


void MotasEditPluginAudioProcessor::addPluginParamsForPage(String pageName, int paramPage, int )
{

	String startChar = "-";
	String endChar = "";


	addPluginLocalParam(pageName + " MAIN OFFSET", paramPage, NRPN_VALUE_PAGE_OFFSET);

	String p;



	if (paramPage == 3 || paramPage == 10 || paramPage == 15   || paramPage == 17
			 || (paramPage >= 22  &&  paramPage <= 27) || paramPage == 31 || paramPage == 35)
	{
		p = pageName + startChar+ " page option1" + endChar;
		addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_OPTION1);

		if (paramPage == 3 || paramPage == 10 )
		{
			p = pageName + startChar+ " page option2" + endChar;
			addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_OPTION2);
		}
	}





	p = pageName + startChar+ " LFOx source" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_LFO1_SOURCE);

	p =  pageName + startChar + " LFOx mod depth" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_LFO1_MOD_DEPTH);






	p = pageName+ startChar + " LFO waveform" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_LFO2_WAVEFORM);

	p = pageName+ startChar + " LFO frequency" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_LFO2_FREQ);


	p = pageName + startChar+ " LFO mod depth"+ endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_LFO2_MOD_DEPTH);

	p = pageName + startChar+ " LFO trigger"+ endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_LFO2_TRIGGER);


	p = pageName + startChar+ " LFO single-shot"+ endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_LFO2_SINGLE_SHOT);

	p = pageName + startChar+ " LFO output mode"+ endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_LFO2_OUTPUT_MODE);

	p = pageName + startChar+ " LFO freq mode"+ endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_LFO2_PITCH_TRACK);

	p = pageName + startChar+ " LFO freq mod LFOx"+ endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_LFO2_LFOX_FREQ_MOD);

	p = pageName + startChar+ " LFO freq mod EG"+ endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_LFO2_EG_FREQ_MOD);




	p = pageName+ startChar + " EG mod depth" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_EG1_MOD_DEPTH);



	p = pageName + startChar+ " EG source" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_EG1_SOURCE);

	p = pageName+ startChar + " EG delay" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_EG1_DELAY);

	p = pageName+ startChar + " EG attack" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_EG1_ATTACK);

	p = pageName + startChar+ " EG decay" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_EG1_DECAY);

	p = pageName+ startChar + " EG sustain" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_EG1_SUSTAIN);

	p = pageName + startChar+ " EG release" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_EG1_RELEASE);



	p = pageName + startChar+ " EG restart" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_EG1_RESTART_ON_TRIGGER);

	p = pageName + startChar+ " EG shape" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_EG1_SHAPE_MULTI);

	p = pageName + startChar+ " EG unipolar" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_EG1_UNIPOLAR);



	p = pageName + startChar+ " note depth" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_NOTE_MOD_DEPTH);


	p = pageName + startChar+ " note destination" + endChar;
		addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_NOTE_DEST);


	p = pageName + startChar+ " note depth alternate" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_NOTE_MOD_DEPTH_ALTERNATE);

	p = pageName + startChar+ " note unipolar" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_NOTE_MOD_UNIPOLAR);


	p = pageName + startChar+ " velocity depth" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_VELOCITY_MOD_DEPTH);

	p = pageName + startChar+ " velocity destination" + endChar;
		addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_VELOCITY_DEST);


	p = pageName + startChar+ " velocity depth alternate" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_VELOCITY_MOD_DEPTH_ALTERNATE);

	p = pageName + startChar+ " velocity unipolar" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_VELOCITY_MOD_UNIPOLAR);







	p = pageName + startChar+ " MOD1 depth" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_MOD1_MOD_DEPTH);

	p = pageName + startChar+ " MOD1 destination" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_MOD1_DEST);

	p = pageName + startChar+ " MOD1 depth alternate" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_MOD1_MOD_DEPTH_ALTERNATE);

	p = pageName + startChar+ " MOD1 unipolar" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_MOD1_MOD_UNIPOLAR);


	p = pageName + startChar+ " MOD2 depth" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_MOD2_MOD_DEPTH);

	p = pageName + startChar+ " MOD2 destination" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_MOD2_DEST);

	p = pageName + startChar+ " MOD2 depth alternate" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_MOD2_MOD_DEPTH_ALTERNATE);

	p = pageName + startChar+ " MOD2 unipolar" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_MOD2_MOD_UNIPOLAR);



	p = pageName + startChar+ " MOD3depth" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_MOD3_MOD_DEPTH);

	p = pageName + startChar+ " MOD3 destination" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_MOD3_DEST);

	p = pageName + startChar+ " MOD3 depth alternate" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_MOD3_MOD_DEPTH_ALTERNATE);

	p = pageName + startChar+ " MOD3 unipolar" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_MOD3_MOD_UNIPOLAR);



	p = pageName + startChar+ " MOD4 depth" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_MOD4_MOD_DEPTH);

	p = pageName + startChar+ " MOD4 destination" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_MOD4_DEST);

	p = pageName + startChar+ " MOD4 destination" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_MOD4_DEST);

	p = pageName + startChar+ " MOD4 depth alternate" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_MOD4_MOD_DEPTH_ALTERNATE);

	p = pageName + startChar+ " MOD4 unipolar" + endChar;
	addPluginLocalParam(p, paramPage, NRPN_VALUE_PAGE_MOD4_MOD_UNIPOLAR);







}




void MotasEditPluginAudioProcessor::addPluginParam(String name, int paramPage, int param, int min , int max)
{
	MotasPluginParameter* m = new MotasPluginParameter(this, name.trim().removeCharacters(" "),
			name, paramPage, param, min, max, min,
			LAMBDA_FUNCTION_CALL
	);
	// store in the hashmap a key which is unique for each parameter
	hashMapParameters.set(paramPage * 128 + param, m);
	addParameter(m);



}

void MotasEditPluginAudioProcessor::addPluginLocalParam(String name, int paramPage, int param)
{

		if (paramPage == 0)
			return;
		bool vcoPage = false;

		bool isMasterPitchPage = false;
		if (paramPage == 3 || paramPage == 10 || paramPage == 17)
		{
			vcoPage = true;
		} else if (paramPage == 0)
		{
			isMasterPitchPage = true;
		}


	MotasPluginParameter* m = new MotasPluginParameter(this, name.trim().removeCharacters(" "),
				name, paramPage, param,
				ControllerChangeLocalGetMinValue(param, isMasterPitchPage),
				ControllerChangeLocalGetMaxValue(param, vcoPage),
				ControllerChangeLocalGetDefaultValue(param, vcoPage),

				LAMBDA_FUNCTION_CALL
		);
		// store in the hashmap a key which is unique for each parameter
		hashMapParameters.set(paramPage * 128 + param, m);
		addParameter(m);


}

void MotasEditPluginAudioProcessor::addPluginGlobalParam(String name, int param)
{

	name = String("GLOBAL:")+name;
		int paramPage = 0;

	MotasPluginParameter* m = new MotasPluginParameter(this, name.trim().removeCharacters(" "),
				name, paramPage, param,
				ControllerChangeGlobalGetMinValue(param),
				ControllerChangeGlobalGetMaxValue(param),
				ControllerChangeGlobalGetDefaultValue(param),
				LAMBDA_FUNCTION_CALL
		);
		// store in the hashmap a key which is unique for each parameter
		hashMapParameters.set(paramPage * 128 + param, m);
		addParameter(m);


}



MotasEditPluginAudioProcessor::~MotasEditPluginAudioProcessor()
{
	DBG("Destroy MotasEditPluginAudioProcessor");
	/*if (midiProcessor->HighResolutionTimer::isTimerRunning())
	{
		midiProcessor->HighResolutionTimer::stopTimer();
	}*/
	//jassert(1 == 0);

}



#pragma message (" make sure add '#define JUCE_ALSA_MIDI_NAME \"MotasEditPlugin\" to  AppConfig.h otherwise crashes on Linux")

//==============================================================================
const String MotasEditPluginAudioProcessor::getName() const
{
    return String("MotasEditPlugin"); //JucePlugin_Name;
}

bool MotasEditPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MotasEditPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MotasEditPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MotasEditPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MotasEditPluginAudioProcessor::getNumPrograms()
{
    return BANK_OF_PRESETS_LENGTH;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MotasEditPluginAudioProcessor::getCurrentProgram()
{
    return appCommon.getPatchNumber();
}

void MotasEditPluginAudioProcessor::setCurrentProgram (int index)
{
	if (index >= 0 && index < BANK_OF_PRESETS_LENGTH)
	{

		HoldingWindowComponent* hw = getActiveHoldingWindow();
		if (hw != nullptr && hw->mainComponent != nullptr)
		{
			MessageManagerLock mml (Thread::getCurrentThread());
			if (mml.lockWasGained())
			{
				 //..do some locked stuff here..
				 hw->mainComponent->setComboBoxIndex(index);
				DBG("Thread locked! ;) +++++++++++++++");
			} else
			{
				DBG("Thread could not be locked!-------------");
			}
		} else
		{
			midiProcessor->commandRequest(MIDIProcessor::COMMAND_REQUEST_PATCH_CHANGE, (uint8_t)  index);
		}

				/*
		appCommon.setPatchNumber(index);
		midiProcessor->commandRequest(MIDIProcessor::COMMAND_REQUEST_SEND_ACTIVE_PATCH, 0);
		HoldingWindowComponent* hw = getActiveHoldingWindow();
		if (hw != nullptr && hw->mainComponent != nullptr)
		{
			MessageManagerLock mml (Thread::getCurrentThread());
			if (mml.lockWasGained())
			{
				//..do some locked stuff here..
				 hw->mainComponent->changePatchFromDAW();
				DBG("Thread locked! ;) +++++++++++++++");
			} else
			{
				DBG("Thread could not be locked!-------------");
			}

		}*/
	}
}

const String MotasEditPluginAudioProcessor::getProgramName (int index)
{
	String name = {};
	if (index >= 0 && index < BANK_OF_PRESETS_LENGTH)
	{
		name = appCommon.getPatchName(index);
	}
    return name;
}

void MotasEditPluginAudioProcessor::changeProgramName (int index, const String& newName)
{

	appCommon.changePatchName(index, newName);

}

//==============================================================================
void MotasEditPluginAudioProcessor::prepareToPlay (double , int )
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..


}

void MotasEditPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.

	//this->midiProcessor->HighResolutionTimer::stopTimer();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MotasEditPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void MotasEditPluginAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessagesDAW)
{
 /*   ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
*/
	/*
	 *
	#include <atomic>  // C++ 2011

	std::atomic<float> level;

	level.store(); // from GUI
	float levelToUse = level.load(); // from audio thread



	Use
	std::atomic<Widget*> ... but complicated

	SO

	pair one store with one load in another thread

	Timur Doumler
	https://www.youtube.com/watch?v=boPEO2auJj4
	*/

	//*filt; // get the value




    /*************************
	 *
	 * MIDI data incoming from the DAW (e.g. from track data or Motas hardware MIDI input to DAW)
	 * If there are any NRPNs then they will ALREADY be set to go straight out to Motas.
	 * We just need to analyse them and update the UI and DAW parameters
	 *
	 ***************************/
#if PROCESS_MIDI_INCOMING_FROM_DAW_PROCESS_BLOCK == 1
	// if we have incoming MIDI data from the DAW
	// then send to UI
    if (!midiMessagesDAW.isEmpty())
    {

    	DBG(String("midiMessagesDAW " + String(midiMessagesDAW.getNumEvents())));

    	MidiBuffer::Iterator midi_buffer_iter(midiMessagesDAW);

		const uint8_t* mm;
		int numBytes;
		int samplePosition;

		// add the incoming midi data to the FIFO to send to the UI
		while (midi_buffer_iter.getNextEvent(mm, numBytes, samplePosition))
		{

			//DBG(String("GetNextEvent " + String(numBytes)));
			fifoMidiIn.addToFifo(mm, numBytes);
		}

		// calling notify MAY not be ok here.... as it may lock a mutex..
		// https://forum.juce.com/t/audioprocessor-processblock-and-thread-notify-is-it-lock-free/27048/3
		midiProcessor->notify();
    }

/*
    buffer.clear();
      MidiBuffer processedMidi;
      int time;
      int numBytes;
      int samplePosition;

      const uint8_t* mm;
      MidiMessage m;//Encapsulates a MIDI message.
      for (MidiBuffer::Iterator i (midiMessagesDAW); i.getNextEvent (mm, numBytes, samplePosition);)
      {
          if (m.isNoteOn())//Returns true if this message is a 'key-down' event.
          {
             // uint8 newVel = (uint8) noteOnVel; //unsigned int 8 bytes
             // m = MidiMessage::noteOn(m.getChannel(), m.getNoteNumber(), newVel);
          }
          else if (m.isNoteOff())// 'key-up'
          {
          }
          else if (m.isAftertouch())
          {
          }
          else if (m.isPitchWheel())
          {
          }
          processedMidi.addEvent (m, time);




          fifoMidiIn.addToFifo(mm, numBytes);


      }
      midiMessagesDAW.swapWith(processedMidi);
      midiProcessor->notify();*/

#endif



    /*************************
	 *
	 * *****DEPRECATED *********
	 * MIDI data generated from the DAW changing the parameters
	 *
	 ***************************/
/*
    // get all the parameter NRPN data created from incoming 'parameter' changes
    // and send these to the UI via the FIFO!
	int numBytesInBuffer = fifoMidiParameters.readAllFromFifo(midiParamBuffer);
	if (numBytesInBuffer > 0)
	{
		fifoMidiInFromDAWParameters.addToFifo(midiParamBuffer, numBytesInBuffer);
		int numBytes;
		int bytesRead = 0;
		int lastTime = midiMessagesDAW.getLastEventTime();
		// now add the Parameter change NRPNs to the outgoing MIDI stream to the DAW
		while (numBytesInBuffer > 0)
		{
			MidiMessage mm(midiParamBuffer + bytesRead, numBytesInBuffer, numBytes, 0, 0 , false);
			bytesRead += numBytes;
			midiMessagesDAW.addEvent(mm, ++lastTime);

			numBytesInBuffer -= numBytes;
		}
	}

*/






    midiMessagesDAW.clear(); // should we clear this? YES to clear input MIDI buffer
    /*
     *
     * The way it works is this buffer contains incoming MIDI data from DAW, and then
     * repurposed as output MIDI buffer
     *
     */
	/*************************
	 *
	 * MIDI data from the UI sending to the DAW
	 *
	 ***************************/
	// add MIDI data to the outgoing MIDI generated in the UI
#if SEND_MIDI_TO_DAW_PROCESS_BLOCK == 1


    /*
     *
     *
     * If one compiles in debug mode can run as plugin from Reaper (from command line)
     * and we see the debug output!
     * ~/Downloads/reaper647_linux_x86_64/reaper_linux_x86_64/REAPER$ ./reaper
     *
     *
     */

    //int numBytesInBufferOut = fifoMidiUI.readAllFromFifo(midiUIBuffer, 1);



    int numReadyInFifo = fifoMidiUI.getNumReady(); // find out how many MIDI bytes are in the FIFO. Remember, all bytes in the FIFO are 'complete' i.e. never 'half' a midi message
    int numBytesInBufferOut = 0;
    if (numReadyInFifo < 64)
    {
    	// only a small amount in the FIFO, so get it all
     	numBytesInBufferOut = fifoMidiUI.readAllFromFifo(midiUIBuffer, 1);
    } else
    {
    	// there is lots in the FIFO, which must surely include sysEx data

    	int numSamples = buffer.getNumSamples();
		  /*
		   *
		   * Setting the audio 'block size' directly sets the getNumSamples() returned
		   * It seems that the higher the block size the more infrequently this function is called
		   * which makes sense. We should scale our MIDI sysEx out amount with the block size
		   * so that Motas-6 can always cope with the data rate, and is not too slow
		   */

    	MIDIoutSampleCount += numSamples;

		// wait until it is larger than the largest SysEx packet, otherwise we cannot send anything!
		int32_t rate = (int32_t) midiProcessor->getMIDISendRate();


		/*
		 *
		 *
		 * If sample rate is 44kHz, then if 'numSamples' is 44 this function
		 * will be called every 1ms with 'numSamples' = 44. In this case we would want to
		 * shift out 1 MIDI byte if the sendRate was set to 1000
		 */

		numSamples = (MIDIoutSampleCount * rate ) / (4 * 44 * 1000); // somehow need factor of 4 slowdown here...

		/*
		 * 'Rate' is samples per second
		 * A 'rate' of 1000 means send a byte every millisecond
		 *
		 * Values 200000 (fastest) 200kBytes/second
					110000
					60500
					33275
					18301
					10065
		 *
		 *
		 */


		numSamples = jmin(SIZE_OF_DAW_MIDI_OUT_BUFFER, numSamples);

		numBytesInBufferOut = fifoMidiUI.readSomeFromFifo(midiUIBuffer, numSamples, false); // do a test read i.e. do not mark as read yet

		int bytesToRead = 0; // default is to read nothing
		for (int i = 0; i < numBytesInBufferOut; i++)
		{
			if (midiUIBuffer[i] == 0xF7)
			{
				// Find the LAST end of sysex in the buffer, and stop there so we only read-out
				// a full message, and not part of a new one
				bytesToRead = i + 1;
			}
		}

		numBytesInBufferOut = fifoMidiUI.readSomeFromFifo(midiUIBuffer, bytesToRead, true); // now read and mark as read/used

    }

	if (numBytesInBufferOut > 0) // we have something to send out to the DAW
	{
		DBG("Num bytes MIDI: "  + String(numBytesInBufferOut));
		int numBytes;
		int bytesRead = 0;
		int lastTime = midiMessagesDAW.getLastEventTime();
		// now add the Parameter change NRPNs to the outgoing MIDI stream
		while (numBytesInBufferOut > 0)
		{
			//DBG("Adding data from midiUIBuffer");
			MidiMessage mm(midiUIBuffer + bytesRead, numBytesInBufferOut, numBytes, 0, 0 , false);
			bytesRead += numBytes;
			midiMessagesDAW.addEvent(mm, ++lastTime);
			numBytesInBufferOut -= numBytes;
		}

		MIDIoutSampleCount = 0; // we sent something
	}


#endif


    buffer.clear(); // remove any audio data


}

//==============================================================================
bool MotasEditPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* MotasEditPluginAudioProcessor::createEditor()
{


	//const MessageManagerLock mm; // required since adding change listener is from another thread than the UI.. or something like that.
    //return new MotasEditPluginAudioProcessorEditor (*this);
	//this->removeAllChangeListeners();


	//Remember not to do anything silly like allowing your processor to keep a pointer to
	//the component that gets created - it could be deleted later without any warning,
	//which would make your pointer into a dangler. Use the getActiveEditor() method instead

	DBG(".... createEditor... ");

	//this->addChangeListener(holdingWindow->commsMIDI.get());
	return new HoldingWindowComponent (*this);
}

//==============================================================================
void MotasEditPluginAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

	// return the data stored in the plugin
	// i.e. all the current slider positions i.e. the patch buffer!


	DBG("Host is getting Plugin settings");
	std::unique_ptr<XmlElement> xml(appCommonPtr->getPluginSettingsXML());
	//XmlElement*  xml = appCommonPtr->getPluginSettingsXML();
	DBG("Got elements");
/*
	/// Enable to test the XML output generated
	File f("~/MotasPlugin.xml");
	xml->writeToFile(f, "MotasPlugin.xml");

*/
	this->copyXmlToBinary(*xml.get(), destData);

	//destData = appCommon.getEntirePluginState();


	DBG("Host is done getting Plugin settings");

}

void MotasEditPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
	DBG("Host is setting Plugin settings");
	//midiProcessor->updateEntirePluginState(data, sizeInBytes);
	//appCommon.updateEntirePluginState(data, sizeInBytes);

	//DBG("Host is setting Plugin settings");
	std::unique_ptr<XmlElement> xml = this->getXmlFromBinary(data, sizeInBytes);
	appCommonPtr->restorePluginState(xml.get());
	midiProcessor->updateMIDISettings();
	if (getActiveHoldingWindow() != nullptr)
	{

		MessageManagerLock mml (Thread::getCurrentThread());

		if (mml.lockWasGained())
		{

			//..do some locked stuff here..

			getActiveHoldingWindow()->updateGUISettings();
			DBG("Thread locked! ;) +++++++++++++++");
		}else
		{
			DBG("Thread could not be locked!-------------");
		}

	} else
	{
		DBG("getActiveHoldingWindow is NULL");
	}

	// send the active patch to Motas
	midiProcessor->commandRequest(MIDIProcessor::COMMAND_REQUEST_SEND_ACTIVE_PATCH, 0);



}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	DBG("Host is creating MotasEditPluginAudioProcessor");
    return new MotasEditPluginAudioProcessor();
}




HoldingWindowComponent* MotasEditPluginAudioProcessor::getActiveHoldingWindow()
{
	return dynamic_cast<HoldingWindowComponent*> (this->getActiveEditor());
}





String MotasEditPluginAudioProcessor::getReadableValue(int paramPage, int param,  int value,	int maxLength)
{

	ParameterData p(0);

	p.setPage(paramPage);
	p.setParameter(param);
	p.setPanelValue(value); // note set PANEL value here
	//String text = Utilities::getParameterDisplay(&p);

	String text =  p.getDisplayHumanText();


	return text.substring(0, maxLength);

}


