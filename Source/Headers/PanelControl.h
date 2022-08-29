
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

    PanelControl.h


  ==============================================================================
*/

#pragma once

#include "ApplicationCommon.h"
#include "CustomLook.h"
#include "ParameterData.h"
class MainComponent;
#include "CustomSlider.h"
//==============================================================================
/*
 *
 * This class is for objects that are either rotary knobs, control for modulation or LFO etc or just preset push button
*/
class PanelControl : public Slider::Listener, ComboBox::Listener, Button::Listener, MouseListener //, KeyListener
{


public:

	enum
	{
		BUTTON_TYPE_PARAM_NO_GROUP,
		BUTTON_TYPE_PARAM_OFFSET,
		BUTTON_TYPE_PARAM_MODULATION,
		BUTTON_TYPE_PRESET


	} ;



	enum
	{
		PANEL_TYPE_NOT_DEFINED,
		PANEL_TYPE_PARAM_KNOB,
		PANEL_TYPE_PARAM_KNOB_LEVEL,
		PANEL_TYPE_MODULATION_CONTROL,
		PANEL_TYPE_LFO_WAVE,
		PANEL_TYPE_SIMPLE,
		PANEL_TYPE_SIMPLE_LEVEL,
		PANEL_TYPE_PRESET = 10,
		PANEL_TYPE_OPTION_BUTTON = 20,
		PANEL_TYPE_LFO2_FREQ,
	} ;


	enum
	{
		PANEL_CONTROL_NONE,
		PANEL_CONTROL_MASTER_PITCH,
		PANEL_CONTROL_OSC1_LEVEL,
		PANEL_CONTROL_OSC1_PITCH,
		PANEL_CONTROL_OSC1_TRI,
		PANEL_CONTROL_OSC1_SAW,
		PANEL_CONTROL_OSC1_PWM,
		PANEL_CONTROL_OSC1_PWM_LEVEL,

		PANEL_CONTROL_OSC2_PHASE,
		PANEL_CONTROL_OSC2_LEVEL,
		PANEL_CONTROL_OSC2_PITCH,
		PANEL_CONTROL_OSC2_TRI,
		PANEL_CONTROL_OSC2_SAW,
		PANEL_CONTROL_OSC2_SQUARE,
		PANEL_CONTROL_OSC2_SUB,

		PANEL_CONTROL_OSC3_PHASE,
		PANEL_CONTROL_OSC3_LEVEL,
		PANEL_CONTROL_OSC3_PITCH,
		PANEL_CONTROL_OSC3_TRI,
		PANEL_CONTROL_OSC3_SAW,
		PANEL_CONTROL_OSC3_PWM,
		PANEL_CONTROL_OSC3_PWM_LEVEL,

		PANEL_CONTROL_NOISE,
		PANEL_CONTROL_MIX,
		PANEL_CONTROL_LPF1,
		PANEL_CONTROL_LPF1_RES,
		PANEL_CONTROL_LPF1_LEVEL,

		PANEL_CONTROL_HPF,
		PANEL_CONTROL_HPF_LEVEL,
		PANEL_CONTROL_EXTERN,
		PANEL_CONTROL_LPF2,
		PANEL_CONTROL_LPF2_RES ,
		PANEL_CONTROL_LPF2_LEVEL ,
		PANEL_CONTROL_OUTPUT,


		// ensure PRESETx in sequence!
		PANEL_CONTROL_PRESET1,
		PANEL_CONTROL_PRESET2,
		PANEL_CONTROL_PRESET3,
		PANEL_CONTROL_PRESET4,
		PANEL_CONTROL_PRESET5,



		PANEL_CONTROL_NOTE ,
		PANEL_CONTROL_VELOCITY ,
		PANEL_CONTROL_M1 ,
		PANEL_CONTROL_M2 ,
		PANEL_CONTROL_M3 ,
		PANEL_CONTROL_M4,

		PANEL_CONTROL_LFOX_WAVEFORM ,
		PANEL_CONTROL_LFOX_FREQ ,
		PANEL_CONTROL_LFOX_MOD_DEPTH,

		PANEL_CONTROL_LFO_WAVEFORM ,
		PANEL_CONTROL_LFO_FREQ ,
		PANEL_CONTROL_LFO_MOD_DEPTH,

		PANEL_CONTROL_EG_DELAY ,
		PANEL_CONTROL_EG_ATTACK,
		PANEL_CONTROL_EG_DECAY,
		PANEL_CONTROL_EG_SUSTAIN,
		PANEL_CONTROL_EG_RELEASE,
		PANEL_CONTROL_EG_MOD_DEPTH ,



		PANEL_CONTROL_PARAM_MAIN_VOLUME

	};



	enum
	{
		CONTROL_PAGE_SPECIFIC_BUT_UNDEFINED = -1,
		CONTROL_GLOBAL = 0,
		CONTROL_MASTER_PITCH  = 1,
		CONTROL_OSC1_LEVEL,
		CONTROL_OSC1_PITCH,
		CONTROL_OSC1_TRI,
		CONTROL_OSC1_SAW,
		CONTROL_OSC1_PWM,
		CONTROL_OSC1_PWM_LEVEL,

		CONTROL_OSC2_PHASE,
		CONTROL_OSC2_LEVEL,
		CONTROL_OSC2_PITCH,
		CONTROL_OSC2_TRI,
		CONTROL_OSC2_SAW,
		CONTROL_OSC2_SQUARE,
		CONTROL_OSC2_SUB,

		CONTROL_OSC3_PHASE,
		CONTROL_OSC3_LEVEL,
		CONTROL_OSC3_PITCH,
		CONTROL_OSC3_TRI,
		CONTROL_OSC3_SAW,
		CONTROL_OSC3_PWM,
		CONTROL_OSC3_PWM_LEVEL,

		CONTROL_NOISE,
		CONTROL_MIX,
		CONTROL_LPF1,
		CONTROL_LPF1_RES,
		CONTROL_LPF1_LEVEL,

		CONTROL_HPF,
		CONTROL_HPF_LEVEL,
		CONTROL_EXTERN,
		CONTROL_LPF2 = 31,
		CONTROL_LPF2_RES ,
		CONTROL_LPF2_LEVEL ,
		CONTROL_OUTPUT = 35,
		CONTROL_END_MARKER
	} ;

	enum
	{
		CONTROL_PARAM_OFFSET = 1,
		CONTROL_PARAM_EG_MOD_DEPTH = 5,
		CONTROL_PARAM_EG_DELAY = 10,
		CONTROL_PARAM_EG_ATTACK,
		CONTROL_PARAM_EG_DECAY,
		CONTROL_PARAM_EG_SUSTAIN,
		CONTROL_PARAM_EG_RELEASE,
		CONTROL_PARAM_LFOX_MOD_DEPTH = 20,
		CONTROL_PARAM_LFO_FREQ = 25,
		CONTROL_PARAM_LFO_MOD_DEPTH,
		CONTROL_PARAM_M1_LEVEL = 30,
		CONTROL_PARAM_M1_ALT_LEVEL,
		CONTROL_PARAM_M1_UNIPOLAR,


		CONTROL_PARAM_M2_LEVEL = 35,
		CONTROL_PARAM_M2_ALT_LEVEL,
		CONTROL_PARAM_M2_UNIPOLAR,

		CONTROL_PARAM_M3_LEVEL = 40,
		CONTROL_PARAM_M3_ALT_LEVEL,
		CONTROL_PARAM_M3_UNIPOLAR,

		CONTROL_PARAM_M4_LEVEL = 45,
		CONTROL_PARAM_M4_ALT_LEVEL,
		CONTROL_PARAM_M4_UNIPOLAR,

		CONTROL_PARAM_VELOCITY_LEVEL = 50,
		CONTROL_PARAM_VELOCITY_ALT_LEVEL,
		CONTROL_PARAM_VELOCITY_UNIPOLAR,

		CONTROL_PARAM_NOTE_LEVEL = 55,
		CONTROL_PARAM_NOTE_ALT_LEVEL,
		CONTROL_PARAM_NOTE_UNIPOLAR,

		CONTROL_PARAM_PAGE_OPTION1 = 64,
		CONTROL_PARAM_PAGE_OPTION2,


		CONTROL_PARAM_EG_SOURCE = 70,
		CONTROL_PARAM_EG_RETRIG,
		CONTROL_PARAM_EG_RESTART,
		CONTROL_PARAM_EG_SHAPE = 75,
		CONTROL_PARAM_EG_SHAPE_MULTI = 76,
		CONTROL_PARAM_EG_UNIPOLAR = 78,


		CONTROL_PARAM_LFO_SOURCE = 80,
		CONTROL_PARAM_LFO_WAVE = 85,
		CONTROL_PARAM_LFO_TRIGGER,
		CONTROL_PARAM_LFO_SINGLE_SHOT,
		CONTROL_PARAM_LFO_OUTPUT_MODE,
		CONTROL_PARAM_LFO2_PITCH_TRACK,

		CONTROL_PARAM_M1_DESTINATION = 90,
		CONTROL_PARAM_LFO2_LFOX_FREQ_MOD = 92,
		CONTROL_PARAM_LFO2_EG_FREQ_MOD,


		CONTROL_PARAM_M2_DESTINATION = 95,
		CONTROL_PARAM_M3_DESTINATION = 100,
		CONTROL_PARAM_M4_DESTINATION = 105,
		CONTROL_PARAM_VELOCITY_DESTINATION = 110,
		CONTROL_PARAM_NOTE_DESTINATION = 115,






		CONTROL_PARAM_END_MARKER

	} ;

	enum
	{
		CONTROL_GLOBAL_LFOX1_FREQ = 4,
		CONTROL_GLOBAL_LFOX2_FREQ,
		CONTROL_GLOBAL_LFOX3_FREQ,
		CONTROL_GLOBAL_LFOX4_FREQ,

		CONTROL_GLOBAL_EGX1_DELAY = 12,
		CONTROL_GLOBAL_EGX2_DELAY,
		CONTROL_GLOBAL_EGX3_DELAY,
		CONTROL_GLOBAL_EGX4_DELAY,

		CONTROL_GLOBAL_EGX1_ATTACK = 20,
		CONTROL_GLOBAL_EGX2_ATTACK,
		CONTROL_GLOBAL_EGX3_ATTACK,
		CONTROL_GLOBAL_EGX4_ATTACK,

		CONTROL_GLOBAL_EGX1_DECAY = 28,
		CONTROL_GLOBAL_EGX2_DECAY,
		CONTROL_GLOBAL_EGX3_DECAY,
		CONTROL_GLOBAL_EGX4_DECAY,

		CONTROL_GLOBAL_EGX1_SUSTAIN = 36,
		CONTROL_GLOBAL_EGX2_SUSTAIN,
		CONTROL_GLOBAL_EGX3_SUSTAIN,
		CONTROL_GLOBAL_EGX4_SUSTAIN,

		CONTROL_GLOBAL_EGX1_RELEASE = 44,
		CONTROL_GLOBAL_EGX2_RELEASE,
		CONTROL_GLOBAL_EGX3_RELEASE,
		CONTROL_GLOBAL_EGX4_RELEASE,


		CONTROL_GLOBAL_PORTAMENTO_MODE = 52,
		CONTROL_GLOBAL_PORTAMENTO_TIME ,
		CONTROL_GLOBAL_PORTAMENTO_RATE ,

		CONTROL_GLOBAL_M1_SOURCE = 64,
		CONTROL_GLOBAL_M2_SOURCE,
		CONTROL_GLOBAL_M3_SOURCE,
		CONTROL_GLOBAL_M4_SOURCE,

		CONTROL_GLOBAL_LFOX1_TRIGGER = 72,
		CONTROL_GLOBAL_LFOX2_TRIGGER,
		CONTROL_GLOBAL_LFOX3_TRIGGER,
		CONTROL_GLOBAL_LFOX4_TRIGGER,

		CONTROL_GLOBAL_LFOX1_SINGLE_SHOT = 76,
		CONTROL_GLOBAL_LFOX2_SINGLE_SHOT,
		CONTROL_GLOBAL_LFOX3_SINGLE_SHOT,
		CONTROL_GLOBAL_LFOX4_SINGLE_SHOT,

		CONTROL_GLOBAL_LFOX1_WAVEFORM = 80,
		CONTROL_GLOBAL_LFOX2_WAVEFORM,
		CONTROL_GLOBAL_LFOX3_WAVEFORM,
		CONTROL_GLOBAL_LFOX4_WAVEFORM,


		CONTROL_GLOBAL_LFO1_PITCH_TRACK = 84,
		CONTROL_GLOBAL_LFO2_PITCH_TRACK ,
		CONTROL_GLOBAL_LFO3_PITCH_TRACK ,
		CONTROL_GLOBAL_LFO4_PITCH_TRACK ,


		CONTROL_GLOBAL_EGX1_TRIGGER = 88,
		CONTROL_GLOBAL_EGX2_TRIGGER,
		CONTROL_GLOBAL_EGX3_TRIGGER,
		CONTROL_GLOBAL_EGX4_TRIGGER,

		CONTROL_GLOBAL_EGX1_RESTART = 96,
		CONTROL_GLOBAL_EGX2_RESTART,
		CONTROL_GLOBAL_EGX3_RESTART,
		CONTROL_GLOBAL_EGX4_RESTART,


		CONTROL_GLOBAL_EGX1_SHAPE = 104,
		CONTROL_GLOBAL_EGX2_SHAPE,
		CONTROL_GLOBAL_EGX3_SHAPE,
		CONTROL_GLOBAL_EGX4_SHAPE,


		CONTROL_GLOBAL_EGX1_UNIPOLAR = 110,
		CONTROL_GLOBAL_EGX2_UNIPOLAR,
		CONTROL_GLOBAL_EGX3_UNIPOLAR,
		CONTROL_GLOBAL_EGX4_UNIPOLAR,

		CONTROL_GLOBAL_EGX1_SHAPE_MULTI = 114,
		CONTROL_GLOBAL_EGX2_SHAPE_MULTI,
		CONTROL_GLOBAL_EGX3_SHAPE_MULTI,
		CONTROL_GLOBAL_EGX4_SHAPE_MULTI,


		CONTROL_GLOBAL_SET_NEW_NRPN_CHANGES_PAGE = 127,


		CONTROL_GLOBAL_END_MARKER

	} ;



	enum
	{
		PARAM_TYPE_MAIN_SLIDER,
		PARAM_TYPE_SECOND_SLIDER,
		PARAM_TYPE_THIRD_SLIDER,
		PARAM_TYPE_MAIN_COMBO,
		PARAM_TYPE_BUTTON,
		PARAM_TYPE_SECOND_COMBO
	};



	PanelControl(MainComponent* owner,  int controlPage,  Rectangle<int> rec);



	void addMainControls(int controlPage);


	void startDragChange (Slider* slider);
	void endDragChange (Slider* slider);
    void sliderValueChanged(Slider* s) override;



    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;
    void setLocation(Rectangle<int> r);
    int type; // what type of control is this e.g. parameter offset, lfo waveform
    int id;
    void setLook(LookAndFeel* laf);

    ParameterData paramDataMain{PARAM_TYPE_MAIN_SLIDER};
	ParameterData paramDataSecondSlider{PARAM_TYPE_SECOND_SLIDER};
	ParameterData paramDataThirdSlider{PARAM_TYPE_THIRD_SLIDER};
	ParameterData paramDataComboBox{PARAM_TYPE_MAIN_COMBO};
	ParameterData paramDataButton{PARAM_TYPE_BUTTON};
	ParameterData paramDataComboBoxSecondary{PARAM_TYPE_SECOND_COMBO};

	void disableParameterButton();
	void dClick(CustomSlider* s);

	void setSelected();
private:
	std::unique_ptr<DrawableButton> button;
	std::unique_ptr<Label> knobLabel;
	std::unique_ptr<CustomSlider> mainSlider;
	std::unique_ptr<CustomSlider> secondSlider;
	std::unique_ptr<CustomSlider> thirdSlider;

	std::unique_ptr<ComboBox> comboBox;
	std::unique_ptr<ComboBox> comboBoxSecondary;
	std::unique_ptr<Drawable> image;
	std::unique_ptr<DrawableButton> paramButton;

    void buttonClicked (Button*) override;

    void 	mouseDown (const MouseEvent &event) override;

    void mouseDoubleClick (const MouseEvent&) override;


   // bool keyPressed (const KeyPress& key, Component* originatingComponent) override;



    MainComponent* owner;
    Rectangle<int> rectangle;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PanelControl)
};
