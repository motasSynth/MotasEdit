
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

    PanelControl.cpp


  ==============================================================================
*/

#include "./Headers/PanelControl.h"
#include "./Headers/CustomLook.h"
#include "./Headers/images.h"
#include "./Headers/MainComponent.h"

#include "../binaryResources/MotasEditResources.h"
#include "Headers/MotasPluginParameter.h"
#include "Headers/HoldingWindowComponent.h"
#include "Headers/MIDIProcessor.h"
#include "Headers/CustomSlider.h"
//==============================================================================
/*
 *
 *  rec based on an area 10000 x 10000.
 *  Later, when actual rectangle of area supplied for redraw we scale to provided rectangle
 */

void PanelControl::addMainControls(int panelControlId)
{
	id = panelControlId - 1;

	type = PANEL_TYPE_NOT_DEFINED;
	int controlPage = CONTROL_PAGE_SPECIFIC_BUT_UNDEFINED;
	int controlParameter = CONTROL_PARAM_OFFSET; // default is offset

	const char* resourceDataImage = nullptr;
	unsigned int resourceDataSize = 0;
	const char* resourceDataImageHover = nullptr;
	int resourceDataImageHoverSize = 0;
	const char* resourceDataImageClick = nullptr;
	int resourceDataImageClickSize = 0;
	int32_t initialValue = 0;
	String mainSliderText = "?";

	switch (panelControlId)
		{
			default:
			case PanelControl::PANEL_CONTROL_NONE:
				//imageFileName = "";
				break;
			case PanelControl::PANEL_CONTROL_MASTER_PITCH:
				type = PanelControl::PANEL_TYPE_PARAM_KNOB;
				controlPage = PanelControl::CONTROL_MASTER_PITCH;
				resourceDataImage = MotasEditResources::masterPitch_svg;
				resourceDataSize = MotasEditResources::masterPitch_svgSize;
				resourceDataImageHover = MotasEditResources::masterPitch_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::masterPitch_hover_svgSize;
				resourceDataImageClick = MotasEditResources::masterPitch_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::masterPitch_clicked_svgSize;
				mainSliderText = "MASTER Pitch";
				break;
			case PanelControl::PANEL_CONTROL_OSC1_LEVEL:
				type = PanelControl::PANEL_TYPE_PARAM_KNOB_LEVEL;
				controlPage = PanelControl::CONTROL_OSC1_LEVEL;
				resourceDataImage = MotasEditResources::level_svg;
				resourceDataSize = MotasEditResources::level_svgSize;
				resourceDataImageHover = MotasEditResources::level_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::level_hover_svgSize;
				resourceDataImageClick = MotasEditResources::level_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::level_clicked_svgSize;
				mainSliderText = "OSC1 Level";

				break;
			case PanelControl::PANEL_CONTROL_OSC1_PITCH:
			{
				initialValue = 6000;
				type = PanelControl::PANEL_TYPE_PARAM_KNOB;
				controlPage = PanelControl::CONTROL_OSC1_PITCH;
				resourceDataImage = MotasEditResources::pitch_svg;
				resourceDataSize = MotasEditResources::pitch_svgSize;
				resourceDataImageHover = MotasEditResources::pitch_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::pitch_hover_svgSize;
				resourceDataImageClick = MotasEditResources::pitch_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::pitch_clicked_svgSize;
				mainSliderText = "OSC1 Pitch";

				comboBoxSecondary =  std::make_unique< ComboBox>();
		//		int i = 1;
				ParameterData::populateComboBox(comboBoxSecondary.get(), 100);
				/*
				comboBoxSecondary->addItem("LST", i++);
				comboBoxSecondary->addItem("1", i++);
				comboBoxSecondary->addItem("2", i++);
				comboBoxSecondary->addItem("3", i++);
				comboBoxSecondary->addItem("HI", i++);
				comboBoxSecondary->addItem("MID", i++);
				comboBoxSecondary->addItem("LOW", i++);
*/
				comboBoxSecondary->setTooltip("Set OSC1 note tracking");
				paramDataComboBoxSecondary.initialise(panelControlId,  controlPage,  PanelControl::CONTROL_PARAM_PAGE_OPTION2, "OSC1 pitch note track");
				comboBoxSecondary->setSelectedId(1, NotificationType::dontSendNotification);
				comboBoxSecondary->addListener(this);
				paramDataComboBoxSecondary.setComboBox(comboBoxSecondary.get());
				owner->addAndMakeVisible(*comboBoxSecondary);

				comboBox =  std::make_unique< ComboBox>();
			//	i = 1;
				ParameterData::populateComboBox(comboBox.get(), 1);
				/*
				comboBox->addItem("S:OFF", i++);
				comboBox->addItem("S:2", i++);
				comboBox->addItem("S:3", i++);
				comboBox->addItem("S:2+3", i++);
				 */
				comboBox->setTooltip("Set OSC1 hard-sync");


				paramDataComboBox.initialise(panelControlId,  controlPage,  PanelControl::CONTROL_PARAM_PAGE_OPTION1, "OSC1 pitch hard sync.");
				comboBox->setSelectedId(1, NotificationType::dontSendNotification);
				paramDataComboBox.setComboBox(comboBox.get());
				comboBox->addListener(this);
				owner->addAndMakeVisible(*comboBox);


			}
				break;
			case PanelControl::PANEL_CONTROL_OSC1_TRI:
				type = PanelControl::PANEL_TYPE_PARAM_KNOB;
				controlPage = PanelControl::CONTROL_OSC1_TRI;
				resourceDataImage = MotasEditResources::triangle_svg;
				resourceDataSize = MotasEditResources::triangle_svgSize;
				resourceDataImageHover = MotasEditResources::triangle_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::triangle_hover_svgSize;
				resourceDataImageClick = MotasEditResources::triangle_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::triangle_clicked_svgSize;
				mainSliderText = "OSC1 Triangle Level";
				break;
			case PanelControl::PANEL_CONTROL_OSC1_SAW:
				type = PanelControl::PANEL_TYPE_PARAM_KNOB;
				controlPage = PanelControl::CONTROL_OSC1_SAW;
				resourceDataImage = MotasEditResources::sawtooth_svg;
				resourceDataSize = MotasEditResources::sawtooth_svgSize;
				resourceDataImageHover = MotasEditResources::sawtooth_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::sawtooth_hover_svgSize;
				resourceDataImageClick = MotasEditResources::sawtooth_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::sawtooth_clicked_svgSize;
				mainSliderText = "OSC1 Sawtooth Level";
				break;
			case PanelControl::PANEL_CONTROL_OSC1_PWM:
				type = PanelControl::PANEL_TYPE_PARAM_KNOB;
				controlPage = PanelControl::CONTROL_OSC1_PWM;
				resourceDataImage = MotasEditResources::pwmText_svg;
				resourceDataSize = MotasEditResources::pwmText_svgSize;
				resourceDataImageHover = MotasEditResources::pwmText_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::pwmText_hover_svgSize;
				resourceDataImageClick = MotasEditResources::pwmText_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::pwmText_clicked_svgSize;
				mainSliderText = "OSC1 PWM";
				break;
			case PanelControl::PANEL_CONTROL_OSC1_PWM_LEVEL:
				type = PanelControl::PANEL_TYPE_PARAM_KNOB;
				controlPage = PanelControl::CONTROL_OSC1_PWM_LEVEL;

				resourceDataImage = MotasEditResources::pwm_svg;
				resourceDataSize = MotasEditResources::pwm_svgSize;
				resourceDataImageHover = MotasEditResources::pwm_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::pwm_hover_svgSize;
				resourceDataImageClick = MotasEditResources::pwm_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::pwm_clicked_svgSize;
				mainSliderText = "OSC1 PWM Level";
				break;
			case PanelControl::PANEL_CONTROL_OSC2_PHASE:
				type = PanelControl::PANEL_TYPE_PARAM_KNOB;
				controlPage = PanelControl::CONTROL_OSC2_PHASE;
				resourceDataImage = MotasEditResources::phaseMod_svg;
				resourceDataSize = MotasEditResources::phaseMod_svgSize;
				resourceDataImageHover = MotasEditResources::phaseMod_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::phaseMod_hover_svgSize;
				resourceDataImageClick = MotasEditResources::phaseMod_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::phaseMod_clicked_svgSize;
				mainSliderText = "OSC2 Phase Modulation Depth";
				break;
			case PanelControl::PANEL_CONTROL_OSC2_LEVEL:
				type = PanelControl::PANEL_TYPE_PARAM_KNOB_LEVEL;
				controlPage = PanelControl::CONTROL_OSC2_LEVEL;
				resourceDataImage = MotasEditResources::level_svg;
				resourceDataSize = MotasEditResources::level_svgSize;
				resourceDataImageHover = MotasEditResources::level_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::level_hover_svgSize;
				resourceDataImageClick = MotasEditResources::level_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::level_clicked_svgSize;
				mainSliderText = "OSC2 Level";
				break;
			case PanelControl::PANEL_CONTROL_OSC2_PITCH:
			{
				initialValue = 6000;
				type = PanelControl::PANEL_TYPE_PARAM_KNOB;
				controlPage = PanelControl::CONTROL_OSC2_PITCH;
				resourceDataImage = MotasEditResources::pitch_svg;
				resourceDataSize = MotasEditResources::pitch_svgSize;
				resourceDataImageHover = MotasEditResources::pitch_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::pitch_hover_svgSize;
				resourceDataImageClick = MotasEditResources::pitch_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::pitch_clicked_svgSize;
				comboBoxSecondary =  std::make_unique< ComboBox>();
				mainSliderText = "OSC2 Pitch";
			//	int i = 1;
				ParameterData::populateComboBox(comboBoxSecondary.get(), 0);
		/*		comboBoxSecondary->addItem("LST", i++);
				comboBoxSecondary->addItem("1", i++);
				comboBoxSecondary->addItem("2", i++);
				comboBoxSecondary->addItem("3", i++);
				comboBoxSecondary->addItem("HI", i++);
				comboBoxSecondary->addItem("MID", i++);
				comboBoxSecondary->addItem("LOW", i++);*/
				comboBoxSecondary->setTooltip("Set OSC2 note tracking");

				paramDataComboBoxSecondary.initialise(panelControlId,  controlPage,  PanelControl::CONTROL_PARAM_PAGE_OPTION2, "OSC2 pitch note track");
				comboBoxSecondary->setSelectedId(1, NotificationType::dontSendNotification);
				comboBoxSecondary->addListener(this);
				owner->addAndMakeVisible(*comboBoxSecondary);
				paramDataComboBoxSecondary.setComboBox(comboBoxSecondary.get());

				comboBox =  std::make_unique< ComboBox>();

		//		i = 1;
				ParameterData::populateComboBox(comboBox.get(), 2);
				/*
				comboBox->addItem("S:OFF", i++);
				comboBox->addItem("S:1", i++);
				comboBox->addItem("S:3", i++);
				comboBox->addItem("S:1+3", i++);
				*/
				comboBox->setTooltip("Set OSC2 hard-sync");

				paramDataComboBox.initialise(panelControlId,  controlPage,  PanelControl::CONTROL_PARAM_PAGE_OPTION1, "OSC2 pitch hard sync.");
				comboBox->setSelectedId(1, NotificationType::dontSendNotification);
				comboBox->addListener(this);
				paramDataComboBox.setComboBox(comboBox.get());
				owner->addAndMakeVisible(*comboBox);
			}
				break;
			case PanelControl::PANEL_CONTROL_OSC2_TRI:
				type = PanelControl::PANEL_TYPE_PARAM_KNOB;
				controlPage = PanelControl::CONTROL_OSC2_TRI;
				resourceDataImage = MotasEditResources::triangle_svg;
				resourceDataSize = MotasEditResources::triangle_svgSize;
				resourceDataImageHover = MotasEditResources::triangle_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::triangle_hover_svgSize;
				resourceDataImageClick = MotasEditResources::triangle_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::triangle_clicked_svgSize;
				mainSliderText = "OSC2 Triangle Level";
				break;
			case PanelControl::PANEL_CONTROL_OSC2_SAW:
				type = PanelControl::PANEL_TYPE_PARAM_KNOB;
				controlPage = PanelControl::CONTROL_OSC2_SAW;
				resourceDataImage = MotasEditResources::sawtooth_svg;
				resourceDataSize = MotasEditResources::sawtooth_svgSize;
				resourceDataImageHover = MotasEditResources::sawtooth_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::sawtooth_hover_svgSize;
				resourceDataImageClick = MotasEditResources::sawtooth_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::sawtooth_clicked_svgSize;
				mainSliderText = "OSC2 Sawtooth Level";
				break;
			case PanelControl::PANEL_CONTROL_OSC2_SQUARE:
				type = PanelControl::PANEL_TYPE_PARAM_KNOB;
				controlPage = PanelControl::CONTROL_OSC2_SQUARE;
				resourceDataImage = MotasEditResources::square_svg;
				resourceDataSize = MotasEditResources::square_svgSize;
				resourceDataImageHover =  MotasEditResources::square_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::square_hover_svgSize;
				resourceDataImageClick = MotasEditResources::square_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::square_clicked_svgSize;
				mainSliderText = "OSC1 Square Level";
				break;
			case PanelControl::PANEL_CONTROL_OSC2_SUB:
				type = PanelControl::PANEL_TYPE_PARAM_KNOB;
				controlPage = PanelControl::CONTROL_OSC2_SUB;
				resourceDataImage = MotasEditResources::sub_svg;
				resourceDataSize = MotasEditResources::sub_svgSize;
				resourceDataImageHover = MotasEditResources::sub_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::sub_hover_svgSize;
				resourceDataImageClick = MotasEditResources::sub_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::sub_clicked_svgSize;
				mainSliderText = "OSC1 Sub Level";
				break;
			case PanelControl::PANEL_CONTROL_OSC3_PHASE:
			{
				type = PanelControl::PANEL_TYPE_PARAM_KNOB;
				controlPage = PanelControl::CONTROL_OSC3_PHASE;
				resourceDataImage = MotasEditResources::phaseMod_svg;
				resourceDataSize = MotasEditResources::phaseMod_svgSize;
				resourceDataImageHover = MotasEditResources::phaseMod_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::phaseMod_hover_svgSize;
				resourceDataImageClick = MotasEditResources::phaseMod_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::phaseMod_clicked_svgSize;
				mainSliderText = "OSC3 Phase Modulation Depth";

				comboBox =  std::make_unique< ComboBox>();
				int i = 1;
				comboBox->addItem("OSC1", i++);
				comboBox->addItem("OSC2", i++);
				comboBox->setTooltip("Set OSC2 phase modulation source");

				paramDataComboBox.initialise(panelControlId,  controlPage,  PanelControl::CONTROL_PARAM_PAGE_OPTION1 , "OSC3 phase mod source");
				comboBox->setSelectedId(1, NotificationType::dontSendNotification);
				comboBox->addListener(this);
				paramDataComboBox.setComboBox(comboBox.get());
				owner->addAndMakeVisible(*comboBox);
				break;
			}
			case PanelControl::PANEL_CONTROL_OSC3_LEVEL:
				type = PanelControl::PANEL_TYPE_PARAM_KNOB_LEVEL;
				controlPage = PanelControl::CONTROL_OSC3_LEVEL;
				resourceDataImage = MotasEditResources::level_svg;
				resourceDataSize = MotasEditResources::level_svgSize;
				resourceDataImageHover = MotasEditResources::level_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::level_hover_svgSize;
				resourceDataImageClick = MotasEditResources::level_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::level_clicked_svgSize;
				mainSliderText = "OSC3 Level";
				break;
			case PanelControl::PANEL_CONTROL_OSC3_PITCH:
			{
				initialValue = 6000;
				type = PanelControl::PANEL_TYPE_PARAM_KNOB;
				controlPage = PanelControl::CONTROL_OSC3_PITCH;
				resourceDataImage = MotasEditResources::pitch_svg;
				resourceDataSize = MotasEditResources::pitch_svgSize;
				resourceDataImageHover = MotasEditResources::pitch_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::pitch_hover_svgSize;
				resourceDataImageClick = MotasEditResources::pitch_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::pitch_clicked_svgSize;

				mainSliderText = "OSC3 Pitch";
				comboBoxSecondary =  std::make_unique< ComboBox>();

				ParameterData::populateComboBox(comboBoxSecondary.get(), 0);
				/*
				comboBoxSecondary->addItem("LST", i++);
				comboBoxSecondary->addItem("1", i++);
				comboBoxSecondary->addItem("2", i++);
				comboBoxSecondary->addItem("3", i++);
				comboBoxSecondary->addItem("HI", i++);
				comboBoxSecondary->addItem("MID", i++);
				comboBoxSecondary->addItem("LOW", i++);
				*/
				comboBoxSecondary->setTooltip("Set OSC3 note tracking");


				paramDataComboBoxSecondary.initialise(panelControlId,  controlPage,  PanelControl::CONTROL_PARAM_PAGE_OPTION2, "OSC3 pitch note track.");
				paramDataComboBoxSecondary.setComboBox(comboBoxSecondary.get());
				comboBoxSecondary->setSelectedId(1, NotificationType::dontSendNotification);
				comboBoxSecondary->addListener(this);
				owner->addAndMakeVisible(*comboBoxSecondary);
				break;
			}
			case PanelControl::PANEL_CONTROL_OSC3_TRI:
				type = PanelControl::PANEL_TYPE_PARAM_KNOB;
				controlPage = PanelControl::CONTROL_OSC3_TRI;
				resourceDataImage = MotasEditResources::triangle_svg;
				resourceDataSize = MotasEditResources::triangle_svgSize;
				resourceDataImageHover = MotasEditResources::triangle_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::triangle_hover_svgSize;
				resourceDataImageClick = MotasEditResources::triangle_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::triangle_clicked_svgSize;
				mainSliderText = "OSC1 Triangle Level";
				break;
			case PanelControl::PANEL_CONTROL_OSC3_SAW:
				type = PanelControl::PANEL_TYPE_PARAM_KNOB;
				controlPage = PanelControl::CONTROL_OSC3_SAW;
				resourceDataImage = MotasEditResources::sawtooth_svg;
				resourceDataSize = MotasEditResources::sawtooth_svgSize;
				resourceDataImageHover = MotasEditResources::sawtooth_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::sawtooth_hover_svgSize;
				resourceDataImageClick = MotasEditResources::sawtooth_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::sawtooth_clicked_svgSize;
				mainSliderText = "OSC3 Sawtooth Level";
				break;
			case PanelControl::PANEL_CONTROL_OSC3_PWM:
				type = PanelControl::PANEL_TYPE_PARAM_KNOB;
				controlPage = PanelControl::CONTROL_OSC3_PWM;
				resourceDataImage = MotasEditResources::pwmText_svg;
				resourceDataSize = MotasEditResources::pwmText_svgSize;
				resourceDataImageHover = MotasEditResources::pwmText_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::pwmText_hover_svgSize;
				resourceDataImageClick = MotasEditResources::pwmText_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::pwmText_clicked_svgSize;
				mainSliderText = "OSC3 PWM";
				break;
			case PanelControl::PANEL_CONTROL_OSC3_PWM_LEVEL:
				type = PanelControl::PANEL_TYPE_PARAM_KNOB;
				controlPage = PanelControl::CONTROL_OSC3_PWM_LEVEL;
				resourceDataImage = MotasEditResources::pwm_svg;
				resourceDataSize = MotasEditResources::pwm_svgSize;
				resourceDataImageHover = MotasEditResources::pwm_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::pwm_hover_svgSize;
				resourceDataImageClick = MotasEditResources::pwm_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::pwm_clicked_svgSize;
				mainSliderText = "OSC3 PWM Level";
				break;
			case PanelControl::PANEL_CONTROL_NOISE:
			{
				type = PanelControl::PANEL_TYPE_PARAM_KNOB;
				controlPage = PanelControl::CONTROL_NOISE;
				resourceDataImage = MotasEditResources::noise_svg;
				resourceDataSize = MotasEditResources::noise_svgSize;
				resourceDataImageHover = MotasEditResources::noise_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::noise_hover_svgSize;
				resourceDataImageClick = MotasEditResources::noise_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::noise_clicked_svgSize;
				mainSliderText = "NOISE Level";
				comboBox =  std::make_unique< ComboBox>();

				ParameterData::populateComboBox(comboBox.get(), 3);

				/*
				comboBox->addItem("WHITE", i++);
				comboBox->addItem("PINK", i++);
				*/
				comboBox->setTooltip("Set noise character");


				paramDataComboBox.initialise(panelControlId,  controlPage,  PanelControl::CONTROL_PARAM_PAGE_OPTION1, "NOISE option");
				paramDataComboBox.setComboBox(comboBox.get());
				comboBox->setSelectedId(1, NotificationType::dontSendNotification);
				comboBox->addListener(this);
				owner->addAndMakeVisible(*comboBox);
				break;
			}
			case PanelControl::PANEL_CONTROL_MIX:
			{
				mainSliderText = "MIX Level";
				type = PanelControl::PANEL_TYPE_PARAM_KNOB_LEVEL;
				controlPage = PanelControl::CONTROL_MIX;
				resourceDataImage = MotasEditResources::level_svg;
				resourceDataSize = MotasEditResources::level_svgSize;
				resourceDataImageHover = MotasEditResources::level_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::level_hover_svgSize;
				resourceDataImageClick = MotasEditResources::level_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::level_clicked_svgSize;

				comboBox =  std::make_unique< ComboBox>();

				ParameterData::populateComboBox(comboBox.get(), 4);
/*
				comboBox->addItem("NORMAL", i++);
				comboBox->addItem("BOOST", i++);
				*/
				comboBox->setTooltip("Set mixer gain");


				paramDataComboBox.initialise(panelControlId,  controlPage,  PanelControl::CONTROL_PARAM_PAGE_OPTION1, "MIXER gain");
				comboBox->setSelectedId(1, NotificationType::dontSendNotification);
				paramDataComboBox.setComboBox(comboBox.get());
				comboBox->addListener(this);
				owner->addAndMakeVisible(*comboBox);

				break;
			}
			case PanelControl::PANEL_CONTROL_LPF1:
			{
				type = PanelControl::PANEL_TYPE_PARAM_KNOB;
				controlPage = PanelControl::CONTROL_LPF1;
				resourceDataImage = MotasEditResources::lpf_svg;
				resourceDataSize = MotasEditResources::lpf_svgSize;
				resourceDataImageHover = MotasEditResources::lpf_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::lpf_hover_svgSize;
				resourceDataImageClick = MotasEditResources::lpf_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::lpf_clicked_svgSize;
				mainSliderText = "LPF1 Frequency";
				comboBox =  std::make_unique< ComboBox>();

				ParameterData::populateComboBox(comboBox.get(), 5);
/*
				comboBox->addItem("MIX", i++);
				comboBox->addItem("LPF2", i++);
				comboBox->addItem("HPF", i++);
				comboBox->addItem("NOISE", i++);
				comboBox->addItem("PULS1", i++);
				comboBox->addItem("SAW2", i++);
				comboBox->addItem("TRI3", i++);
				comboBox->addItem("SUB2", i++);
				*/
				comboBox->setTooltip("Set input source for Low-pass filter 1");



				paramDataComboBox.initialise(panelControlId,  controlPage,  PanelControl::CONTROL_PARAM_PAGE_OPTION1, "LPF1 input source");
				comboBox->setSelectedId(1, NotificationType::dontSendNotification);
				paramDataComboBox.setComboBox(comboBox.get());
				comboBox->addListener(this);
				owner->addAndMakeVisible(*comboBox);
				break;
			}
			case PanelControl::PANEL_CONTROL_LPF1_RES:
			{
				type = PanelControl::PANEL_TYPE_PARAM_KNOB;
				controlPage = PanelControl::CONTROL_LPF1_RES;
				resourceDataImage = MotasEditResources::resonance_svg;
				resourceDataSize = MotasEditResources::resonance_svgSize;
				resourceDataImageHover = MotasEditResources::resonance_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::resonance_hover_svgSize;
				resourceDataImageClick = MotasEditResources::resonance_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::resonance_clicked_svgSize;
				comboBox =  std::make_unique< ComboBox>();

				ParameterData::populateComboBox(comboBox.get(), 6);
				/*
				comboBox->addItem("POLE-4", i++);
				comboBox->addItem("POLE-6", i++);
				comboBox->addItem("HPF", i++);*/
				mainSliderText = "LPF1 Resonance Amount";
				comboBox->setTooltip("Set resonance feedback source for Low-pass filter 1");


				paramDataComboBox.initialise(panelControlId,  controlPage,  PanelControl::CONTROL_PARAM_PAGE_OPTION1, "LPF1 resonance source");
				comboBox->setSelectedId(1, NotificationType::dontSendNotification);
				paramDataComboBox.setComboBox(comboBox.get());
				comboBox->addListener(this);
				owner->addAndMakeVisible(*comboBox);
				break;

			}
			case PanelControl::PANEL_CONTROL_LPF1_LEVEL:
			{
				type = PanelControl::PANEL_TYPE_PARAM_KNOB_LEVEL;
				controlPage = PanelControl::CONTROL_LPF1_LEVEL;
				resourceDataImage = MotasEditResources::level_svg;
				resourceDataSize = MotasEditResources::level_svgSize;
				resourceDataImageHover = MotasEditResources::level_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::level_hover_svgSize;
				resourceDataImageClick = MotasEditResources::level_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::level_clicked_svgSize;
				comboBox =  std::make_unique< ComboBox>();
				mainSliderText = "LPF1 Output Level";


				ParameterData::populateComboBox(comboBox.get(), 7);
				/*
				comboBox->addItem("BYPASS", i++);
				comboBox->addItem("POLE-1", i++);
				comboBox->addItem("POLE-2", i++);
				comboBox->addItem("POLE-3", i++);
				comboBox->addItem("POLE-4", i++);
				comboBox->addItem("POLE-5", i++);
				comboBox->addItem("POLE-6", i++);
				*/
				comboBox->setTooltip("Set output for Low-pass filter 1");


				paramDataComboBox.initialise(panelControlId,  controlPage,  PanelControl::CONTROL_PARAM_PAGE_OPTION1, "LPF1 output");
				comboBox->setSelectedId(1, NotificationType::dontSendNotification);
				paramDataComboBox.setComboBox(comboBox.get());
				comboBox->addListener(this);
				owner->addAndMakeVisible(*comboBox);
				break;
			}
			case PanelControl::PANEL_CONTROL_HPF:
			{
				mainSliderText = "HPF Frequency";

				type = PanelControl::PANEL_TYPE_PARAM_KNOB;
				controlPage = PanelControl::CONTROL_HPF;
				resourceDataImage = MotasEditResources::hpf_svg;
				resourceDataSize = MotasEditResources::hpf_svgSize;
				resourceDataImageHover = MotasEditResources::hpf_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::hpf_hover_svgSize;
				resourceDataImageClick = MotasEditResources::hpf_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::hpf_clicked_svgSize;
				comboBox =  std::make_unique< ComboBox>();

				ParameterData::populateComboBox(comboBox.get(), 8);
				/*
				comboBox->addItem("MIX", i++);
				comboBox->addItem("POLE-1", i++);
				comboBox->addItem("POLE-2", i++);
				comboBox->addItem("POLE-4", i++);
				comboBox->addItem("POLE-5", i++);
				comboBox->addItem("POLE-6", i++);
				comboBox->addItem("NOISE", i++);
				comboBox->addItem("LPF2", i++);*/
				comboBox->setTooltip("Set input source for High-pass filter");



				paramDataComboBox.initialise(panelControlId,  controlPage,  PanelControl::CONTROL_PARAM_PAGE_OPTION1, "HPF input");
				comboBox->setSelectedId(1, NotificationType::dontSendNotification);
				paramDataComboBox.setComboBox(comboBox.get());
				comboBox->addListener(this);
				owner->addAndMakeVisible(*comboBox);
				break;
			}
			case PanelControl::PANEL_CONTROL_HPF_LEVEL:
			{
				mainSliderText = "HPF Output Level";

				type = PanelControl::PANEL_TYPE_PARAM_KNOB_LEVEL;
				controlPage = PanelControl::CONTROL_HPF_LEVEL;
				resourceDataImage = MotasEditResources::level_svg;
				resourceDataSize = MotasEditResources::level_svgSize;
				resourceDataImageHover = MotasEditResources::level_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::level_hover_svgSize;
				resourceDataImageClick = MotasEditResources::level_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::level_clicked_svgSize;
				break;
			}
			case PanelControl::PANEL_CONTROL_EXTERN:
				mainSliderText = "FEEDBACK/EXTERN Input Level";

				type = PanelControl::PANEL_TYPE_PARAM_KNOB;
				controlPage = PanelControl::CONTROL_EXTERN;
				resourceDataImage = MotasEditResources::extern_svg;
				resourceDataSize = MotasEditResources::extern_svgSize;
				resourceDataImageHover = MotasEditResources::extern_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::extern_hover_svgSize;
				resourceDataImageClick = MotasEditResources::extern_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::extern_clicked_svgSize;
				break;
			case PanelControl::PANEL_CONTROL_LPF2:
			{
				mainSliderText = "LPF2 Frequency";

				type = PanelControl::PANEL_TYPE_PARAM_KNOB;
				controlPage = PanelControl::CONTROL_LPF2;
				resourceDataImage = MotasEditResources::lpf_svg;
				resourceDataSize = MotasEditResources::lpf_svgSize;
				resourceDataImageHover = MotasEditResources::lpf_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::lpf_hover_svgSize;
				resourceDataImageClick = MotasEditResources::lpf_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::lpf_clicked_svgSize;
				comboBox =  std::make_unique< ComboBox>();

				ParameterData::populateComboBox(comboBox.get(), 9);
				/*
				comboBox->addItem("MIX", i++);
				comboBox->addItem("POLE-1", i++);
				comboBox->addItem("POLE-2", i++);
				comboBox->addItem("POLE-4", i++);
				comboBox->addItem("POLE-5", i++);
				comboBox->addItem("POLE-6", i++);
				comboBox->addItem("NOISE", i++);
				comboBox->addItem("HPF", i++);
				*/
				comboBox->setTooltip("Set input source for Low-pass filter 2");



				paramDataComboBox.initialise(panelControlId,  controlPage,  PanelControl::CONTROL_PARAM_PAGE_OPTION1,  "LPF2 input source");
				comboBox->setSelectedId(1, NotificationType::dontSendNotification);
				paramDataComboBox.setComboBox(comboBox.get());
				comboBox->addListener(this);
				owner->addAndMakeVisible(*comboBox);
				break;
			}
			case PanelControl::PANEL_CONTROL_LPF2_RES:
				mainSliderText = "LPF2 Resonance";

				type = PanelControl::PANEL_TYPE_PARAM_KNOB;
				controlPage = PanelControl::CONTROL_LPF2_RES;
				resourceDataImage = MotasEditResources::resonance_svg;
				resourceDataSize = MotasEditResources::resonance_svgSize;
				resourceDataImageHover = MotasEditResources::resonance_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::resonance_hover_svgSize;
				resourceDataImageClick = MotasEditResources::resonance_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::resonance_clicked_svgSize;
				break;
			case PanelControl::PANEL_CONTROL_LPF2_LEVEL:
				mainSliderText = "LPF2 Output Level";

				type = PanelControl::PANEL_TYPE_PARAM_KNOB_LEVEL;
				controlPage = PanelControl::CONTROL_LPF2_LEVEL;
				resourceDataImage = MotasEditResources::level_svg;
				resourceDataSize = MotasEditResources::level_svgSize;
				resourceDataImageHover = MotasEditResources::level_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::level_hover_svgSize;
				resourceDataImageClick = MotasEditResources::level_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::level_clicked_svgSize;
				break;
			case PanelControl::PANEL_CONTROL_OUTPUT:
			{
				mainSliderText = "OUTPUT Level";

				type = PanelControl::PANEL_TYPE_PARAM_KNOB_LEVEL;
				controlPage = PanelControl::CONTROL_OUTPUT;
				resourceDataImage = MotasEditResources::level_svg;
				resourceDataSize = MotasEditResources::level_svgSize;
				resourceDataImageHover = MotasEditResources::level_hover_svg;
				resourceDataImageHoverSize = MotasEditResources::level_hover_svgSize;
				resourceDataImageClick = MotasEditResources::level_clicked_svg;
				resourceDataImageClickSize = MotasEditResources::level_clicked_svgSize;
				comboBox =  std::make_unique< ComboBox>();

				ParameterData::populateComboBox(comboBox.get(), 10);
				/*
				comboBox->addItem("DRY", i++);
				comboBox->addItem("SOFT CLIP", i++);
				comboBox->addItem("DRY+SOFT CLIP", i++);
				comboBox->addItem("HARD CLIP", i++);
				comboBox->addItem("DRY+HARD CLIP", i++);
				comboBox->addItem("SOFT+HARD CLIP", i++);
				comboBox->addItem("DRY+SOFT+HARD", i++);
				*/
				comboBox->setTooltip("Set output clipping options");




				paramDataComboBox.initialise(panelControlId,  controlPage,  PanelControl::CONTROL_PARAM_PAGE_OPTION1, "OUTPUT clipping options");
				comboBox->setSelectedId(1, NotificationType::dontSendNotification);
				paramDataComboBox.setComboBox(comboBox.get());
				comboBox->addListener(this);
				owner->addAndMakeVisible(*comboBox);
				break;
			}
		}

	if (type != PANEL_TYPE_NOT_DEFINED)
	{
		this->paramDataMain.initialise(panelControlId, controlPage, controlParameter, mainSliderText);



		image = Drawable::createFromImageData(resourceDataImage, resourceDataSize);

		if (image == nullptr)
		{
			DrawableText* t = new DrawableText();
			t->setText("Image N/A!");
			image.reset(t);
		}
		button = std::make_unique<DrawableButton>("button", DrawableButton::ImageFitted);

		button->addListener(this);

		button->setClickingTogglesState(true);

		if (panelControlId ==  PanelControl::PANEL_CONTROL_MASTER_PITCH || panelControlId == PanelControl::PANEL_CONTROL_PRESET1)
		{
			button->setToggleState(true, NotificationType::dontSendNotification);
		}
		std::unique_ptr<Drawable> hoverImage = nullptr;
		std::unique_ptr<Drawable> clickImage = nullptr;
		//if (panelControlId  <= PanelControl::PANEL_CONTROL_PRESET5)
		{
			hoverImage = Drawable::createFromImageData(resourceDataImageHover, (size_t) resourceDataImageHoverSize);
			clickImage = Drawable::createFromImageData(resourceDataImageClick, (size_t) resourceDataImageClickSize);
		}
		if (image != nullptr)
		{

			button->setImages(image.get(), hoverImage.get(), clickImage.get(), nullptr, clickImage.get(), nullptr , nullptr, nullptr);
			owner->addAndMakeVisible(*button);
		}
		if (type == PANEL_TYPE_PARAM_KNOB || type == PANEL_TYPE_PARAM_KNOB_LEVEL)
		{
			button->setRadioGroupId(BUTTON_TYPE_PARAM_OFFSET, NotificationType::dontSendNotification);
		} else if (type == PANEL_TYPE_OPTION_BUTTON)
		{
			DBG("Button");
			button->setRadioGroupId(BUTTON_TYPE_PARAM_NO_GROUP, NotificationType::dontSendNotification);
		} else if (type == PANEL_TYPE_PRESET)
		{
			button->setRadioGroupId(BUTTON_TYPE_PRESET, NotificationType::dontSendNotification);
		}
		else
		{
			button->setRadioGroupId(BUTTON_TYPE_PARAM_MODULATION, NotificationType::dontSendNotification);
		}

		switch (type)
		{
			default:
				break;
			case PANEL_TYPE_PARAM_KNOB: // rotary parameter selection
			case PANEL_TYPE_PARAM_KNOB_LEVEL: // red one with level
			{
				//mainSlider = new CustomSlider( Slider::RotaryHorizontalVerticalDrag, Slider::NoTextBox);
				mainSlider = std::make_unique<CustomSlider>(button.get(), this);

				Slider* s = mainSlider.get();
				mainSlider->onDragStart  = [this, s] { startDragChange (s); };
				mainSlider->onDragEnd     = [this, s] { endDragChange (s); };


				mainSlider->addListener(this);
				mainSlider->setPopupMenuEnabled(true);
				mainSlider->setRange(paramDataMain.getMin(), paramDataMain.getMax(), 1);

				mainSlider->setTooltip(mainSliderText);

				owner->addAndMakeVisible(mainSlider.get(), -1);


				mainSlider->setValue(initialValue, NotificationType::dontSendNotification);

				this->paramDataMain.isParameterPageFixed = true;
				paramDataMain.setSlider(mainSlider.get());

				if (comboBox != nullptr)
				{
					comboBox->addMouseListener(this, true);
					paramDataComboBox.isParameterPageFixed = true;
				}
				if (comboBoxSecondary != nullptr)
				{
					comboBoxSecondary->addMouseListener(this, true);
					paramDataComboBoxSecondary.isParameterPageFixed = true;
				}

				//mainSlider->setSliderSnapsToMousePosition(false);
				mainSlider->setVelocityModeParameters(1, 0, 0, true, ModifierKeys::shiftModifier);
				break;
			}


		}

		if (comboBox != nullptr)
			comboBox->addMouseListener(this, true);
		if (comboBoxSecondary != nullptr)
			comboBoxSecondary->addMouseListener(this, true);

	}

}


PanelControl::PanelControl(
		MainComponent* owner, int panelControlId, Rectangle<int> rec)
: rectangle (rec)

{

	this->owner = owner;
	button = nullptr;
	knobLabel = nullptr;
	mainSlider = nullptr;
	secondSlider = nullptr;
	thirdSlider = nullptr;
	image = nullptr;
	comboBox = nullptr;
	paramButton = nullptr;
	comboBoxSecondary = nullptr;

	//String imageFileName = "name";

	int controlPage = CONTROL_PAGE_SPECIFIC_BUT_UNDEFINED;
	int controlParameter = CONTROL_PARAM_OFFSET; // default is offset

	const char* resourceDataImage = nullptr;
	unsigned int resourceDataSize = 0;
	const char* resourceDataImageHover = nullptr;
	int resourceDataImageHoverSize = 0;
	const char* resourceDataImageClick = nullptr;
	int resourceDataImageClickSize = 0;
	//int32_t initialValue = 0;
	String mainSliderText = "?";


	this->addMainControls(panelControlId);
	if (type != PANEL_TYPE_NOT_DEFINED)
		return;
	switch (panelControlId)
	{
		default:
			break;
		case PanelControl::PANEL_CONTROL_NOTE:
			type = PanelControl::PANEL_TYPE_MODULATION_CONTROL;
			controlParameter = PanelControl::CONTROL_PARAM_NOTE_LEVEL;
			resourceDataImage = MotasEditResources::note_svg;
			resourceDataSize = MotasEditResources::note_svgSize;
			resourceDataImageHover = MotasEditResources::note_hover_svg;
			resourceDataImageHoverSize = MotasEditResources::note_hover_svgSize;
			resourceDataImageClick = MotasEditResources::note_clicked_svg;
			resourceDataImageClickSize = MotasEditResources::note_clicked_svgSize;
			mainSliderText = "Note Modulation Amount";

			break;
		case PanelControl::PANEL_CONTROL_VELOCITY:
			type = PanelControl::PANEL_TYPE_MODULATION_CONTROL;
			controlParameter = PanelControl::CONTROL_PARAM_VELOCITY_LEVEL;
			resourceDataImage = MotasEditResources::velocity_svg;
			resourceDataSize = MotasEditResources::velocity_svgSize;
			resourceDataImageHover = MotasEditResources::velocity_hover_svg;
			resourceDataImageHoverSize = MotasEditResources::velocity_hover_svgSize;
			resourceDataImageClick = MotasEditResources::velocity_clicked_svg;
			resourceDataImageClickSize = MotasEditResources::velocity_clicked_svgSize;
			mainSliderText = "Velocity Modulation Amount";

			break;
		case PanelControl::PANEL_CONTROL_M1:
			type = PanelControl::PANEL_TYPE_MODULATION_CONTROL;
			controlParameter = PanelControl::CONTROL_PARAM_M1_LEVEL;
			resourceDataImage = MotasEditResources::m1_svg;
			resourceDataSize = MotasEditResources::m1_svgSize;
			resourceDataImageHover = MotasEditResources::m1_hover_svg;
			resourceDataImageHoverSize = MotasEditResources::m1_hover_svgSize;
			resourceDataImageClick = MotasEditResources::m1_clicked_svg;
			resourceDataImageClickSize = MotasEditResources::m1_clicked_svgSize;
			mainSliderText = "M1 Modulation Amount";

			break;
		case PanelControl::PANEL_CONTROL_M2:
			type = PanelControl::PANEL_TYPE_MODULATION_CONTROL;
			controlParameter = PanelControl::CONTROL_PARAM_M2_LEVEL;
			resourceDataImage = MotasEditResources::m2_svg;
			resourceDataSize = MotasEditResources::m2_svgSize;
			resourceDataImageHover = MotasEditResources::m2_hover_svg;
			resourceDataImageHoverSize = MotasEditResources::m2_hover_svgSize;
			resourceDataImageClick = MotasEditResources::m2_clicked_svg;
			resourceDataImageClickSize = MotasEditResources::m2_clicked_svgSize;
			mainSliderText = "M2 Modulation Amount";

			break;
		case PanelControl::PANEL_CONTROL_M3:
			type = PanelControl::PANEL_TYPE_MODULATION_CONTROL;
			controlParameter = PanelControl::CONTROL_PARAM_M3_LEVEL;
			resourceDataImage = MotasEditResources::m3_svg;
			resourceDataSize = MotasEditResources::m3_svgSize;
			resourceDataImageHover = MotasEditResources::m3_hover_svg;
			resourceDataImageHoverSize = MotasEditResources::m3_hover_svgSize;
			resourceDataImageClick = MotasEditResources::m3_clicked_svg;
			resourceDataImageClickSize = MotasEditResources::m3_clicked_svgSize;
			mainSliderText = "M3 Modulation Amount";

			break;
		case PanelControl::PANEL_CONTROL_M4:
			type = PanelControl::PANEL_TYPE_MODULATION_CONTROL;
			controlParameter = PanelControl::CONTROL_PARAM_M4_LEVEL;
			resourceDataImage = MotasEditResources::m4_svg;
			resourceDataSize = MotasEditResources::m4_svgSize;
			resourceDataImageHover = MotasEditResources::m4_hover_svg;
			resourceDataImageHoverSize = MotasEditResources::m4_hover_svgSize;
			resourceDataImageClick = MotasEditResources::m4_clicked_svg;
			resourceDataImageClickSize = MotasEditResources::m4_clicked_svgSize;
			mainSliderText = "M4 Modulation Amount";

			break;

		case PanelControl::PANEL_CONTROL_LFOX_WAVEFORM:
		{
			type = PanelControl::PANEL_TYPE_LFO_WAVE;

			mainSliderText = "LFOx Waveform";

			resourceDataImage = MotasEditResources::lfo_svg;
			resourceDataSize = MotasEditResources::lfo_svgSize;
			resourceDataImageHover = MotasEditResources::lfo_hover_svg;
			resourceDataImageHoverSize = MotasEditResources::lfo_hover_svgSize;
			resourceDataImageClick = MotasEditResources::lfo_clicked_svg;
			resourceDataImageClickSize = MotasEditResources::lfo_clicked_svgSize;

			image = Drawable::createFromImageData(MotasEditResources::s_svg, MotasEditResources::s_svgSize);

			if (image == nullptr)
			{
				DrawableText* t = new DrawableText();
				t->setText("Image N/A!");
				image.reset(t);
			}

			std::unique_ptr<Drawable> clickImage = Drawable::createFromImageData(MotasEditResources::s_clicked_svg, MotasEditResources::s_clicked_svgSize);
			std::unique_ptr<Drawable> hoverImage = Drawable::createFromImageData(MotasEditResources::s_hover_svg, MotasEditResources::s_hover_svgSize);;

			paramButton = std::make_unique<DrawableButton>("s", DrawableButton::ImageFitted);
			paramButton->setTooltip("LFOx single-shot on/off");
			paramButton->setImages(image.get(), hoverImage.get(), clickImage.get(), nullptr, clickImage.get(), nullptr , nullptr, nullptr);
			paramDataButton.initialise(panelControlId, CONTROL_PAGE_SPECIFIC_BUT_UNDEFINED,  PanelControl::CONTROL_PARAM_LFO_SINGLE_SHOT, "LFOx single shot");
			paramButton->addListener(this);
			paramDataButton.setButton(paramButton.get());
			owner->addAndMakeVisible(*paramButton);
			paramButton->setClickingTogglesState(true);



			break;
		}
		case PanelControl::PANEL_CONTROL_LFOX_FREQ:
		{
			mainSliderText = "LFOx Frequency";

			type = PanelControl::PANEL_TYPE_SIMPLE;
			resourceDataImage = MotasEditResources::frequency_svg;
			resourceDataSize = MotasEditResources::frequency_svgSize;
			resourceDataImageHover = MotasEditResources::frequency_hover_svg;
			resourceDataImageHoverSize = MotasEditResources::frequency_hover_svgSize;
			resourceDataImageClick = MotasEditResources::frequency_clicked_svg;
			resourceDataImageClickSize = MotasEditResources::frequency_clicked_svgSize;


	/*		comboBox =  std::make_unique< ComboBox>();
			ParameterData::populateComboBox(comboBox.get(), 20);

			comboBox->setTooltip("LFOx clock sync. option");

			paramDataComboBox.initialise(panelControlId,  CONTROL_GLOBAL,  PanelControl::CONTROL_PARAM_LFO_FREQ, "LFOx sync. to clock");
			paramDataComboBox.setComboBox(comboBox.get());

			comboBox->setSelectedId(1, NotificationType::dontSendNotification);
			comboBox->addListener(this);
			owner->addAndMakeVisible(*comboBox);


			comboBoxSecondary =  std::make_unique< ComboBox>();
			comboBoxSecondary->addItem("OFF", 1);
			comboBoxSecondary->addItem("FIRST", 2);
			comboBoxSecondary->addItem("ALL", 3);
			comboBoxSecondary->setTooltip("LFOx reset on keypress option");
			paramDataComboBoxSecondary.initialise(panelControlId, CONTROL_GLOBAL,  PanelControl::CONTROL_PARAM_LFO_TRIGGER, "LFOx trigger");
			paramDataComboBoxSecondary.setComboBox(comboBoxSecondary.get());
			comboBoxSecondary->setSelectedId(1, NotificationType::dontSendNotification);
			comboBoxSecondary->addListener(this);
			owner->addAndMakeVisible(*comboBoxSecondary);*/


			comboBox =  std::make_unique< ComboBox>();
			comboBox->addItem("OFF", 1);
			comboBox->addItem("FIRST", 2);
			comboBox->addItem("ALL", 3);
			comboBox->setTooltip("LFOx reset on keypress option");
			paramDataComboBox.initialise(panelControlId, CONTROL_GLOBAL,  PanelControl::CONTROL_PARAM_LFO_TRIGGER, "LFOx trigger");
			paramDataComboBox.setComboBox(comboBox.get());
			comboBox->setSelectedId(1, NotificationType::dontSendNotification);
			comboBox->addListener(this);
			owner->addAndMakeVisible(*comboBox);



			break;
		}

		case PanelControl::PANEL_CONTROL_LFOX_MOD_DEPTH:
		{
			mainSliderText = "LFOx Modulation Amount";

			type = PanelControl::PANEL_TYPE_SIMPLE_LEVEL;
			controlParameter = PanelControl::CONTROL_PARAM_LFOX_MOD_DEPTH;
			resourceDataImage = MotasEditResources::level_svg;
			resourceDataSize = MotasEditResources::level_svgSize;
			resourceDataImageHover = MotasEditResources::level_hover_svg;
			resourceDataImageHoverSize = MotasEditResources::level_hover_svgSize;
			resourceDataImageClick = MotasEditResources::level_clicked_svg;
			resourceDataImageClickSize = MotasEditResources::level_clicked_svgSize;


			comboBox =  std::make_unique< ComboBox>();
			ParameterData::populateComboBox(comboBox.get(), 21);
/*
			comboBox->addItem("LFO1", 1);
			comboBox->addItem("LFO2", 2);
			comboBox->addItem("LFO3", 3);
			comboBox->addItem("LFO4", 4);
			*/
			comboBox->setTooltip("LFOx choice");
			paramDataComboBox.initialise(panelControlId,  CONTROL_GLOBAL,  PanelControl::CONTROL_PARAM_LFO_SOURCE, "LFOx choice");
			paramDataComboBox.setComboBox(comboBox.get());
			comboBox->setSelectedId(1, NotificationType::dontSendNotification);
			comboBox->addListener(this);
			owner->addAndMakeVisible(*comboBox);


			comboBoxSecondary =  std::make_unique< ComboBox>();

			ParameterData::populateComboBox(comboBoxSecondary.get(), 22);
			comboBoxSecondary->setTooltip("LFO frequency mode");
			paramDataComboBoxSecondary.initialise(panelControlId,  CONTROL_GLOBAL,  PanelControl::CONTROL_GLOBAL_LFO1_PITCH_TRACK, "LFO frequency");
			paramDataComboBoxSecondary.setComboBox(comboBoxSecondary.get());
			comboBoxSecondary->setSelectedId(1, NotificationType::dontSendNotification);
			comboBoxSecondary->addListener(this);
			owner->addAndMakeVisible(*comboBoxSecondary);




			break;
		}


		case PanelControl::PANEL_CONTROL_LFO_WAVEFORM:
		{
			mainSliderText = "LFO Waveform";

			type = PanelControl::PANEL_TYPE_LFO_WAVE;
			controlParameter = PanelControl::CONTROL_PARAM_LFO_WAVE;
		//	imageFileName = "s";


			image = Drawable::createFromImageData(MotasEditResources::s_svg, MotasEditResources::s_svgSize);


			//image = Drawable::createFromImageFile(File(IMAGE_FILE_LOCATION + imageFileName + ".svg"));
			if (image == nullptr)
			{
				DrawableText* t = new DrawableText();
				t->setText("Image N/A!");
				image.reset(t);
			}
			//std::unique_ptr<Drawable> clickImage = Drawable::createFromImageFile(File(IMAGE_FILE_LOCATION + imageFileName + "_clicked.svg"));
			//std::unique_ptr<Drawable> hoverImage = Drawable::createFromImageFile(File(IMAGE_FILE_LOCATION + imageFileName + "_hover.svg"));
			std::unique_ptr<Drawable> clickImage = Drawable::createFromImageData(MotasEditResources::s_clicked_svg, MotasEditResources::s_clicked_svgSize);
			std::unique_ptr<Drawable> hoverImage = Drawable::createFromImageData(MotasEditResources::s_hover_svg, MotasEditResources::s_hover_svgSize);;




			paramButton = std::make_unique<DrawableButton>("s", DrawableButton::ImageFitted);

			paramButton->setTooltip("LFO single-shot on/off");

			paramButton->setImages(image.get(), hoverImage.get(), clickImage.get(), nullptr, clickImage.get(), nullptr , nullptr, nullptr);
			paramDataButton.initialise(panelControlId, CONTROL_PAGE_SPECIFIC_BUT_UNDEFINED,  PanelControl::CONTROL_PARAM_LFO_SINGLE_SHOT, "LFO single-shot");
			paramButton->addListener(this);
			owner->addAndMakeVisible(*paramButton);
			paramButton->setClickingTogglesState(true);
			paramDataButton.setButton(paramButton.get());

			resourceDataImage = MotasEditResources::lfo_svg;
			resourceDataSize = MotasEditResources::lfo_svgSize;
			resourceDataImageHover = MotasEditResources::lfo_hover_svg;
			resourceDataImageHoverSize = MotasEditResources::lfo_hover_svgSize;
			resourceDataImageClick = MotasEditResources::lfo_clicked_svg;
			resourceDataImageClickSize = MotasEditResources::lfo_clicked_svgSize;
			break;
		}
		case PanelControl::PANEL_CONTROL_LFO_FREQ:
		{
			mainSliderText = "LFO Frequency";

			type = PanelControl::PANEL_TYPE_LFO2_FREQ;
			controlParameter = PanelControl::CONTROL_PARAM_LFO_FREQ;

			resourceDataImage = MotasEditResources::frequency_svg;
			resourceDataSize = MotasEditResources::frequency_svgSize;
			resourceDataImageHover = MotasEditResources::frequency_hover_svg;
			resourceDataImageHoverSize = MotasEditResources::frequency_hover_svgSize;
			resourceDataImageClick = MotasEditResources::frequency_clicked_svg;
			resourceDataImageClickSize = MotasEditResources::frequency_clicked_svgSize;



			comboBox =  std::make_unique< ComboBox>();
			ParameterData::populateComboBox(comboBox.get(), 23);
			/*
			comboBox->addItem("OFF", 1);
			comboBox->addItem("FIRST", 2);
			comboBox->addItem("ALL", 3);
			*/
			comboBox->setTooltip("LFO reset on keypress option");

			paramDataComboBox.initialise(panelControlId, CONTROL_PAGE_SPECIFIC_BUT_UNDEFINED,  PanelControl::CONTROL_PARAM_LFO_TRIGGER, "LFO trigger");
			paramDataComboBox.setComboBox(comboBox.get());
			comboBox->setSelectedId(1, NotificationType::dontSendNotification);
			comboBox->addListener(this);
			owner->addAndMakeVisible(*comboBox);



			secondSlider = std::make_unique<CustomSlider>(button.get(), this);
			secondSlider->addListener(this);

			secondSlider->setTooltip("Frequency modulation from LFOX");
			secondSlider->setPopupMenuEnabled(true);
			secondSlider->setVelocityModeParameters(1, 0, 0, true, ModifierKeys::shiftModifier);


			String s = String(" Freq mod LFOx");
			this->paramDataSecondSlider.initialise(panelControlId, CONTROL_PAGE_SPECIFIC_BUT_UNDEFINED,PanelControl::CONTROL_PARAM_LFO2_LFOX_FREQ_MOD, s);
			paramDataSecondSlider.setSlider(secondSlider.get());

			secondSlider->setRange(-4095, 4095, 1);
			owner->addAndMakeVisible(*secondSlider);



			thirdSlider = std::make_unique<CustomSlider>(button.get(), this);
			thirdSlider->addListener(this);

			thirdSlider->setTooltip("Frequency modulation from EG");
			thirdSlider->setPopupMenuEnabled(true);
			thirdSlider->setVelocityModeParameters(1, 0, 0, true, ModifierKeys::shiftModifier);


			s = String(" Freq mod EG");
			this->paramDataThirdSlider.initialise(panelControlId, CONTROL_PAGE_SPECIFIC_BUT_UNDEFINED,PanelControl::CONTROL_PARAM_LFO2_EG_FREQ_MOD, s);
			paramDataThirdSlider.setSlider(thirdSlider.get());

			thirdSlider->setRange(-4095, 4095, 1);
			owner->addAndMakeVisible(*thirdSlider);

			break;
		}
		case PanelControl::PANEL_CONTROL_LFO_MOD_DEPTH:
		{
			type = PanelControl::PANEL_TYPE_SIMPLE_LEVEL;
			mainSliderText = "LFO Modulation Amount";

			controlParameter = PanelControl::CONTROL_PARAM_LFO_MOD_DEPTH;
			resourceDataImage = MotasEditResources::level_svg;
			resourceDataSize = MotasEditResources::level_svgSize;
			resourceDataImageHover = MotasEditResources::level_hover_svg;
			resourceDataImageHoverSize = MotasEditResources::level_hover_svgSize;
			resourceDataImageClick = MotasEditResources::level_clicked_svg;
			resourceDataImageClickSize = MotasEditResources::level_clicked_svgSize;

			comboBox =  std::make_unique< ComboBox>();
			ParameterData::populateComboBox(comboBox.get(), 24);
/*
			comboBox->addItem("OFF", 1);
			comboBox->addItem("xEG", 2);
			comboBox->addItem("xLFOx", 3);
			*/
			comboBox->setTooltip("LFO output modulation");
			paramDataComboBox.initialise(panelControlId, CONTROL_PAGE_SPECIFIC_BUT_UNDEFINED,  PanelControl::CONTROL_PARAM_LFO_OUTPUT_MODE, "LFO cross modulation");
			paramDataComboBox.setComboBox(comboBox.get());
			comboBox->setSelectedId(1, NotificationType::dontSendNotification);
			comboBox->addListener(this);
			owner->addAndMakeVisible(*comboBox);


			comboBoxSecondary =  std::make_unique< ComboBox>();

			ParameterData::populateComboBox(comboBoxSecondary.get(), 22);
/*
			comboBoxSecondary->addItem("frequency", i++);
			comboBoxSecondary->addItem("note track", i++);
			comboBoxSecondary->addItem("pitch track", i++);
*/

			paramDataComboBoxSecondary.initialise(panelControlId,  CONTROL_PAGE_SPECIFIC_BUT_UNDEFINED,  PanelControl::CONTROL_PARAM_LFO2_PITCH_TRACK, "LFO frequency");
			paramDataComboBoxSecondary.setComboBox(comboBoxSecondary.get());
			comboBoxSecondary->setTooltip("LFO frequency mode");
			comboBoxSecondary->setSelectedId(1, NotificationType::dontSendNotification);
			comboBoxSecondary->addListener(this);
			owner->addAndMakeVisible(*comboBoxSecondary);



			break;
		}


		case PanelControl::PANEL_CONTROL_EG_DELAY:
			mainSliderText = "EG Delay";

			type = PanelControl::PANEL_TYPE_SIMPLE;
			controlParameter = PanelControl::CONTROL_PARAM_EG_DELAY;
			resourceDataImage = MotasEditResources::delay_svg;
			resourceDataSize = MotasEditResources::delay_svgSize;
			resourceDataImageHover = MotasEditResources::delay_hover_svg;
			resourceDataImageHoverSize = MotasEditResources::delay_hover_svgSize;
			resourceDataImageClick = MotasEditResources::delay_clicked_svg;
			resourceDataImageClickSize = MotasEditResources::delay_clicked_svgSize;


			comboBox =  std::make_unique< ComboBox>();
			ParameterData::populateComboBox(comboBox.get(), 25);
/*
			comboBox->addItem(" ", 1);
			comboBox->addItem("1", 2);
			comboBox->addItem("2", 3);
			comboBox->addItem("3", 4);
			comboBox->addItem("EVERY", 5);
			comboBox->addItem("HIGH", 6);
			comboBox->addItem("MIDDLE", 7);
			comboBox->addItem("LOW", 8);
			*/
			comboBox->setTooltip("EG trigger mode");
			paramDataComboBox.initialise(panelControlId, CONTROL_PAGE_SPECIFIC_BUT_UNDEFINED, PanelControl::CONTROL_PARAM_EG_RETRIG, "EG trigger");
			paramDataComboBox.setComboBox(comboBox.get());
			comboBox->setSelectedId(1, NotificationType::dontSendNotification);
			comboBox->addListener(this);
			owner->addAndMakeVisible(*comboBox);


			break;
		case PanelControl::PANEL_CONTROL_EG_ATTACK:
		{
			mainSliderText = "EG Attack";

			type = PanelControl::PANEL_TYPE_SIMPLE;
			controlParameter = PanelControl::CONTROL_PARAM_EG_ATTACK;
			resourceDataImage = MotasEditResources::attack_svg;
			resourceDataSize = MotasEditResources::attack_svgSize;
			resourceDataImageHover = MotasEditResources::attack_hover_svg;
			resourceDataImageHoverSize = MotasEditResources::attack_hover_svgSize;
			resourceDataImageClick = MotasEditResources::attack_clicked_svg;
			resourceDataImageClickSize = MotasEditResources::attack_clicked_svgSize;

			secondSlider = std::make_unique<CustomSlider>(button.get(), this);
			secondSlider->addListener(this);

			secondSlider->setTooltip("EG shape");
			secondSlider->setPopupMenuEnabled(true);
			secondSlider->setVelocityModeParameters(1, 0, 0, true, ModifierKeys::shiftModifier);


			String s = String(" EG shape");
			this->paramDataSecondSlider.initialise(panelControlId, CONTROL_PAGE_SPECIFIC_BUT_UNDEFINED,
					PanelControl::CONTROL_PARAM_EG_SHAPE_MULTI, s);
			paramDataSecondSlider.setSlider(secondSlider.get());

			secondSlider->setRange(-64, 64, 1);
			owner->addAndMakeVisible(*secondSlider);



			break;
		}
		case PanelControl::PANEL_CONTROL_EG_DECAY:
		{
			mainSliderText = "EG Decay";

			type = PanelControl::PANEL_TYPE_SIMPLE;
			controlParameter = PanelControl::CONTROL_PARAM_EG_DECAY;
			resourceDataImage = MotasEditResources::decay_svg;
			resourceDataSize = MotasEditResources::decay_svgSize;
			resourceDataImageHover = MotasEditResources::decay_hover_svg;
			resourceDataImageHoverSize = MotasEditResources::decay_hover_svgSize;
			resourceDataImageClick = MotasEditResources::decay_clicked_svg;
			resourceDataImageClickSize = MotasEditResources::decay_clicked_svgSize;


		/*	image = Drawable::createFromImageData(MotasEditResources::restart_svg, MotasEditResources::restart_svgSize);
			if (image == nullptr)
			{
				DrawableText* t = new DrawableText();
				t->setText("Image N/A!");
				image.reset( t);
			}
			std::unique_ptr<Drawable> clickImage = Drawable::createFromImageData(MotasEditResources::restart_clicked_svg, MotasEditResources::restart_clicked_svgSize);
			std::unique_ptr<Drawable> hoverImage = Drawable::createFromImageData(MotasEditResources::restart_hover_svg, MotasEditResources::restart_hover_svgSize);;
*/


			comboBox =  std::make_unique< ComboBox>();
						ParameterData::populateComboBox(comboBox.get(), 32);

			comboBox->setTooltip("EG restart");
			paramDataComboBox.initialise(panelControlId, CONTROL_PAGE_SPECIFIC_BUT_UNDEFINED,
					PanelControl::CONTROL_PARAM_EG_RESTART, "EG restart");
			paramDataComboBox.setComboBox(comboBox.get());
			paramDataComboBox.setMax(MAX_VALUE_EG_RESTART_ON_TRIGGER); // we need to manually set the max for this odd bitfield case
			comboBox->setSelectedId(1, NotificationType::dontSendNotification);
			comboBox->addListener(this);
			owner->addAndMakeVisible(*comboBox);

			/*

			paramButton = std::make_unique< DrawableButton>("restart", DrawableButton::ImageFitted);
			paramButton->setTooltip("EG repeat on/off");
			paramButton->setImages(image.get(), hoverImage.get(), clickImage.get(), nullptr, clickImage.get(), nullptr , nullptr, nullptr);
			paramDataButton.initialise(panelControlId, CONTROL_PAGE_SPECIFIC_BUT_UNDEFINED,  PanelControl::CONTROL_PARAM_EG_RESTART, "EG repeat");
			paramButton->addListener(this);
			owner->addAndMakeVisible(*paramButton);
			paramButton->setClickingTogglesState(true);
			paramDataButton.setButton(paramButton.get());
*/

			break;
		}
		case PanelControl::PANEL_CONTROL_EG_SUSTAIN:
		{
			mainSliderText = "EG Sustain";

			type = PanelControl::PANEL_TYPE_SIMPLE;
			controlParameter = PanelControl::CONTROL_PARAM_EG_SUSTAIN;
			resourceDataImage = MotasEditResources::sustain_svg;
			resourceDataSize = MotasEditResources::sustain_svgSize;
			resourceDataImageHover = MotasEditResources::sustain_hover_svg;
			resourceDataImageHoverSize = MotasEditResources::sustain_hover_svgSize;
			resourceDataImageClick = MotasEditResources::sustain_clicked_svg;
			resourceDataImageClickSize = MotasEditResources::sustain_clicked_svgSize;

			image = Drawable::createFromImageData(MotasEditResources::smalldownarrow_svg, MotasEditResources::smalldownarrow_svgSize);
			if (image == nullptr)
			{
				DrawableText* t = new DrawableText();
				t->setText("Image N/A!");
				image.reset(t);
			}
			std::unique_ptr<Drawable> clickImage = Drawable::createFromImageData(MotasEditResources::smalldownarrow_clicked_svg, MotasEditResources::smalldownarrow_clicked_svgSize);
			std::unique_ptr<Drawable> hoverImage = Drawable::createFromImageData(MotasEditResources::smalldownarrow_hover_svg, MotasEditResources::smalldownarrow_hover_svgSize);;
			paramButton = std::make_unique<DrawableButton>("s", DrawableButton::ImageFitted);
			paramButton->setImages(image.get(), hoverImage.get(), clickImage.get(), nullptr, clickImage.get(), nullptr , nullptr, nullptr);
			paramDataButton.initialise(panelControlId, CONTROL_PAGE_SPECIFIC_BUT_UNDEFINED,  PanelControl::CONTROL_PARAM_EG_RESTART, "EG restart");

			paramButton->addListener(this);
			paramButton->setTooltip("EG restart from zero with keypress on/off");
			owner->addAndMakeVisible(*paramButton);
			paramButton->setClickingTogglesState(true);
			paramDataButton.setButton(paramButton.get());
			paramDataButton.setMax(MAX_VALUE_EG_RESTART_ON_TRIGGER); // we need to manually set the max for this odd bitfield case


			break;
		}
		case PanelControl::PANEL_CONTROL_EG_RELEASE:
		{
			mainSliderText = "EG Release";

			controlParameter = PanelControl::CONTROL_PARAM_EG_RELEASE;
			type = PanelControl::PANEL_TYPE_SIMPLE;
			resourceDataImage = MotasEditResources::release_svg;
			resourceDataSize = MotasEditResources::release_svgSize;
			resourceDataImageHover = MotasEditResources::release_hover_svg;
			resourceDataImageHoverSize = MotasEditResources::release_hover_svgSize;
			resourceDataImageClick = MotasEditResources::release_clicked_svg;
			resourceDataImageClickSize = MotasEditResources::release_clicked_svgSize;




			image = Drawable::createFromImageData(MotasEditResources::unipolar_svg, MotasEditResources::unipolar_svgSize);
			if (image == nullptr)
			{
				DrawableText* t = new DrawableText();
				t->setText("Image N/A!");
				image.reset( t);
			}
			std::unique_ptr<Drawable> clickImage = Drawable::createFromImageData(MotasEditResources::unipolar_clicked_svg, MotasEditResources::unipolar_clicked_svgSize);
			std::unique_ptr<Drawable> hoverImage = Drawable::createFromImageData(MotasEditResources::unipolar_hover_svg, MotasEditResources::unipolar_hover_svgSize);
			paramButton = std::make_unique< DrawableButton>("restart", DrawableButton::ImageFitted);
			paramButton->setTooltip("EG unipolar on/off");
			paramButton->setImages(image.get(), hoverImage.get(), clickImage.get(), nullptr, clickImage.get(), nullptr , nullptr, nullptr);
			paramDataButton.initialise(panelControlId, CONTROL_PAGE_SPECIFIC_BUT_UNDEFINED,  PanelControl::CONTROL_PARAM_EG_UNIPOLAR, "EG unipolar");
			paramButton->addListener(this);
			owner->addAndMakeVisible(*paramButton);
			paramButton->setClickingTogglesState(true);
			paramDataButton.setButton(paramButton.get());



			break;
		}
		case PanelControl::PANEL_CONTROL_EG_MOD_DEPTH:
			mainSliderText = "EG Modulation Depth";

			controlParameter = PanelControl::CONTROL_PARAM_EG_MOD_DEPTH;
			type = PanelControl::PANEL_TYPE_SIMPLE_LEVEL;
			resourceDataImage = MotasEditResources::level_svg;
			resourceDataSize = MotasEditResources::level_svgSize;
			resourceDataImageHover = MotasEditResources::level_hover_svg;
			resourceDataImageHoverSize = MotasEditResources::level_hover_svgSize;
			resourceDataImageClick = MotasEditResources::level_clicked_svg;
			resourceDataImageClickSize = MotasEditResources::level_clicked_svgSize;
			comboBox =  std::make_unique<ComboBox>();
			ParameterData::populateComboBox(comboBox.get(), 26);
/*
			comboBox->addItem("EG", 1);
			comboBox->addItem("EG1", 2);
			comboBox->addItem("EG2", 3);
			comboBox->addItem("EG3", 4);
			comboBox->addItem("EG4", 5);*/
			comboBox->setTooltip("EG choice (independent EG or global EG1-EG4)");
			paramDataComboBox.initialise(panelControlId, CONTROL_PAGE_SPECIFIC_BUT_UNDEFINED, PanelControl::CONTROL_PARAM_EG_SOURCE, "EG choice");
			paramDataComboBox.setComboBox(comboBox.get());
			comboBox->setSelectedId(1, NotificationType::dontSendNotification);
			comboBox->addListener(this);
			owner->addAndMakeVisible(*comboBox);
			break;
		case PanelControl::PANEL_CONTROL_PRESET1:
			type = PanelControl::PANEL_TYPE_PRESET;
			resourceDataImage = MotasEditResources::number1_svg;
			resourceDataSize = MotasEditResources::number1_svgSize;
			resourceDataImageHover = MotasEditResources::number1_hover_svg;
			resourceDataImageHoverSize = MotasEditResources::number1_hover_svgSize;
			resourceDataImageClick = MotasEditResources::number1_clicked_svg;
			resourceDataImageClickSize = MotasEditResources::number1_clicked_svgSize;

			paramDataButton.initialise(panelControlId, CONTROL_GLOBAL,  0, "Preset 1");



			break;
		case PanelControl::PANEL_CONTROL_PRESET2:
			type = PanelControl::PANEL_TYPE_PRESET;
			resourceDataImage = MotasEditResources::number2_svg;
			resourceDataSize = MotasEditResources::number2_svgSize;
			resourceDataImageHover = MotasEditResources::number2_hover_svg;
			resourceDataImageHoverSize = MotasEditResources::number2_hover_svgSize;
			resourceDataImageClick = MotasEditResources::number2_clicked_svg;
			resourceDataImageClickSize = MotasEditResources::number2_clicked_svgSize;
			paramDataButton.initialise(panelControlId, CONTROL_GLOBAL,  1, "Preset 2");
			break;
		case PanelControl::PANEL_CONTROL_PRESET3:
			type = PanelControl::PANEL_TYPE_PRESET;
			resourceDataImage = MotasEditResources::number3_svg;
			resourceDataSize = MotasEditResources::number3_svgSize;
			resourceDataImageHover = MotasEditResources::number3_hover_svg;
			resourceDataImageHoverSize = MotasEditResources::number3_hover_svgSize;
			resourceDataImageClick = MotasEditResources::number3_clicked_svg;
			resourceDataImageClickSize = MotasEditResources::number3_clicked_svgSize;
			paramDataButton.initialise(panelControlId, CONTROL_GLOBAL,  2, "Preset 3");
			break;
		case PanelControl::PANEL_CONTROL_PRESET4:
			type = PanelControl::PANEL_TYPE_PRESET;
			resourceDataImage = MotasEditResources::number4_svg;
			resourceDataSize = MotasEditResources::number4_svgSize;
			resourceDataImageHover = MotasEditResources::number4_hover_svg;
			resourceDataImageHoverSize = MotasEditResources::number4_hover_svgSize;
			resourceDataImageClick = MotasEditResources::number4_clicked_svg;
			resourceDataImageClickSize = MotasEditResources::number4_clicked_svgSize;
			paramDataButton.initialise(panelControlId, CONTROL_GLOBAL,  3, "Preset 4");
			break;
		case PanelControl::PANEL_CONTROL_PRESET5:
			type = PanelControl::PANEL_TYPE_PRESET;
			resourceDataImage = MotasEditResources::number5_svg;
			resourceDataSize = MotasEditResources::number5_svgSize;
			resourceDataImageHover = MotasEditResources::number5_hover_svg;
			resourceDataImageHoverSize = MotasEditResources::number5_hover_svgSize;
			resourceDataImageClick = MotasEditResources::number5_clicked_svg;
			resourceDataImageClickSize = MotasEditResources::number5_clicked_svgSize;
			paramDataButton.initialise(panelControlId, CONTROL_GLOBAL,  4, "Preset 5");
			break;
	}

	if (type != PANEL_TYPE_NOT_DEFINED)
	{


		this->paramDataMain.initialise(panelControlId, controlPage, controlParameter, mainSliderText);

		this->owner = owner;

	//	DBG("rec w: " + String(rec.getWidth()) +  "rec h: " + String(rec.getHeight())
	//			+" x:" + String(rec.getCentreX())+ " y:" + String(rec.getCentreY()) );


	   // DBG("Image: " + imageFileName);
		//image = Drawable::createFromImageFile(File(IMAGE_FILE_LOCATION + imageFileName + ".svg"));
		image = Drawable::createFromImageData(resourceDataImage, resourceDataSize);

		if (image == nullptr)
		{
			DrawableText* t = new DrawableText();
			t->setText("Image N/A!");
			image.reset(t);
		}
		button = std::make_unique<DrawableButton>("button", DrawableButton::ImageFitted);

		button->addListener(this);

		button->setClickingTogglesState(true);

		if (panelControlId ==  PanelControl::PANEL_CONTROL_MASTER_PITCH || panelControlId == PanelControl::PANEL_CONTROL_PRESET1)
		{
			button->setToggleState(true, NotificationType::dontSendNotification);
		}
		std::unique_ptr<Drawable> hoverImage = nullptr;
		std::unique_ptr<Drawable> clickImage = nullptr;
		//if (panelControlId  <= PanelControl::PANEL_CONTROL_PRESET5)
		{
			hoverImage = Drawable::createFromImageData(resourceDataImageHover, (size_t) resourceDataImageHoverSize);
			clickImage = Drawable::createFromImageData(resourceDataImageClick, (size_t) resourceDataImageClickSize);
		}
		if (image != nullptr)
		{
			//image->setAlpha(0.5);
			//image->toBack();
			button->setImages(image.get(), hoverImage.get(), clickImage.get(), nullptr, clickImage.get(), nullptr , nullptr, nullptr);
			//button->setImages(image, image, image, nullptr, image, nullptr , nullptr, nullptr);
			//button->toBack();
			//button->setLookAndFeel(new CustomLook(0));
			owner->addAndMakeVisible(*button);
		}
		if (type == PANEL_TYPE_PARAM_KNOB || type == PANEL_TYPE_PARAM_KNOB_LEVEL)
		{
			button->setRadioGroupId(BUTTON_TYPE_PARAM_OFFSET, NotificationType::dontSendNotification);
		} else if (type == PANEL_TYPE_OPTION_BUTTON)
		{
			DBG("Button");
			button->setRadioGroupId(BUTTON_TYPE_PARAM_NO_GROUP, NotificationType::dontSendNotification);
		} else if (type == PANEL_TYPE_PRESET)
		{
			button->setRadioGroupId(BUTTON_TYPE_PRESET, NotificationType::dontSendNotification);
		}
		else
		{
			button->setRadioGroupId(BUTTON_TYPE_PARAM_MODULATION, NotificationType::dontSendNotification);
		}

		switch (type)
		{
			default:
				break;

			case PANEL_TYPE_MODULATION_CONTROL: // modulation control
			{
				mainSlider = std::make_unique<CustomSlider>(button.get(), this);
				//mainSlider = new CustomSlider( Slider::RotaryHorizontalVerticalDrag, Slider::NoTextBox);
				mainSlider->setPopupMenuEnabled(true);
				mainSlider->setVelocityModeParameters(1, 0, 0, true, ModifierKeys::shiftModifier);
				mainSlider->addListener(this);
				paramDataMain.setSlider(mainSlider.get());

				secondSlider = std::make_unique<CustomSlider>(button.get(), this);
				//secondSlider = new Slider( Slider::RotaryHorizontalVerticalDrag, Slider::NoTextBox);
				secondSlider->addListener(this);


				secondSlider->setPopupMenuEnabled(true);
				secondSlider->setVelocityModeParameters(1, 0, 0, true, ModifierKeys::shiftModifier);

				String t = "";
				switch (panelControlId)
				{
				case PanelControl::PANEL_CONTROL_VELOCITY:
					t = "Velocity";
					break;
				case PanelControl::PANEL_CONTROL_NOTE:
					t = "Note";
					break;
				case PanelControl::PANEL_CONTROL_M1:
					t = "M1";
					break;
				case PanelControl::PANEL_CONTROL_M2:
					t = "M2";
					break;
				case PanelControl::PANEL_CONTROL_M3:
					t = "M3";
					break;
				case PanelControl::PANEL_CONTROL_M4:
					t = "M4";
					break;

				}
				String s = t + String(" Alternate Dest. Amount");
				this->paramDataSecondSlider.initialise(panelControlId, CONTROL_PAGE_SPECIFIC_BUT_UNDEFINED,controlParameter + 1, s);
				paramDataSecondSlider.setSlider(secondSlider.get());
				comboBox =  std::make_unique<ComboBox>();
				mainSlider->setRange(-4095.0, 4095.0, 1);
				secondSlider->setRange(-4095.0, 4095.0, 1);

				ParameterData::populateComboBox(comboBox.get(), 30);
/*
				comboBox->addItem("1 LFOX AMOUNT", 1);
				comboBox->addItem("2 LFO FREQ", 2);
				comboBox->addItem("3 LFO AMOUNT", 3);
				comboBox->addItem("4 EG RATE", 4);
				comboBox->addItem("5 EG ATTACK", 5);
				comboBox->addItem("6 EG DECAY", 6);
				comboBox->addItem("7 EG SUSTAIN", 7);
				comboBox->addItem("8 EG RELEASE", 8);
				comboBox->addItem("9 EG AMOUNT", 9);
*/
				comboBox->setTooltip("Alternate Modulation Destination");
				s = t + String( " Alternate Dest.");
				paramDataComboBox.initialise(panelControlId, CONTROL_PAGE_SPECIFIC_BUT_UNDEFINED, controlParameter + 60, s);
				paramDataComboBox.setComboBox(comboBox.get());
				comboBox->setSelectedId(1, NotificationType::dontSendNotification);
				owner->addAndMakeVisible(*mainSlider);
				owner->addAndMakeVisible(*secondSlider);
				owner->addAndMakeVisible(*comboBox);
				comboBox->addListener(this);

				image = Drawable::createFromImageData(MotasEditResources::unipolar_svg, MotasEditResources::unipolar_svgSize);
				if (image == nullptr)
				{
					DrawableText* te = new DrawableText();
					te->setText("Image N/A!");
					image.reset( te);
				}
				clickImage = Drawable::createFromImageData(MotasEditResources::unipolar_clicked_svg, MotasEditResources::unipolar_clicked_svgSize);
				hoverImage = Drawable::createFromImageData(MotasEditResources::unipolar_hover_svg, MotasEditResources::unipolar_hover_svgSize);
				paramButton = std::make_unique< DrawableButton>("Unipolar", DrawableButton::ImageFitted);
				paramButton->setTooltip("Unipolar on/off");
				paramButton->setImages(image.get(), hoverImage.get(), clickImage.get(), nullptr, clickImage.get(), nullptr , nullptr, nullptr);
				paramDataButton.initialise(panelControlId, CONTROL_PAGE_SPECIFIC_BUT_UNDEFINED,  controlParameter + 2, "Unipolar");
				paramButton->addListener(this);
				owner->addAndMakeVisible(*paramButton);
				paramButton->setClickingTogglesState(true);
				paramDataButton.setButton(paramButton.get());


				break;
			}
			case PANEL_TYPE_LFO_WAVE: // LFO wave shape
			{

				comboBox =  std::make_unique<ComboBox>();

				ParameterData::populateComboBox(comboBox.get(), 31);
/*
				comboBox->addItem("SIN", i++);
				comboBox->addItem("SIN-90", i++);
				comboBox->addItem("TRI", i++);
				comboBox->addItem("TRI-90", i++);
				comboBox->addItem("SQUARE", i++);
				comboBox->addItem("PULSE1",i++);
				comboBox->addItem("PULSE2",i++);
				comboBox->addItem("PULSE2",i++);
				comboBox->addItem("RAMP",i++);
				comboBox->addItem("RAMP90",i++);
				comboBox->addItem("RMPHLD",i++);
				comboBox->addItem("3-STEP",i++);
				comboBox->addItem("4-STEP",i++);
				comboBox->addItem("S+H",i++);
				comboBox->addItem("NOISE",i++);
				comboBox->addItem("RANTRI",i++);
				comboBox->addItem("PLSE1R",i++);
				comboBox->addItem("PLSE2R",i++);
				comboBox->addItem("PLSE3R",i++);
				comboBox->addItem("RMPH-R",i++);
				comboBox->addItem("BOUNCE",i++);
				comboBox->addItem("BOING",i++);
				comboBox->addItem("RETARD",i++);
				comboBox->addItem("SINDEC",i++);
				comboBox->addItem("SININV",i++);
				comboBox->addItem("BURST",i++);
				comboBox->addItem("BURST2",i++);
				comboBox->addItem("SIN+2",i++);
				comboBox->addItem("SIN+3",i++);
				comboBox->addItem("SINTRI",i++);
				comboBox->addItem("TRI/2",i++);
				comboBox->addItem("BEAT",i++);
				comboBox->addItem("BEAT2",i++);
				comboBox->addItem("CHROM",i++);
				comboBox->addItem("MAJOR",i++);
				comboBox->addItem("MINOR",i++);
				comboBox->addItem("AUG",i++);
				comboBox->addItem("DIM",i++);
				comboBox->addItem("DIM7",i++);
				comboBox->addItem("2DIM7",i++);
				comboBox->addItem("MIN7",i++);
				comboBox->addItem("MINMAJ",i++);
				comboBox->addItem("DOM7",i++);
				comboBox->addItem("MAJ7",i++);
				comboBox->addItem("AUG7",i++);
				comboBox->addItem("AMAJ7",i++);
				comboBox->addItem("SEQ1",i++);
				comboBox->addItem("SEQ2",i++);
				comboBox->addItem("SEQ3",i++);
				comboBox->addItem("SEQ4",i++);
*/
				comboBox->setTooltip("LFO waveform");
				if (panelControlId == PanelControl::PANEL_CONTROL_LFOX_WAVEFORM)
					paramDataComboBox.initialise(panelControlId,  CONTROL_PAGE_SPECIFIC_BUT_UNDEFINED, controlParameter, "LFOx waveform");// not tied to a page
				else
					paramDataComboBox.initialise(panelControlId,  CONTROL_PAGE_SPECIFIC_BUT_UNDEFINED, controlParameter, "LFO waveform");// not tied to a page
				paramDataComboBox.setComboBox(comboBox.get());

				comboBox->setSelectedId(1, NotificationType::dontSendNotification);
				comboBox->addListener(this);
				owner->addAndMakeVisible(*comboBox);
				break;
			}

			case PANEL_TYPE_SIMPLE: // simple control
			case PANEL_TYPE_SIMPLE_LEVEL:
			case PANEL_TYPE_LFO2_FREQ:
			{
				mainSlider = std::make_unique<CustomSlider>(button.get(), this);
				//mainSlider = new CustomSlider( Slider::RotaryHorizontalVerticalDrag, Slider::NoTextBox);
				mainSlider->setPopupMenuEnabled(true);
				mainSlider->addListener(this);
				mainSlider->setVelocityModeParameters(1, 0, 0, true, ModifierKeys::shiftModifier);
				if (panelControlId == PanelControl::PANEL_CONTROL_EG_ATTACK
						|| panelControlId == PanelControl::PANEL_CONTROL_EG_DECAY
						|| panelControlId == PanelControl::PANEL_CONTROL_EG_RELEASE)
				{
			//		paramDataMain.setInverted(true);
					mainSlider->setRange(0, 4095.0, 1);
				} else if (panelControlId == PanelControl::PANEL_CONTROL_EG_DELAY
						|| panelControlId == PanelControl::PANEL_CONTROL_EG_SUSTAIN
						|| panelControlId == PanelControl::PANEL_CONTROL_LFOX_FREQ
						|| panelControlId == PanelControl::PANEL_CONTROL_LFO_FREQ
						)
				{
					mainSlider->setRange(0, 4095.0, 1);
				}
				else
				{
					mainSlider->setRange(-4095.0, 4095.0, 1);
				}
				owner->addAndMakeVisible(*mainSlider);

				mainSlider->toBack();
				paramDataMain.setSlider(mainSlider.get());


				break;
			}
			case PANEL_TYPE_PRESET: // preset button
				button->setTooltip("Change Preset");
				break;
			case PANEL_TYPE_OPTION_BUTTON:
				this->paramDataMain.isParameterPageFixed = true;
				break;
		}


		if (comboBox != nullptr)
			comboBox->addMouseListener(this, true);
		if (comboBoxSecondary != nullptr)
			comboBoxSecondary->addMouseListener(this, true);
	}


}

/*
 *
 * Given rectangle of entire screen area put this control in the right place
 */

#define SLIDER_SEPARATION 20

void PanelControl::setLocation(Rectangle<int> r)
{

	#define BUTTON_SEPARATION 100
	#define KNOB_SEPARATION 90
	#define KNOB_SEPARATION_HEIGHT 110
	#define KNOB_BORDER 3

	int screenWidth = r.getWidth();
	int screenHeight = r.getHeight();

	float x = (rectangle.getCentreX() * screenWidth) / 10000.0f;
	float y = (rectangle.getCentreY()* screenHeight)/ 10000.0f;
	float w = (rectangle.getWidth() * screenWidth) / 10000.0f;
	float h = (rectangle.getHeight()* screenHeight) / 10000.0f;

	float size = jmin(w * 2.0f, h*1.0f);
	float itemSize;

	//DBG("rec w: " + String(r.getWidth()) +  "rec h: " + String(r.getHeight()) +" x:" + String(x)+ " y:" + String(y) + " w:" + String(w) + " h:" + String(h));
	switch (type)
	{
		case PANEL_TYPE_PARAM_KNOB:
		case PANEL_TYPE_PARAM_KNOB_LEVEL:
			//The coordinates are relative to the top-left of the component's parent, or relative to the origin of the screen if the component is on the desktop.

			itemSize = size * 0.56f;
			mainSlider->setSize( (int) itemSize, (int) itemSize );
			mainSlider->setCentrePosition((int) (x + w/2), (int) (y + h * 0.2f));
			//mainSlider->addKeyListener(this);
			itemSize = size * 0.28f;
			if (button != nullptr)
			{
				if (type == PANEL_TYPE_PARAM_KNOB)
				{
					//button->setBounds(x, y + h/2, w/2, h / 2);
					button->setSize((int) itemSize, (int) itemSize);
				} else
				{
					button->setBounds((int)  x, (int) (y + (h * 6)/20), (int) ((w * 6)/20), (int) ((h * 6) / 20));
				}
				button->setCentrePosition((int) (x + w/2), (int) (y + h * 0.55f));
			}
			itemSize = w * 0.45f;
			if (comboBox != nullptr && comboBoxSecondary != nullptr)
			{
				comboBoxSecondary->setBounds(0,0, (int) itemSize, (int) (h *0.18f));
				comboBoxSecondary->setCentrePosition((int) (x  + w / 2 - itemSize / 2)   , (int) (y  - (1.5f*h)/8));
				comboBoxSecondary->setJustificationType(Justification::centred);

				comboBox->setBounds(0,0, (int) itemSize, (int) (h *0.18f));
				comboBox->setCentrePosition((int) (x + w / 2 + itemSize/2) , (int) (y  - (1.5f*h)/8) );
				comboBox->setJustificationType(Justification::centred);
			}
			else if (comboBox != nullptr)
			{
				comboBox->setBounds(0,0, (int) ((2*w) /3), (int) (h *0.18f));
				comboBox->setCentrePosition((int) (x + w/2) , (int) (y - (1.5f*h)/8 ));
				comboBox->setJustificationType(Justification::centred);
			}
			else if (comboBoxSecondary != nullptr)
			{
				comboBoxSecondary->setBounds(0,0, (int) ((2*w) /3), (int) (h *0.18f));
				comboBoxSecondary->setCentrePosition((int) (x + w/2) , (int) (y - (1.5f*h)/8) );
				comboBoxSecondary->setJustificationType(Justification::centred);
			}


			break;
		case PANEL_TYPE_MODULATION_CONTROL:
			itemSize = size * 0.45f;
			mainSlider->setBounds(0,0, (int)  itemSize, (int)  itemSize);
			mainSlider->setCentrePosition((int)  x  , (int) (y - h/16 + h / 12));
			itemSize = size * 0.28f;
			secondSlider->setBounds(0,0, (int)  itemSize, (int)  itemSize);
			secondSlider->setCentrePosition((int)  (x  + w * 0.45f), (int) (y - h/16));
			itemSize = itemSize * 0.5f;
			// set position of the 'unipolar' button
			paramButton->setBounds(0,0, (int)  itemSize, (int)  itemSize);
			paramButton->setCentrePosition((int)  (x  + w * 0.45f), (int) (y - h/16 + h / 5));



			comboBox->setBounds(0,0, (int) ((12 *w) / 15), (int) (h *0.24f));
			comboBox->setCentrePosition((int) (x + (1*w)/10 ), (int) (y - (5*h)/16 ) );
			comboBox->setJustificationType(Justification::centred);
			if (button != nullptr)
			{
				button->setBounds(0, 0, (int) w, (int) ((5*h)/30));
				button->setCentrePosition((int) x  , (int) (y + (5*h)/16));
			}



			break;

		case PANEL_TYPE_LFO_WAVE:
			comboBox->setBounds(0,0, (int) ((2*w) /3), (int) (h *0.24f));
			comboBox->setCentrePosition((int) x ,(int)  y );
			comboBox->setJustificationType(Justification::centred);
			if (button != nullptr)
			{
				button->setBounds(0, 0, (int) w, (int) ((6*h)/30));
				button->setCentrePosition((int) x  , (int) (y + (5*h)/16));
			}

			break;
		case PANEL_TYPE_SIMPLE:
		case PANEL_TYPE_SIMPLE_LEVEL:
			itemSize = size * 0.45f;
			mainSlider->setBounds(0,0,  (int) itemSize, (int) itemSize);
			mainSlider->setCentrePosition((int) x , (int) (y - h/16 + h / 12));

			if (button != nullptr)
			{
				button->setBounds(0, 0, (int) w, (int) (6*h)/30);
				button->setCentrePosition((int)  x , (int) (y + (5*h)/16));
			}


			itemSize = w * 0.45f;
			if (comboBox != nullptr && comboBoxSecondary != nullptr)
			{
				comboBoxSecondary->setBounds(0,0, (int) itemSize, (int) ( h *0.24f));
				comboBoxSecondary->setCentrePosition((int) (x   - itemSize / 2)   , (int) (y - (2.5f*h)/8));
				comboBoxSecondary->setJustificationType(Justification::centred);

				comboBox->setBounds(0,0, (int) itemSize, (int) (h *0.24f));
				comboBox->setCentrePosition((int) (x  + itemSize/2 ),(int) ( y - (2.5f*h)/8) );
				comboBox->setJustificationType(Justification::centred);
			} else if (comboBox != nullptr)
			{
				comboBox->setBounds(0,0, (int)  ((2*w) /3), (int) (h *0.24f));
				comboBox->setCentrePosition((int) x , (int) (y - (2.5f*h)/8 ));
				comboBox->setJustificationType(Justification::centred);
			}

			if (secondSlider != nullptr)
			{
				itemSize = size * 0.28f;
				secondSlider->setBounds(0,0, (int)  itemSize, (int)  itemSize);
				secondSlider->setCentrePosition((int)  (x ), (int) (y- (2.5f*h)/8));
			}


			break;
		case PANEL_TYPE_LFO2_FREQ:
			itemSize = size * 0.3f;
			mainSlider->setBounds(0,0,  (int) itemSize, (int) itemSize);
			mainSlider->setCentrePosition((int)( x- w/8) , (int) (y - h/16 + h / 12));

			if (button != nullptr)
			{
				button->setBounds(0, 0, (int) w, (int) (6*h)/30);
				button->setCentrePosition((int)  (x  - w/8), (int) (y + (5*h)/16));
			}


			if (secondSlider != nullptr)
			{
				itemSize = size * 0.24f;
				secondSlider->setBounds(0,0, (int)  itemSize, (int)  itemSize);
				secondSlider->setCentrePosition((int)  (x + w/5 ), (int) (y- h/16 + h / 12 - h/10));
			}

			if (thirdSlider != nullptr)
			{
				itemSize = size * 0.24f;
				thirdSlider->setBounds(0,0, (int)  itemSize, (int)  itemSize);
				thirdSlider->setCentrePosition((int)  (x + w/5), (int) (y- h/16 + h / 12 +h/10));
			}
			if (comboBox != nullptr)
			{
				comboBox->setBounds(0,0, (int)  ((2*w) /3), (int) (h *0.24f));
				comboBox->setCentrePosition((int) x , (int) (y - (2.5f*h)/8 ));
				comboBox->setJustificationType(Justification::centred);
			}




			break;
		case PANEL_TYPE_PRESET:
		case PANEL_TYPE_OPTION_BUTTON:
			if (button != nullptr)
			{
				button->setBounds(0, 0, (int) (w/2), (int) (h / 2));
				button->setCentrePosition((int) x,(int)  y );
			}
			break;
	}
	if (paramButton != nullptr)
	{
		if (type !=  PANEL_TYPE_MODULATION_CONTROL)
		{
			paramButton->setBounds(0, 0, (int) w, (int) ((3*h)/12));
			paramButton->setCentrePosition((int) x, (int) (y -  (2.5f*h)/8 ));
		}
	}




}



void PanelControl::setLook(LookAndFeel* laf)
{
	if (mainSlider != nullptr)
		mainSlider->setLookAndFeel(laf);

	if (secondSlider != nullptr)
		secondSlider->setLookAndFeel(laf);

	if (thirdSlider != nullptr)
		thirdSlider->setLookAndFeel(laf);

	if (comboBox != nullptr)
		comboBox->setLookAndFeel(laf);

	if (comboBoxSecondary != nullptr)
		comboBoxSecondary->setLookAndFeel(laf);

	if (button != nullptr)
		button->setLookAndFeel(laf);

	if (paramButton != nullptr)
		paramButton->setLookAndFeel(laf);

}


void PanelControl::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{

	if (comboBoxThatHasChanged == comboBox.get())
	{
		if (this->paramDataComboBox.getPanelControlId() == PanelControl::PANEL_CONTROL_LFOX_FREQ)
		{
		/*	if (comboBoxThatHasChanged->getSelectedId() > 1)
				mainSlider->setEnabled(false);
			else
				mainSlider->setEnabled(true);

			this->paramDataComboBox.setValue(1 - comboBoxThatHasChanged->getSelectedId());*/
			this->paramDataComboBox.setValue(comboBoxThatHasChanged->getSelectedId() -1);
		}
		else if (this->paramDataComboBox.getParameter() == PanelControl::CONTROL_PARAM_EG_SOURCE)
		{
			if (comboBoxThatHasChanged->getSelectedId() <= 1) // if set to unique EG
				this->paramDataComboBox.setValue(0xFF);
			else
				this->paramDataComboBox.setValue(comboBoxThatHasChanged->getSelectedId() - 2);
		}
		else
		{
			this->paramDataComboBox.setValue(comboBoxThatHasChanged->getSelectedId() - 1);
	/*		if (this->type == PANEL_TYPE_PARAM_KNOB || type == PANEL_TYPE_PARAM_KNOB_LEVEL)
			{
				//DBG("Change page");
				this->button->setToggleState(true, NotificationType::sendNotification);
			}*/

		}
		this->owner->parameterChangeGUI(this->paramDataComboBox);

		button->setToggleState(true, NotificationType::dontSendNotification);


	} else if (comboBoxThatHasChanged == comboBoxSecondary.get())
	{
		this->paramDataComboBoxSecondary.setValue(comboBoxThatHasChanged->getSelectedId() - 1);
	/*	if (this->type == PANEL_TYPE_PARAM_KNOB || type == PANEL_TYPE_PARAM_KNOB_LEVEL)
		{
			//DBG("Change page");
			//owner->setParameterPage(this->paramDataMain.getPage());
			this->button->setToggleState(true, NotificationType::sendNotification);
		}*/

		this->owner->parameterChangeGUI(this->paramDataComboBoxSecondary);

		button->setToggleState(true, NotificationType::dontSendNotification);

	}
}

void PanelControl::sliderValueChanged(Slider* s)
{
	if (this->paramDataComboBox.getPanelControlId() == PanelControl::PANEL_CONTROL_LFOX_FREQ)
	{
	//	if (this->comboBox->getSelectedItemIndex() != 0)
	//		return;
	}

	if (s == this->mainSlider.get())
	{
		this->paramDataMain.setValue((int32_t) s->getValue());
		this->owner->parameterChangeGUI(this->paramDataMain);
		button->setToggleState(true, NotificationType::sendNotification); // send notification so other buttons turn off

	}
	else if (s == this->secondSlider.get())
	{
		this->paramDataSecondSlider.setValue((int32_t) s->getValue());
		this->owner->parameterChangeGUI(this->paramDataSecondSlider);
		button->setToggleState(true, NotificationType::dontSendNotification);

	}else if (s == this->thirdSlider.get())
	{
		this->paramDataThirdSlider.setValue((int32_t) s->getValue());
		this->owner->parameterChangeGUI(this->paramDataThirdSlider);
		button->setToggleState(true, NotificationType::dontSendNotification);

	}
}

void PanelControl::buttonClicked (Button* b)
{

	if (b->getToggleState() && b->getRadioGroupId() != 0) // only respond to buttons pressed ON
	{
		if (b->getRadioGroupId() == BUTTON_TYPE_PARAM_OFFSET) // no data to send, just set the page
		{
			DBG("Main button pressed, data value: " + String(paramDataMain.getPanelValue()));

			//this->sliderValueChanged(this->paramDataMain.getSlider());
			owner->setParameterPage(this->paramDataMain, id);
			//this->paramDataMain.setValue((int32_t) this->paramDataMain.getSlider()->getValue());
			//this->owner->parameterChangeGUI(this->paramDataMain);
			owner->updateParamText();
			if (mainSlider != nullptr)
			{
				mainSlider->setActive();
			}
		}
		else if (b->getRadioGroupId() == BUTTON_TYPE_PRESET)
		{
			DBG("Preset Button pressed");
			this->owner->sendProgramChange((uint8_t) paramDataButton.getParameter());
		}
		else
		{
			DBG("Sub parameter selected");
			if (secondSlider != nullptr)
			{
				if (thirdSlider != nullptr)
				{
					if (thirdSlider->isSliderClicked)
					{
						owner->parameterChangeGUI(paramDataThirdSlider);
						thirdSlider->setActive();
					}
				}
				else if (secondSlider->isSliderClicked)
				{
					owner->parameterChangeGUI(paramDataSecondSlider);
					secondSlider->setActive();
				}
				else
				{
					owner->parameterChangeGUI(paramDataMain);
					if (mainSlider != nullptr)
						mainSlider->setActive();
				}
			} else
			{
				owner->parameterChangeGUI(paramDataMain);
				if (mainSlider != nullptr)
					mainSlider->setActive();
			}
		}
	}
	else if (b->getRadioGroupId() == BUTTON_TYPE_PARAM_NO_GROUP) // a button that has it's own settings
	{
		DBG("Other Button pressed");
		//this->paramDataMain.setValue(b->getToggleState());
		//this->owner->sendParameterChange(this->paramDataMain);
		this->paramDataButton.setValue(b->getToggleState());
		this->owner->parameterChangeGUI(this->paramDataButton);
		owner->updateParamText();

		button->setToggleState(true, NotificationType::dontSendNotification);

	}


}

void PanelControl::startDragChange (Slider* )
{
	/*
    DBG("Slider drag started");

    MotasPluginParameter*p = owner->holdingWindow->midiProcessor->getDAWParameter(1, 1);
    if (p!= nullptr)
    {
    	DBG("beginChangeGesture*****************");
    	p->beginChangeGesture();
    }
*/

}

void PanelControl::endDragChange (Slider* )
{
	/*
    DBG("Slider drag ended");
    MotasPluginParameter*p = owner->holdingWindow->midiProcessor->getDAWParameter(1, 1);
	if (p!= nullptr)
	{
		DBG("endChangeGesture ************");
		p->endChangeGesture();
	}
	*/
}


void PanelControl::mouseDown(const MouseEvent& event)
{

	DBG("Mouse down");
	if (event.originalComponent == comboBox.get())
	{
		DBG("M1 alternate dest mouse");
		if (paramDataComboBox.getPanelControlId() == PanelControl::PANEL_CONTROL_M1)
		{
			DBG("M1 alternate dest mouse c");
		}
	}

}

void PanelControl::disableParameterButton()
{
	if (button->getToggleState() && button->getRadioGroupId() == PanelControl::BUTTON_TYPE_PARAM_MODULATION)
	{
		button->setToggleState(false, NotificationType::dontSendNotification);
	}
}

void PanelControl::dClick(CustomSlider* s)
{
	DBG("Mouse double clicked");
	if (s == mainSlider.get())
	{
		DBG("Mouse double clicked on main slider");
		owner->restoreParameterValue(&paramDataMain, false, false);
	} else if (s == secondSlider.get())
	{
		owner->restoreParameterValue(&paramDataSecondSlider, true, false);
	}else if (s == thirdSlider.get())
	{
		owner->restoreParameterValue(&paramDataThirdSlider, false, true);
	}
	sliderValueChanged(s);
}

void PanelControl::setSelected()
{
	if (this->button != NULL)
	{
		//button->triggerClick();

		button->setToggleState(true, NotificationType::sendNotification); // send notification so other buttons turn off

		//this->owner->parameterChangeGUI(this->paramDataMain);
	}

}

void PanelControl::mouseDoubleClick(const MouseEvent& event)
{
	DBG("Mouse double clicked");
	if (event.originalComponent == mainSlider.get())
	{
		DBG("Mouse double clicked on main slider");
		//owner->restoreParameterValue(&paramDataMain);
	}

}


/*
bool PanelControl::keyPressed(const KeyPress& key,
		Component* originatingComponent)
{
	DBG("Key pressed clicked");


	return false;
}
*/
