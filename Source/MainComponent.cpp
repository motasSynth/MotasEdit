


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



#include "./Headers/MainComponent.h"
#include "./Headers/CustomLook.h"
//#include "./Headers/SubWindow.h"
#include "./Headers/MIDIComms.h"
#include "../binaryResources/MotasEditResources.h"
#include "./Headers/utilities.h"
//#include "./Headers/readscreen.h"

#include "./Headers/ModulationButton.h"
#include "./Headers/PanelControl.h"
#include "./Headers/PanelGraphic.h"

#include "./Headers/crc.h"

#include "Headers/HoldingWindowComponent.h"
#include "Headers/motasDefines.h"
#include "Headers/MIDIProcessor.h"
#include "Headers/PatchPatternSequence.h"

#include "Headers/PatchSetup.h"
#include "Headers/advancedMod.h"




#define PARAM_CONTROL_WIDTH 830.0f
#define PARAM_CONTROL_HEIGHT 1730.0f //1330.0f
#define PARAM_CONTROL_TOP_SPACE 2000.0f

#define MOD_CONTROL_WIDTH (((PARAM_CONTROL_WIDTH *22.5f) / 30.0f)*1.1f)
#define MOD_CONTROL_HEIGHT (((PARAM_CONTROL_HEIGHT *22.5f) / 30.0f)*1.3f)
#define MOD_CONTROL_TOP_SPACE 2200.0f

#define REFERENCE_SCREEN_SIZE 10000.0f

#define LINE_TOP_SPACE ((20 * PARAM_CONTROL_TOP_SPACE)  / 28)
#define SHORT_VERTICAL_LINE_LENGTH 300
#define GAP_FOR_RHS_KNOBS (PARAM_CONTROL_WIDTH * 8 - PARAM_CONTROL_WIDTH / 2)


 const int buttonRepeatSpeed = 30;
 const int buttonRepeatDelay = 150;
 const int buttonRepeatSpeedBigSteps = 30;
 const int buttonRepeatDelayBigSteps = 400;


//==============================================================================
MainComponent::MainComponent(HoldingWindowComponent* holdingWindow, LookAndFeel* l)
: holdingWindow(holdingWindow), laf(l)
{

	oldName = "";
	setWantsKeyboardFocus(true);


	editingComboPatchName = false;

	this->activeParameter = nullptr;

	for (int i = 0; i < 5; i++)
	{
		EGRestart[i] = 0;
	}
	this->activeEG = 0xFF;
	this->activeLFO = 0;
	activeParameterPage = 1;
	previousActiveParameterPage = 0;


	currentValueEdit.setTooltip("Enter new value then press RETURN");


	std::unique_ptr<Drawable> image =  Drawable::createFromImageData(MotasEditResources::motas6Logo_svg,MotasEditResources::motas6Logo_svgSize);

	if (image.get() == nullptr)
	{
		std::unique_ptr<DrawableText> t = std::make_unique<DrawableText>();
		t->setText("Image N/A!");
		image = std::move(t); // move ownership
	}
	motasLogoButton = std::make_unique<DrawableButton>("motasLogo", DrawableButton::ImageFitted);
	motasLogoButton->setImages(image.get(), nullptr, nullptr, nullptr, nullptr, nullptr , nullptr, nullptr);

	addAndMakeVisible(motasLogoButton.get());


#if INCLUDE_TEST_FUNCTIONS == 1
	addAndMakeVisible(toggleButtonTestMIDI);
#endif
	addAndMakeVisible(toggleButtonLiveScreenShot);
	addAndMakeVisible(toggleButtonChangePage);
	addAndMakeVisible(toggleButtonMonitorOnly);

	screenshotImage = Image(Image::RGB, 128*screenshotScalingFactor, 64*screenshotScalingFactor, true);


	//motasScreenshotGraphics(this->motasScreenshot);
	//midiWindow = new SubWindow("MIDI settings", Colours::grey, DocumentWindow::allButtons);

	//midiWindow->setUsingNativeTitleBar(true);
	///midiWindow->setContentOwned(new InformationComponent(), true);// InformationComponent is my GUI editor component (the visual editor of JUCE)
//
	//midiWindow->centreWithSize(midiWindow->getWidth(), midiWindow->getHeight());


	motasScreenshot = std::make_unique<ImageComponent>("motasScreenshot");
	motasScreenshot->setImage(screenshotImage);



#if INCLUDE_TEST_FUNCTIONS == 1

	toggleButtonTestMIDI.addListener(this);
#endif


	toggleButtonLiveScreenShot.addListener(this);
	toggleButtonChangePage.addListener(this);
	toggleButtonMonitorOnly.addListener(this);

	sendAllPatchSettingsButton.setButtonText("Send");
	addAndMakeVisible(sendAllPatchSettingsButton);
	requestAllPatchSettingsButton.setButtonText("Request");
	addAndMakeVisible(requestAllPatchSettingsButton);

	initControls();


	//currentValueEdit.setReadOnly(true);
	currentValueEdit.setLookAndFeel(laf);
	currentValueEditRaw.setLookAndFeel(laf);

	currentValueEdit.setJustificationType(Justification::right);
	currentValueEdit.setFont(Font(24, Font::FontStyleFlags::plain));
	currentValueEditRaw.setJustificationType(Justification::right);
	currentValueEditRaw.setFont(12);

	//currentValueEdit.addListener(this);
	String allowedChars = "-0123456789";
	//currentValueEdit.setInputRestrictions(6, allowedChars);

	currentValueName.setLookAndFeel(laf);
	currentValueName.setJustificationType(Justification::right);
	addAndMakeVisible(currentValueEdit);
	addAndMakeVisible(currentValueName);
	addAndMakeVisible(currentValueEditRaw);

	graphicsScreenshot = std::make_unique<Graphics>(screenshotImage);
/*
	comboBoxLookAndFeel.addItem ("Motas-6 look",  1);
	comboBoxLookAndFeel.addItem ("LookAndFeel_V1", 2);
	comboBoxLookAndFeel.addItem ("LookAndFeel_V2", 3);
	comboBoxLookAndFeel.addItem ("LookAndFeel_V3", 4);
	comboBoxLookAndFeel.addItem ("LookAndFeel_V4 (Dark)", 5);
	comboBoxLookAndFeel.addItem ("LookAndFeel_V4 (Midnight)", 6);
	comboBoxLookAndFeel.addItem ("LookAndFeel_V4 (Grey)", 7);
	comboBoxLookAndFeel.addItem ("LookAndFeel_V4 (Light)", 8);

    comboBoxLookAndFeel.setSelectedId(1, NotificationType::dontSendNotification);
*/


   // Typeface::Ptr f = Typeface::createSystemTypefaceFor(MotasEditResources::lfrr_ttf, MotasEditResources::lfrr_ttfSize);
   // lookAndFeels[0]->setDefaultSansSerifTypeface(f);

    // c++ lambda function technique
   //changeThemeButton.onClick = [this] { setAllLookAndFeels (); };

    sendAllPatchSettingsButton.addListener(this);
    requestAllPatchSettingsButton.addListener(this);
  //  buttonMIDIsettings.onClick = [this] {buttonMIDIclicked();};

    //addAndMakeVisible(buttonMIDIsettings);
   // addAndMakeVisible(this->changeThemeButton);

   // addAndMakeVisible(comboBoxLookAndFeel);
    addAndMakeVisible(motasScreenshot.get());



    requestAllPatchSettingsButton.setTooltip("Get current patch settings\nfrom Motas and update controls");
    sendAllPatchSettingsButton.setTooltip("Send patch settings to Motas");

#if INCLUDE_TEST_FUNCTIONS == 1

	toggleButtonTestMIDI.setClickingTogglesState(true);
	toggleButtonTestMIDI.setLookAndFeel(laf);
#endif



    toggleButtonLiveScreenShot.setClickingTogglesState(true);
    toggleButtonLiveScreenShot.setTooltip("Continually request\nlive screenshots from Motas.\nCheck Motas setup for sysex in and out enabled.");
    toggleButtonLiveScreenShot.setLookAndFeel(laf);
    toggleButtonChangePage.setTooltip("Send command so Motas display matches control changes."
    		"\nMake sure the Motas setting 'EXT. PAGE CHANGE' is ON.");
    toggleButtonChangePage.setLookAndFeel(laf);

    toggleButtonChangePage.setClickingTogglesState(true);

    toggleButtonMonitorOnly.setTooltip("Send command so Motas display always shows level monitor.");
    toggleButtonMonitorOnly.setLookAndFeel(laf);

    toggleButtonMonitorOnly.setClickingTogglesState(true);



    increaseButton.setButtonText("+");
    decreaseButton.setButtonText("-");
    increaseButtonBiggerSteps.setButtonText("++");
    decreaseButtonBiggerSteps.setButtonText("--");
    savePatchChangeButton.setButtonText("store");
    savePatchChangeButton.setTooltip("click to commit changes to the active patch");

    restorePatchChangeButton.setButtonText("restore");
    restorePatchChangeButton.setTooltip("click to undo changes from the active preset");
    addAndMakeVisible(increaseButton);
    addAndMakeVisible(decreaseButtonBiggerSteps);
    addAndMakeVisible(increaseButtonBiggerSteps);
    addAndMakeVisible(decreaseButton);
    addAndMakeVisible(savePatchChangeButton);
    addAndMakeVisible(restorePatchChangeButton);
    increaseButton.setLookAndFeel(laf);
    decreaseButton.setLookAndFeel(laf);
    increaseButton.addListener(this);
    decreaseButton.addListener(this);
    increaseButtonBiggerSteps.addListener(this);
    decreaseButtonBiggerSteps.addListener(this);
    savePatchChangeButton.addListener(this);
    restorePatchChangeButton.addListener(this);


    renamePatchButton.setButtonText("rename");
    renamePatchButton.addListener(this);
    addAndMakeVisible(renamePatchButton);
    renamePatchButton.setTooltip("Click to rename the current patch");
    renamePatchButton.setLookAndFeel(laf);

    increaseButton.setRepeatSpeed(buttonRepeatDelay, buttonRepeatSpeed);
    increaseButtonBiggerSteps.setRepeatSpeed(buttonRepeatDelayBigSteps, buttonRepeatSpeedBigSteps);
    decreaseButton.setRepeatSpeed(buttonRepeatDelay, buttonRepeatSpeed);
    decreaseButtonBiggerSteps.setRepeatSpeed(buttonRepeatDelayBigSteps, buttonRepeatSpeedBigSteps);




    renamePatchButton.setClickingTogglesState(true);


    patchesCombo.setLookAndFeel(l);
    addAndMakeVisible(patchesCombo);
    patchesCombo.setTooltip("You can organise these plugin presets on the 'Patch/Pattern/Sequence' tab");
    patchesCombo.addListener(this);


   //patchesCombo.setEditableText(true);


    resetPage.setButtonText("reset page");
    resetPage.setTooltip("Set all values for this page to defaults");
    zeroParameter.setButtonText("reset");
    zeroParameter.setTooltip("Set the active parameter to the default value");
    resetPage.setLookAndFeel(laf);
    zeroParameter.setLookAndFeel(laf);
    resetPage.addListener(this);
    zeroParameter.addListener(this);
    addAndMakeVisible(resetPage);
    addAndMakeVisible(zeroParameter);

 //   currentValueEdit.addMouseListener(this, true);



	modulation1.addListener(this);
    modulation1.setPopupMenuEnabled(true);
    modulation1.setRange(0, 127, 1);
    modulation1.setTooltip("modulation wheel (controller #1)");

	addAndMakeVisible(modulation1);
	modulation1.setValue(63, NotificationType::dontSendNotification);


	modulation2.addListener(this);
	modulation2.setPopupMenuEnabled(true);
	modulation2.setRange(0, 127, 1);
	modulation2.setTooltip("breath (controller #2)");

	addAndMakeVisible(modulation2);
	modulation2.setValue(63, NotificationType::dontSendNotification);



	modulation3.addListener(this);
	modulation3.setPopupMenuEnabled(true);
	modulation3.setRange(0, 127, 1);
	modulation3.setTooltip("(controller #3)");

	addAndMakeVisible(modulation3);
	modulation3.setValue(63, NotificationType::dontSendNotification);



	modulation4.addListener(this);
	modulation4.setPopupMenuEnabled(true);
	modulation4.setRange(0, 127, 1);
	modulation4.setTooltip("foot (controller #4)");

	addAndMakeVisible(modulation4);
	modulation4.setValue(63, NotificationType::dontSendNotification);


	//modulation1.setText



    midiKeyboard.setName ("MIDI Keyboard");
    addAndMakeVisible (midiKeyboard);
    keyboardState.addListener (this);
    setAllLookAndFeels();
    updatePresetComboBox(true);
    patchesCombo.setSelectedId(1 + holdingWindow->appCommon->getPatchNumber(), NotificationType::dontSendNotification);
    //comboBoxChanged(&this->patchesCombo);


    nameEdit = std::make_unique<TextEditor>();
	nameEdit->setMultiLine(false);
	nameEdit->setInputRestrictions(32);

	nameEdit->setColour (TextEditor::backgroundColourId, Colours::black);
	nameEdit->setColour (TextEditor::textColourId, Utilities::textYellow);
	nameEdit->addListener(this);
	nameEdit->setLookAndFeel(laf);
	nameEdit->setFont(16.0f );
	nameEdit->setTooltip("Enter new name here");

	addAndMakeVisible(nameEdit.get());


DBG("Update panel call");
    updatePanel();


    initPanelSettings();


}


void MainComponent::textEditorTextChanged(TextEditor&)
{

}


void MainComponent::updateParamText()
{
	if (activeParameter != nullptr)
	{

	//	if (!currentValueEdit.hasKeyboardFocus(true))
		{
			String valueText = activeParameter->getDisplayHumanText();
			// updating this text seems to be quite slow so now called at slower update rate
			currentValueEdit.setText(valueText, NotificationType::dontSendNotification);
			currentValueEditRaw.setText(String(activeParameter->getPanelValue()), NotificationType::dontSendNotification);
			adjustFontToFit();
			currentValueName.setText(activeParameter->getName(), NotificationType::dontSendNotification);
		}
	} else
	{
		//DBG("activeParameter is null");
	}
}


void MainComponent::initPanelSettings()
{


	toggleButtonChangePage.setToggleState(holdingWindow->appCommon->
			loadAppPropertyBool(ApplicationCommon::APP_PROPERTY_ENABLE_AUTO_PAGE_CHANGE),
			NotificationType::dontSendNotification);

	toggleButtonLiveScreenShot.setToggleState(holdingWindow->appCommon->
			loadAppPropertyBool(ApplicationCommon::APP_PROPERTY_LIVE_SCREENSHOT),
			NotificationType::dontSendNotification);

	toggleButtonMonitorOnly.setToggleState(holdingWindow->appCommon->
				loadAppPropertyBool(ApplicationCommon::APP_PROPERTY_ENABLE_SHOW_MONITOR_ONLY),
				NotificationType::dontSendNotification);


}


void MainComponent::handleNoteOn (MidiKeyboardState*, int , int midiNoteNumber, float )
{
  /* MidiMessage m (MidiMessage::noteOn (midiChannel, midiNoteNumber, velocity));
   m.setTimeStamp (Time::getMillisecondCounterHiRes() * 0.001);
   MidiBuffer buf;
   buf.addEvent(m, 1);
   holdingWindow->midiProcessor->sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_NOTES_FROM_MOTAS_EDIT, "Note-on from MotasEdit");
   */
   uint8_t buffer[3];
   buffer[0] = 0x90;
   buffer[1] = (uint8_t) midiNoteNumber;
   buffer[2] =  64;//MidiMessage::floatValueToMidiByte(velocity);

   bool retry = holdingWindow->midiProcessor->commandRequest(MIDIProcessor::COMMAND_REQUEST_SEND_GUI_NOTES_CONTROLLERS, buffer, 3);
   if (retry)
   {
	   DBG("BUSY Keyboard note ON");
   }

  // DBG("Keyboard note on, velocity" + String(velocity) + " " + String(buffer[1]));
}

void MainComponent::handleNoteOff (MidiKeyboardState*, int , int midiNoteNumber, float )
{
	/*
   MidiMessage m (MidiMessage::noteOff (midiChannel, midiNoteNumber, velocity));
   m.setTimeStamp (Time::getMillisecondCounterHiRes() * 0.001);
   MidiBuffer buf;
   buf.addEvent(m, 1);
   holdingWindow->midiProcessor->sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_NOTES_FROM_MOTAS_EDIT, "Note-off from MotasEdit");
   */
   uint8_t buffer[3];
   buffer[0] = 0x80;
   buffer[1] = (uint8_t) midiNoteNumber;
   buffer[2] = 64;//MidiMessage::floatValueToMidiByte(velocity);
   holdingWindow->midiProcessor->commandRequest(MIDIProcessor::COMMAND_REQUEST_SEND_GUI_NOTES_CONTROLLERS, buffer, 3);



   DBG("Keyboard note off");
}

void MainComponent::initControls()
{
	String fileName = "oscillator1";

//	const char* resourceDataImage = nullptr;
//	int resourceDataSize = 0;

	PanelGraphic* p = new PanelGraphic(MotasEditResources::oscillator1_svg, MotasEditResources::oscillator1_svgSize, 4 * PARAM_CONTROL_WIDTH, LINE_TOP_SPACE);
	panelGraphicsArray.add(p);
	p = new PanelGraphic(MotasEditResources::oscillator2_svg, MotasEditResources::oscillator2_svgSize, 4 * PARAM_CONTROL_WIDTH, LINE_TOP_SPACE + PARAM_CONTROL_HEIGHT);
	panelGraphicsArray.add(p);
	p = new PanelGraphic(MotasEditResources::oscillator3_svg, MotasEditResources::oscillator3_svgSize, 4 * PARAM_CONTROL_WIDTH, LINE_TOP_SPACE + 2 * PARAM_CONTROL_HEIGHT);
	panelGraphicsArray.add(p);
	p = new PanelGraphic(MotasEditResources::mixer_svg, MotasEditResources::mixer_svgSize, PARAM_CONTROL_WIDTH, LINE_TOP_SPACE + 3 * PARAM_CONTROL_HEIGHT);
	panelGraphicsArray.add(p);
	p = new PanelGraphic(MotasEditResources::lowpassfilter1_svg, MotasEditResources::lowpassfilter1_svgSize, 3.5f* PARAM_CONTROL_WIDTH, LINE_TOP_SPACE + 3 * PARAM_CONTROL_HEIGHT);
	panelGraphicsArray.add(p);
	p = new PanelGraphic(MotasEditResources::highpassfilter_svg, MotasEditResources::highpassfilter_svgSize, 6* PARAM_CONTROL_WIDTH, LINE_TOP_SPACE + 3 * PARAM_CONTROL_HEIGHT);
	panelGraphicsArray.add(p);
	p = new PanelGraphic(MotasEditResources::lowpassfilter2_svg, MotasEditResources::lowpassfilter2_svgSize, 3.5f* PARAM_CONTROL_WIDTH, LINE_TOP_SPACE + 4 * PARAM_CONTROL_HEIGHT);
	panelGraphicsArray.add(p);
	p = new PanelGraphic(MotasEditResources::output_svg, MotasEditResources::output_svgSize, 6* PARAM_CONTROL_WIDTH, LINE_TOP_SPACE + 4 * PARAM_CONTROL_HEIGHT);
	panelGraphicsArray.add(p);



	p = new PanelGraphic(MotasEditResources::modulation_svg, MotasEditResources::modulation_svgSize, PARAM_CONTROL_WIDTH * 8 - PARAM_CONTROL_WIDTH / 2 + 2.5f  * MOD_CONTROL_WIDTH,
			MOD_CONTROL_TOP_SPACE +  (MOD_CONTROL_HEIGHT  * 0.5f) / 1);
	panelGraphicsArray.add(p);


	p = new PanelGraphic(MotasEditResources::lfo_svg, MotasEditResources::lfo_svgSize, PARAM_CONTROL_WIDTH * 8 - PARAM_CONTROL_WIDTH / 2 + 1.0f  * MOD_CONTROL_WIDTH,
			MOD_CONTROL_TOP_SPACE +  (MOD_CONTROL_HEIGHT  * 1.5f) / 1);
	panelGraphicsArray.add(p);

	p = new PanelGraphic(MotasEditResources::lfoname_svg, MotasEditResources::lfoname_svgSize, PARAM_CONTROL_WIDTH * 8 - PARAM_CONTROL_WIDTH / 2 + 4.0f  * MOD_CONTROL_WIDTH,
				MOD_CONTROL_TOP_SPACE + (MOD_CONTROL_HEIGHT  * 1.5f) / 1);
	panelGraphicsArray.add(p);

	p = new PanelGraphic(MotasEditResources::envelopegenerator_svg, MotasEditResources::envelopegenerator_clicked_svgSize, PARAM_CONTROL_WIDTH * 8 - PARAM_CONTROL_WIDTH / 2 + 2.5f  * MOD_CONTROL_WIDTH,
					MOD_CONTROL_TOP_SPACE +  (MOD_CONTROL_HEIGHT  * 2.5f) / 1);
	panelGraphicsArray.add(p);



	String imageFileName;
	Rectangle<int> rec;
	int j = 0;

	int panelControlId;



	const int numberOfParameterOffsetControls = 33;
	const int numberOfModulationButtons = 6;
	const int numberOfLFOButtons = 6;
	const int numberOfEGButtons = 6;
	const int numberOfPresetButtons = 5;

	int type = 0;
	for (int i = 0; i < numberOfParameterOffsetControls + numberOfModulationButtons
						+ numberOfLFOButtons + numberOfEGButtons + numberOfPresetButtons; i++)
	{
		switch (i)
		{
			default:
				panelControlId = PanelControl::PANEL_CONTROL_NONE;
				break;
			case 0:
				panelControlId = PanelControl::PANEL_CONTROL_MASTER_PITCH;
				break;
			case 1:
				panelControlId = PanelControl::PANEL_CONTROL_OSC1_LEVEL;
				break;
			case 2:
				panelControlId = PanelControl::PANEL_CONTROL_OSC1_PITCH;
				break;
			case 3:
				panelControlId = PanelControl::PANEL_CONTROL_OSC1_TRI;
				break;
			case 4:
				panelControlId = PanelControl::PANEL_CONTROL_OSC1_SAW;
				break;
			case 5:
				panelControlId = PanelControl::PANEL_CONTROL_OSC1_PWM;
				break;
			case 6:
				panelControlId = PanelControl::PANEL_CONTROL_OSC1_PWM_LEVEL;
				break;
			case 7:
				panelControlId = PanelControl::PANEL_CONTROL_OSC2_PHASE;
				break;
			case 8:
				panelControlId = PanelControl::PANEL_CONTROL_OSC2_LEVEL;
				break;
			case 9:
				panelControlId = PanelControl::PANEL_CONTROL_OSC2_PITCH;
				break;
			case 10:
				panelControlId = PanelControl::PANEL_CONTROL_OSC2_TRI;
				break;
			case 11:
				panelControlId = PanelControl::PANEL_CONTROL_OSC2_SAW;
				break;
			case 12:
				panelControlId = PanelControl::PANEL_CONTROL_OSC2_SQUARE;
				break;
			case 13:
				panelControlId = PanelControl::CONTROL_OSC2_SUB;
				break;
			case 14:
				panelControlId = PanelControl::PANEL_CONTROL_OSC3_PHASE;
				break;
			case 15:
				panelControlId = PanelControl::PANEL_CONTROL_OSC3_LEVEL;
				break;
			case 16:
				panelControlId = PanelControl::PANEL_CONTROL_OSC3_PITCH;
				break;
			case 17:
				panelControlId = PanelControl::PANEL_CONTROL_OSC3_TRI;
				break;
			case 18:
				panelControlId = PanelControl::PANEL_CONTROL_OSC3_SAW;
				break;
			case 19:
				panelControlId = PanelControl::PANEL_CONTROL_OSC3_PWM;
				break;
			case 20:
				panelControlId = PanelControl::PANEL_CONTROL_OSC3_PWM_LEVEL;
				break;
			case 21:
				panelControlId = PanelControl::PANEL_CONTROL_NOISE;
				break;
			case 22:
				panelControlId = PanelControl::PANEL_CONTROL_MIX;
				break;
			case 23:
				panelControlId = PanelControl::PANEL_CONTROL_LPF1;
				break;
			case 24:
				panelControlId = PanelControl::PANEL_CONTROL_LPF1_RES;
				break;
			case 25:
				panelControlId = PanelControl::PANEL_CONTROL_LPF1_LEVEL;
				break;
			case 26:
				panelControlId = PanelControl::PANEL_CONTROL_HPF;
				break;
			case 27:
				panelControlId = PanelControl::PANEL_CONTROL_HPF_LEVEL;
				break;
			case 28:
				panelControlId = PanelControl::PANEL_CONTROL_EXTERN;
				break;
			case 29:
				panelControlId = PanelControl::PANEL_CONTROL_LPF2;
				j++;
				break;
			case 30:
				panelControlId = PanelControl::PANEL_CONTROL_LPF2_RES;
				break;
			case 31:
				panelControlId = PanelControl::PANEL_CONTROL_LPF2_LEVEL;
				break;
			case 32:
				panelControlId = PanelControl::PANEL_CONTROL_OUTPUT;
				j++;
				break;



			case 33:
				panelControlId = PanelControl::PANEL_CONTROL_NOTE;
				j = 0;
				type = 1;
				break;
			case 34:
				panelControlId = PanelControl::PANEL_CONTROL_VELOCITY;
				break;
			case 35:
				panelControlId = PanelControl::PANEL_CONTROL_M1;
				break;
			case 36:
				panelControlId = PanelControl::PANEL_CONTROL_M2;
				break;
			case 37:
				panelControlId = PanelControl::PANEL_CONTROL_M3;
				break;
			case 38:
				panelControlId = PanelControl::PANEL_CONTROL_M4;
				break;

			case 39:
				panelControlId = PanelControl::PANEL_CONTROL_LFOX_WAVEFORM;
				j = 0;
				type = 2;
				break;
			case 40:
				panelControlId = PanelControl::PANEL_CONTROL_LFOX_FREQ;
				break;
			case 41:
				panelControlId = PanelControl::PANEL_CONTROL_LFOX_MOD_DEPTH;
				break;
			case 42:
				panelControlId = PanelControl::PANEL_CONTROL_LFO_WAVEFORM;
				break;
			case 43:
				panelControlId = PanelControl::PANEL_CONTROL_LFO_FREQ;
				break;
			case 44:
				panelControlId = PanelControl::PANEL_CONTROL_LFO_MOD_DEPTH;
				break;
			case 45:
				panelControlId = PanelControl::PANEL_CONTROL_EG_DELAY;
				j = 0;
				type = 3;
				break;
			case 46:
				panelControlId =  PanelControl::PANEL_CONTROL_EG_ATTACK;
				break;
			case 47:
				panelControlId =  PanelControl::PANEL_CONTROL_EG_DECAY;
				break;
			case 48:
				panelControlId =  PanelControl::PANEL_CONTROL_EG_SUSTAIN;
				break;
			case 49:
				panelControlId =  PanelControl::PANEL_CONTROL_EG_RELEASE;
				break;
			case 50:
				panelControlId =  PanelControl::PANEL_CONTROL_EG_MOD_DEPTH;
				break;

			case 51:
				j = 0;
				type = 4;
				panelControlId = PanelControl::PANEL_CONTROL_PRESET1;
				break;
			case 52:
				panelControlId = PanelControl::PANEL_CONTROL_PRESET2;
				break;
			case 53:
				panelControlId = PanelControl::PANEL_CONTROL_PRESET3;
				break;
			case 54:
				panelControlId = PanelControl::PANEL_CONTROL_PRESET4;
				break;
			case 55:
				panelControlId = PanelControl::PANEL_CONTROL_PRESET5;
				break;

		}


		switch (type) // main parameter selections
		{
			case 0:
				rec.setWidth((int)PARAM_CONTROL_WIDTH);
				rec.setHeight((int)PARAM_CONTROL_HEIGHT);
				rec.setCentre((int)((j % 7) * PARAM_CONTROL_WIDTH), (int)(PARAM_CONTROL_TOP_SPACE + (j / 7) * PARAM_CONTROL_HEIGHT));
				break;
			case 1:
				rec.setWidth((int)MOD_CONTROL_WIDTH);
				rec.setHeight((int)MOD_CONTROL_HEIGHT);
				rec.setCentre((int)(GAP_FOR_RHS_KNOBS   + j  * MOD_CONTROL_WIDTH), (int)(MOD_CONTROL_TOP_SPACE + MOD_CONTROL_HEIGHT) );
				break;
			case 2:
				rec.setCentre((int)(GAP_FOR_RHS_KNOBS + j * MOD_CONTROL_WIDTH), (int)(MOD_CONTROL_TOP_SPACE + MOD_CONTROL_HEIGHT  *2));
				break;
			case 3:
				rec.setCentre((int)(GAP_FOR_RHS_KNOBS + j * MOD_CONTROL_WIDTH), (int)(MOD_CONTROL_TOP_SPACE + MOD_CONTROL_HEIGHT  * 3));
				break;
			case 4:
				rec.setCentre((int)((PARAM_CONTROL_WIDTH * 7) / 2 + j * ((PARAM_CONTROL_WIDTH * 22.5f) / 30.0f)),
						(int)(1.4f * PARAM_CONTROL_TOP_SPACE / 3));
				break;
		}
		PanelControl* pc = new PanelControl(this, panelControlId, rec);
		controlsArray.add(pc);
		j++;
		//s->addListener(this);
	}



	this->activeParameter = &controlsArray[0]->paramDataMain;
	activePanelControlId = 0;
	holdingWindow->midiProcessor->commandRequest(MIDIProcessor::COMMAND_REQUEST_GUI_UPDATE, 0);



}


void MainComponent::setAllLookAndFeels ()
{

	this->sendAllPatchSettingsButton.setLookAndFeel(laf);
	this->requestAllPatchSettingsButton.setLookAndFeel(laf);


	for (int i = 0; i < controlsArray.size(); i++)
	{
		PanelControl* p = controlsArray[i];
		int id = 0;//comboBoxLookAndFeel.getSelectedId() - 1;
		if (id == 0)
		{
			if (p->type == PanelControl::PANEL_TYPE_PARAM_KNOB_LEVEL
					|| p->type == PanelControl::PANEL_TYPE_SIMPLE_LEVEL
					|| p->type == PanelControl::PANEL_TYPE_MODULATION_CONTROL)
				//p->setLook(lookAndFeels[1]);
				p->setLook(&knobLookAndFeel);
			else
				//p->setLook(lookAndFeels[0]);
				p->setLook(this->laf);
		} else
		{
			//p->setLook(lookAndFeels[id + 1]);
		}
	}
}



MainComponent::~MainComponent()
{
	keyboardState.removeListener (this);
	DBG("MainComponent destructor");
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    drawBackground(g, mainColour);
}





void MainComponent::resized()
{


/*
		DBG("Size of tEGCore: " + String( sizeof(tEGCore)));
		DBG("Size of tCommonLFO: " + String( sizeof(tCommonLFO)));
		DBG("Size of tLFOAmount: " + String( sizeof(tLFOAmount)));
		DBG("Size of tEGAmount: " + String( sizeof(tEGAmount)));
		DBG("Size of tStorageLFO: " + String( sizeof(tStorageLFO)));
		DBG("Size of tStorageMIDI: " + String( sizeof(tStorageMIDI)));
		DBG("Size of singleParam: " + String( sizeof(singleParam)));
		DBG("Size of tArpegSettings: " + String( sizeof(tArpegSettings)));
		DBG("Size of tArpegChordSettings: " + String( sizeof(tArpegChordSettings)));
		DBG("Size of tAllParam: " + String( sizeof(tAllParam)));
*/



    // This is called when the MainComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

	//returns a rectangle that is at position (0, 0) with the same width and height as the componen


    auto r = getLocalBounds().reduced(10);
    float h = (float) r.getHeight();
    float w = (float) r.getWidth();


    float comboHeight = h * 0.04f;
    if (comboHeight < 16)
    	comboHeight = 16;
    float comboWidth = w * 0.18f;

    float buttonHeight =  h / 24.0f;
    float buttonHeightThin = buttonHeight * 0.7f;

    patchesCombo.setSize((int)comboWidth, (int)(buttonHeightThin * 2.0f));


    const float hCombo = (PARAM_CONTROL_WIDTH * 7) / 2 + 1 * ((PARAM_CONTROL_WIDTH * 22.5f) / 30.0f);
    const float patchesX = (hCombo * w) / 10000.0f;
    float patchesY =  h * 0.038f;
    patchesCombo.setCentrePosition((int) patchesX, (int) patchesY);


    int buttonWidth = (int) (w * ((PARAM_CONTROL_WIDTH * 22.5f) / 30.0f) / 10000.0f);




    savePatchChangeButton.setSize((int) buttonWidth, (int)buttonHeightThin);
    savePatchChangeButton.setCentrePosition((int)(patchesX + 2 * buttonWidth), (int)(patchesY + buttonHeightThin / 2.0f) );
    restorePatchChangeButton.setSize((int)(buttonWidth), (int) buttonHeightThin);
    restorePatchChangeButton.setCentrePosition((int)(patchesX + 3 * buttonWidth), (int)(patchesY + buttonHeightThin/2.0f ));


	renamePatchButton.setSize((int)(buttonWidth*2), (int) buttonHeightThin);
    renamePatchButton.setCentrePosition((int)(patchesX + 2.5f * buttonWidth), (int)(patchesY - buttonHeightThin / 2.0f ));

    buttonHeight =  h / 24.0f;
    buttonWidth = (int) (w *0.138f);

    for (int i = 0; i < controlsArray.size(); i++)
	{
		PanelControl* p = this->controlsArray[i];

		p->setLocation(r);
	}







    sendAllPatchSettingsButton.setBounds (r.removeFromBottom((int)buttonHeight).removeFromRight ((int)buttonWidth/2));
    requestAllPatchSettingsButton.setBounds (r.removeFromBottom((int)buttonHeight).removeFromRight ((int)buttonWidth/2));



    r = getLocalBounds().reduced(10);//.removeFromRight(w*0.05f);
    auto r2 = r;
    r.removeFromRight(buttonWidth/2);

   	decreaseButton.setBounds(r.removeFromBottom((int)buttonHeight).removeFromRight (buttonWidth/4));
   	increaseButton.setBounds(r.removeFromBottom((int)buttonHeight).removeFromRight (buttonWidth/4));
    r = getLocalBounds().reduced(10);
    r.removeFromRight(buttonWidth/2 + buttonWidth/4);
   	decreaseButtonBiggerSteps.setBounds(r.removeFromBottom((int)buttonHeight).removeFromRight (buttonWidth/4));
    increaseButtonBiggerSteps.setBounds(r.removeFromBottom((int)buttonHeight).removeFromRight (buttonWidth/4));

    r = getLocalBounds().reduced(10);
    r.removeFromRight(buttonWidth);
    currentValueEditRaw.setBounds (r.removeFromBottom((int)(buttonHeight*0.5f)).removeFromRight (buttonWidth*3/2));
    currentValueEdit.setBounds (r.removeFromBottom((int)(buttonHeight*1.0f)).removeFromRight (buttonWidth*3/2));
    currentValueName.setBounds (r.removeFromBottom((int)(buttonHeight*0.5f)).removeFromRight (buttonWidth*3/2));

    r = getLocalBounds().reduced(10);




    r = getLocalBounds().reduced(10);
	r.removeFromRight((int)(2.5f * buttonWidth));
	zeroParameter.setBounds(r.removeFromBottom((int)buttonHeight).removeFromRight (buttonWidth/2));
	resetPage.setBounds(r.removeFromBottom((int)buttonHeight).removeFromRight (buttonWidth/2));



    motasScreenshot->setSize((int) (w *0.3f), (int) (h*0.25f));
    motasScreenshot->setTopLeftPosition((int) ((GAP_FOR_RHS_KNOBS * w*0.975f) / 10000.0f) , 10);

    r2.removeFromTop((int)buttonHeight*2);
    r2.removeFromRight((int) buttonWidth /8);

#if INCLUDE_TEST_FUNCTIONS == 1
    toggleButtonTestMIDI.setBounds (r2.removeFromTop((int)buttonHeight).removeFromRight ((int)buttonWidth/2));
#endif



    toggleButtonLiveScreenShot.setBounds (r2.removeFromTop((int)buttonHeight).removeFromRight ((int)buttonWidth/2));
    r2.removeFromTop((int)buttonHeight/2);
    toggleButtonChangePage.setBounds (r2.removeFromTop((int)buttonHeight).removeFromRight ((int)buttonWidth/2));

    toggleButtonMonitorOnly.setBounds (r2.removeFromTop((int)buttonHeight).removeFromRight ((int)buttonWidth/2));






    motasLogoButton->setBounds(0,  0, (int) ( w *0.2f), (int) ( h*0.25f));
    motasLogoButton->setCentrePosition((int) (w *0.12f),  (int)  (h*0.075f));


    float controllersHeight = h*0.1f;
    float controllersWidth = w *0.38f;

    float conposX = w * 0.605f;
    int controllersModWidth = (int) (controllersWidth * 0.05f);
    modulation1.setSize( (int) controllersModWidth, (int) controllersHeight);
    modulation1.setTopLeftPosition((int) conposX , (int) (h* 0.825f));
    conposX *= 1.035f;
    modulation2.setSize((int)controllersModWidth, (int)controllersHeight);
    modulation2.setTopLeftPosition((int)  conposX , (int) (h* 0.825f));
    conposX *= 1.035f;
    modulation3.setSize((int)controllersModWidth, (int)controllersHeight);
    modulation3.setTopLeftPosition((int)  conposX , (int) (h* 0.825f));
    conposX *= 1.035f;
    modulation4.setSize((int)controllersModWidth, (int)controllersHeight);
    modulation4.setTopLeftPosition((int)  conposX , (int) (h* 0.825f));

    midiKeyboard.setSize((int)(controllersWidth * 0.78f), (int)controllersHeight);
    midiKeyboard.setTopLeftPosition((int) (w *(0.09f + 0.605f)) , (int) (h* 0.825f));


    if (w < 1000)
    	toggleButtonLiveScreenShot.setButtonText("SC");
    else
    	toggleButtonLiveScreenShot.setButtonText("Screen capture");

    if (w < 1000)
    	toggleButtonChangePage.setButtonText("APC");
    else
    	toggleButtonChangePage.setButtonText("Auto page change");

    if (w < 1000)
		toggleButtonMonitorOnly.setButtonText("MON");
	else
		toggleButtonMonitorOnly.setButtonText("Monitor display only");


    adjustFontToFit();

}

void MainComponent::drawBackground(Graphics& g, Colour c)
{
	this->mainColour = c;
	Rectangle<int> r = getLocalBounds().reduced(10);

	float scaleW =  r.getWidth() / REFERENCE_SCREEN_SIZE;
	float scaleH =  r.getHeight() /REFERENCE_SCREEN_SIZE;


	drawLine(g, PARAM_CONTROL_WIDTH * scaleW,  LINE_TOP_SPACE * scaleH,
			7 * PARAM_CONTROL_WIDTH * scaleW, LINE_TOP_SPACE * scaleH);

	drawLine (g,PARAM_CONTROL_WIDTH * scaleW * 0.25f, (LINE_TOP_SPACE + PARAM_CONTROL_HEIGHT) * scaleH,
			7 * PARAM_CONTROL_WIDTH * scaleW, (LINE_TOP_SPACE + PARAM_CONTROL_HEIGHT) * scaleH);

	drawLine (g,PARAM_CONTROL_WIDTH * scaleW * 0.25f, (LINE_TOP_SPACE + 2* PARAM_CONTROL_HEIGHT) * scaleH,
			7 * PARAM_CONTROL_WIDTH * scaleW, (LINE_TOP_SPACE + 2* PARAM_CONTROL_HEIGHT) * scaleH);

	drawLine (g,PARAM_CONTROL_WIDTH * scaleW * 0.25f, (LINE_TOP_SPACE + 3 * PARAM_CONTROL_HEIGHT) * scaleH,
			7 * PARAM_CONTROL_WIDTH * scaleW, (LINE_TOP_SPACE + 3 * PARAM_CONTROL_HEIGHT) * scaleH);

	drawLine (g,2*PARAM_CONTROL_WIDTH * scaleW, (LINE_TOP_SPACE + 4 * PARAM_CONTROL_HEIGHT) * scaleH,
				7 * PARAM_CONTROL_WIDTH * scaleW, (LINE_TOP_SPACE + 4 * PARAM_CONTROL_HEIGHT) * scaleH);



	// draw small vertical lines

	drawLine (g,2*PARAM_CONTROL_WIDTH * scaleW, (LINE_TOP_SPACE) * scaleH,
			2*PARAM_CONTROL_WIDTH * scaleW, (LINE_TOP_SPACE + SHORT_VERTICAL_LINE_LENGTH) * scaleH);


	drawLine (g,2*PARAM_CONTROL_WIDTH * scaleW, (LINE_TOP_SPACE + 3*PARAM_CONTROL_HEIGHT) * scaleH,
				2*PARAM_CONTROL_WIDTH * scaleW, (LINE_TOP_SPACE + SHORT_VERTICAL_LINE_LENGTH + 3*PARAM_CONTROL_HEIGHT) * scaleH);


	drawLine (g,5*PARAM_CONTROL_WIDTH * scaleW, (LINE_TOP_SPACE + 3*PARAM_CONTROL_HEIGHT) * scaleH,
				5*PARAM_CONTROL_WIDTH * scaleW, (LINE_TOP_SPACE + SHORT_VERTICAL_LINE_LENGTH + 3*PARAM_CONTROL_HEIGHT) * scaleH);


	drawLine (g,2*PARAM_CONTROL_WIDTH * scaleW, (LINE_TOP_SPACE + 4*PARAM_CONTROL_HEIGHT) * scaleH,
				2*PARAM_CONTROL_WIDTH * scaleW, (LINE_TOP_SPACE + SHORT_VERTICAL_LINE_LENGTH + 4*PARAM_CONTROL_HEIGHT) * scaleH);


	drawLine (g,5*PARAM_CONTROL_WIDTH * scaleW, (LINE_TOP_SPACE + 4*PARAM_CONTROL_HEIGHT) * scaleH,
				5*PARAM_CONTROL_WIDTH * scaleW, (LINE_TOP_SPACE + SHORT_VERTICAL_LINE_LENGTH + 4*PARAM_CONTROL_HEIGHT) * scaleH);




	// MODs

	drawLine (g,(PARAM_CONTROL_WIDTH * 7.5f  + 2.5f  * MOD_CONTROL_WIDTH) * scaleW, (MOD_CONTROL_TOP_SPACE +MOD_CONTROL_HEIGHT  * 1.5f)  * scaleH,
			(PARAM_CONTROL_WIDTH * 7.5f  + 2.5f  * MOD_CONTROL_WIDTH) * scaleW, ((MOD_CONTROL_TOP_SPACE +MOD_CONTROL_HEIGHT  * 1.5f)  + SHORT_VERTICAL_LINE_LENGTH) * scaleH);


	drawLine (g,(PARAM_CONTROL_WIDTH * 7.5f ) * scaleW, (MOD_CONTROL_TOP_SPACE +MOD_CONTROL_HEIGHT  * 0.5f)  * scaleH,
			(PARAM_CONTROL_WIDTH * 7.5f  + 5  * MOD_CONTROL_WIDTH) * scaleW, ((MOD_CONTROL_TOP_SPACE +MOD_CONTROL_HEIGHT  * 0.5f)  )* scaleH);


	drawLine (g,(PARAM_CONTROL_WIDTH * 7.5f ) * scaleW, (MOD_CONTROL_TOP_SPACE +MOD_CONTROL_HEIGHT  * 1.5f)  * scaleH,
			(PARAM_CONTROL_WIDTH * 7.5f  + 5  * MOD_CONTROL_WIDTH) * scaleW, ((MOD_CONTROL_TOP_SPACE +MOD_CONTROL_HEIGHT  * 1.5f)  )* scaleH);

	drawLine (g,(PARAM_CONTROL_WIDTH * 7.5f ) * scaleW, (MOD_CONTROL_TOP_SPACE +MOD_CONTROL_HEIGHT  * 2.5f)  * scaleH,
			(PARAM_CONTROL_WIDTH * 7.5f  + 5  * MOD_CONTROL_WIDTH) * scaleW, ((MOD_CONTROL_TOP_SPACE+ MOD_CONTROL_HEIGHT  * 2.5f)  )* scaleH);

	drawLine (g,(PARAM_CONTROL_WIDTH * 7.5f) * scaleW, (MOD_CONTROL_TOP_SPACE +MOD_CONTROL_HEIGHT  * 3.5f)  * scaleH,
			(PARAM_CONTROL_WIDTH * 7.5f + 5  * MOD_CONTROL_WIDTH) * scaleW, ((MOD_CONTROL_TOP_SPACE +MOD_CONTROL_HEIGHT  * 3.5f)  )* scaleH);



	for (int i = 0; i < panelGraphicsArray.size(); i++)
	{
		panelGraphicsArray[i]->drawGraphic(g, scaleW, scaleH, mainColour);
	}

	//g.drawImageAt(im, (int)  (3 * PARAM_CONTROL_WIDTH * scaleW), (int) ((LINE_TOP_SPACE + PARAM_CONTROL_HEIGHT) * scaleH));


}

void MainComponent::drawLine(Graphics& g, float x1, float y1, float x2, float y2)
{
	g.setColour (Colours::black);

	float lineThickness = 2.0f;
	Path p;
	p.addLineSegment (Line<float> (x1, y1, x2, y2),  lineThickness);
	g.strokePath(p, PathStrokeType(lineThickness, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::rounded));
	g.fillPath(p);

}

bool MainComponent::keyPressed(const KeyPress& key)
{
	/*
	 *
	 * Allow left/right to move between main potentiometer controls
	 */

	if (key == KeyPress::homeKey)
	{
		activePanelControlId = 0;
		controlsArray[activePanelControlId]->setSelected();
	}else if (key == KeyPress::endKey)
	{
		activePanelControlId = 32;
		controlsArray[activePanelControlId]->setSelected();
	}
	else if (key == KeyPress::pageUpKey)
	{
		if (activePanelControlId > 0)
		{
			activePanelControlId--;
			controlsArray[activePanelControlId]->setSelected();
		}
	} else if (key == KeyPress::pageDownKey)
	{
		if (activePanelControlId < 32)
		{
			activePanelControlId++;
			controlsArray[activePanelControlId]->setSelected();
		}
	}


	return false;
}


/*
 *
 * Use this when we edit e.g. CC mappings (not used for normal patch editing via NRPN)
 */
void MainComponent::indicatePatchDataChanged()
{

#if BUILD_STANDALONE_EXECUTABLE == 0
	MIDIProcessor* m = this->holdingWindow->midiProcessor;
#else
	MIDIProcessor* m = this->holdingWindow->midiProcessor.get();
#endif

	m->indicatePatchDataChanged();


}

void MainComponent::resetParameterInPatch(ParameterData&)
{


	//holdingWindow->midiProcessor->resetParameterInPatch(p);
	//NRPNmessage_t n = p.convertParameterDataToNRPN();
	//holdingWindow->midiProcessor->commandRequest(MIDIProcessor::COMMAND_REQUEST_GUI_UPDATE, 0);


/*
	int page = p.getPage();
	singleParam* sp;
	tAllParams* patch = (tAllParams*) holdingWindow->appCommon->getGUIPatch();

	if (page > 0 && page <= 29)
		sp = &patch->params.singleParams[page - 1];
	else if (page > 29 && page < 35)
		sp = &patch->params.singleParams[page - 2];
	else
		sp = &patch->params.paramNames.VcaMaster;
	DBG("Page: " + String(page));
	for (int parameter = 0; parameter < PanelControl::CONTROL_PARAM_END_MARKER; parameter++)
	{
		int32_t val = 0;;
		if (page > 0)
		{
			switch (parameter)
			{
				case PanelControl::CONTROL_PARAM_OFFSET:
					DBG("Adjusting basic page offset: " + String(val));

					if (page == PanelControl::PANEL_CONTROL_OSC1_PITCH
							|| page == PanelControl::PANEL_CONTROL_OSC2_PITCH
							|| page == PanelControl::PANEL_CONTROL_OSC3_PITCH)
						val = 18000;

					// the final result we want is 'val' but internally Motas will add the potentiometerValue to the basic offset to get that result.
					sp->basicOffset  = val - sp->potentiometerValue;
					// DO NOT tamper with offsetIncludingPot as Motas will set that by adding basicOffset to potentiometerValue
					//sp->offsetIncludingPot = val;
					break;

				case PanelControl::CONTROL_PARAM_EG_MOD_DEPTH:
					sp->EG1.modDepth = (int16_t) val;
					break;
				case PanelControl::CONTROL_PARAM_EG_DELAY:
					sp->EG0.delay = (uint16_t) val;
					break;
				case PanelControl::CONTROL_PARAM_EG_ATTACK:
					sp->EG0.attack = EG_RATE_MAX_VALUE;
					break;
				case PanelControl::CONTROL_PARAM_EG_DECAY:
					sp->EG0.decay = EG_RATE_MAX_VALUE;
					break;
				case PanelControl::CONTROL_PARAM_EG_SUSTAIN:
					sp->EG0.sustain = (uint16_t) val;
					break;
				case PanelControl::CONTROL_PARAM_EG_RELEASE:
					sp->EG0.release = EG_RATE_MAX_VALUE;
					break;
				case PanelControl::CONTROL_PARAM_LFOX_MOD_DEPTH:
					sp->LFO1.modDepth = (int16_t)val;
					break;
				case PanelControl::CONTROL_PARAM_LFO_FREQ:
					sp->LFO2.freq = (int16_t)val;
					break;
				case PanelControl::CONTROL_PARAM_LFO_MOD_DEPTH:
					sp->LFO2.modDepth = (int16_t) val;
					break;
				case PanelControl::CONTROL_PARAM_M1_LEVEL:
					sp->MIDICon1.modDepth = (int16_t)  val;
					break;
				case PanelControl::CONTROL_PARAM_M1_ALT_LEVEL:
					sp->MIDICon1.modDepthAlternate = (int16_t)  val;
					break;
				case PanelControl::CONTROL_PARAM_M2_LEVEL:
					sp->MIDICon2.modDepth = (int16_t)  val;
					break;
				case PanelControl::CONTROL_PARAM_M2_ALT_LEVEL:
					sp->MIDICon2.modDepthAlternate = (int16_t)  val;
					break;
				case PanelControl::CONTROL_PARAM_M3_LEVEL:
					sp->MIDICon3.modDepth = (int16_t)  val;
					break;
				case PanelControl::CONTROL_PARAM_M3_ALT_LEVEL:
					sp->MIDICon3.modDepthAlternate =(int16_t)   val;
					break;
				case PanelControl::CONTROL_PARAM_M4_LEVEL:
					sp->MIDICon4.modDepth = (int16_t)  val;
					break;
				case PanelControl::CONTROL_PARAM_M4_ALT_LEVEL:
					sp->MIDICon4.modDepthAlternate = (int16_t)  val;
					break;

				case PanelControl::CONTROL_PARAM_VELOCITY_LEVEL:
					sp->MIDIVelocity.modDepth =(int16_t)   val;
					break;
				case PanelControl::CONTROL_PARAM_VELOCITY_ALT_LEVEL:
					sp->MIDIVelocity.modDepthAlternate = (int16_t)  val;
					break;

				case PanelControl::CONTROL_PARAM_NOTE_LEVEL:
					sp->MIDINote.modDepth = (int16_t)  val;
					break;
				case PanelControl::CONTROL_PARAM_NOTE_ALT_LEVEL:
					sp->MIDINote.modDepthAlternate = (int16_t) val;
					break;

				case PanelControl::CONTROL_PARAM_PAGE_OPTION1:
					sp->option1 = (uint8_t) val;
					break;
				case PanelControl::CONTROL_PARAM_PAGE_OPTION2:
					sp->option2 = (uint8_t) val;
					break;

				case PanelControl::CONTROL_PARAM_EG_SOURCE:
					sp->EG1.source = 0xFF;
					break;
				case PanelControl::CONTROL_PARAM_EG_RETRIG:
					sp->EG0.egRetrig = (uint8_t) val;
					break;
				case PanelControl::CONTROL_PARAM_EG_RESTART:
					sp->EG0.egRestartOnTrigger = (uint8_t) val;
					break;
				case PanelControl::CONTROL_PARAM_EG_SHAPE:
					sp->EG0.egShape =(uint8_t)  val;
					break;

				case PanelControl::CONTROL_PARAM_LFO_SOURCE:
					sp->LFO1.source = (uint8_t) 0;
					break;

				case PanelControl::CONTROL_PARAM_LFO_WAVE:
					sp->LFO2.waveformType = (uint8_t) val;
					break;

				case PanelControl::CONTROL_PARAM_LFO_TRIGGER:
					sp->LFO2.trigger = (uint8_t) val;
					break;

				case PanelControl::CONTROL_PARAM_LFO_SINGLE_SHOT:
					sp->LFO2.singleShotLFO2 = (uint8_t) val;
					break;
				case PanelControl::CONTROL_PARAM_LFO_OUTPUT_MODE:
					sp->LFO2.outputMode = (uint8_t) val;
					break;

				case PanelControl::CONTROL_PARAM_M1_DESTINATION:
					sp->MIDICon1.destination = (uint8_t) val;
					break;
				case PanelControl::CONTROL_PARAM_M2_DESTINATION:
					sp->MIDICon2.destination = (uint8_t) val;
					break;
				case PanelControl::CONTROL_PARAM_M3_DESTINATION:
					sp->MIDICon3.destination =(uint8_t)  val;
					break;
				case PanelControl::CONTROL_PARAM_M4_DESTINATION:
					sp->MIDICon4.destination =  (uint8_t) val;
					break;
				case PanelControl::CONTROL_PARAM_VELOCITY_DESTINATION:
					sp->MIDIVelocity.destination = (uint8_t) val;
					break;
				case PanelControl::CONTROL_PARAM_NOTE_DESTINATION:
					sp->MIDINote.destination = (uint8_t) val;
					break;
				default:
					break;

			}
		}
	}
	Utilities::updatePatchCRC(patch);
	updatePanel();
	*/
}


void MainComponent::parameterChangeGUI(ParameterData& p)
{
	//DBG("Send param change. PanelControlid: " + String(p.getPanelControlId()) + " page: "  + String(p.getPage()) + " parameter: "  + String(p.getParameter()));

	leavePatchNameEditMode();
	//bool refreshAllSettings = FALSE;
	if (!p.isParameterPageFixed) // if not assigned to a page
	{
		// set the active page
		p.setPage(activeParameterPage);
		switch (p.getPanelControlId())
		{
			default:
				break;
			case PanelControl::PANEL_CONTROL_LFOX_MOD_DEPTH:
				if (p.getSubId() == PanelControl::PARAM_TYPE_MAIN_COMBO)
				{
					// store the current EG for this page, used when we access other EG controls
					this->activeLFO = (uint8_t)  p.getPanelValue();
		//			refreshAllSettings = TRUE;
				}else if  ( p.getSubId() == PanelControl::PARAM_TYPE_SECOND_COMBO)
				{
					p.setPage(PanelControl::CONTROL_GLOBAL);
					p.setParameter(PanelControl::CONTROL_GLOBAL_LFO1_PITCH_TRACK + this->activeLFO);
				}
				break;
			case PanelControl::PANEL_CONTROL_LFOX_FREQ:
				/*
				if (p.getSubId() == PanelControl::PARAM_TYPE_MAIN_COMBO)
				{
					p.setPage(PanelControl::CONTROL_GLOBAL);
					p.setParameter(PanelControl::CONTROL_GLOBAL_LFOX1_FREQ + this->activeLFO);
				} else if  ( p.getSubId() == PanelControl::PARAM_TYPE_SECOND_COMBO)
				{
					p.setPage(PanelControl::CONTROL_GLOBAL);
					p.setParameter(PanelControl::CONTROL_GLOBAL_LFOX1_TRIGGER + this->activeLFO);
				}
				else if  ( p.getSubId() == PanelControl::PARAM_TYPE_MAIN_SLIDER)
				{
					p.setPage(PanelControl::CONTROL_GLOBAL);
					p.setParameter(PanelControl::CONTROL_GLOBAL_LFOX1_FREQ + this->activeLFO);
				}*/

				if  ( p.getSubId() == PanelControl::PARAM_TYPE_MAIN_COMBO)
				{
					p.setPage(PanelControl::CONTROL_GLOBAL);
					p.setParameter(PanelControl::CONTROL_GLOBAL_LFOX1_TRIGGER + this->activeLFO);
				}
				else if  ( p.getSubId() == PanelControl::PARAM_TYPE_MAIN_SLIDER)
				{
					p.setPage(PanelControl::CONTROL_GLOBAL);
					p.setParameter(PanelControl::CONTROL_GLOBAL_LFOX1_FREQ + this->activeLFO);
				}


				break;
			case PanelControl::PANEL_CONTROL_LFOX_WAVEFORM:
				if (p.getSubId() == PanelControl::PARAM_TYPE_MAIN_COMBO)
				{
					p.setPage(PanelControl::CONTROL_GLOBAL);
					p.setParameter(PanelControl::CONTROL_GLOBAL_LFOX1_WAVEFORM + this->activeLFO);

				} else
				{
					p.setPage(PanelControl::CONTROL_GLOBAL);
					p.setParameter(PanelControl::CONTROL_GLOBAL_LFOX1_SINGLE_SHOT + this->activeLFO);
				}
				break;
			case PanelControl::PANEL_CONTROL_EG_MOD_DEPTH:
				if (p.getSubId() == PanelControl::PARAM_TYPE_MAIN_COMBO)
				{
					// store the current EG for this page, used when we access other EG controls
					this->activeEG = (uint8_t) p.getPanelValue();
			//		refreshAllSettings = TRUE;
				}
				break;
			case PanelControl::PANEL_CONTROL_EG_DELAY:

				if (this->activeEG < 4) // global EG adjustment
				{
					p.setPage(PanelControl::CONTROL_GLOBAL); // global
					if ( p.getSubId() == PanelControl::PARAM_TYPE_MAIN_SLIDER)
						p.setParameter(PanelControl::CONTROL_GLOBAL_EGX1_DELAY + this->activeEG);
					else
						p.setParameter(PanelControl::CONTROL_GLOBAL_EGX1_TRIGGER + this->activeEG);
				} else
				{
					if ( p.getSubId() == PanelControl::PARAM_TYPE_MAIN_SLIDER)
						p.setParameter(PanelControl::CONTROL_PARAM_EG_DELAY);
					else
						p.setParameter(PanelControl::CONTROL_PARAM_EG_RETRIG);
				}

				break;
			case PanelControl::PANEL_CONTROL_EG_ATTACK:
					//DBG("EG attack");
				if (this->activeEG < 4) // global EG adjustment
				{
					p.setPage(PanelControl::CONTROL_GLOBAL); // global
					if ( p.getSubId() == PanelControl::PARAM_TYPE_MAIN_SLIDER)
						p.setParameter(PanelControl::CONTROL_GLOBAL_EGX1_ATTACK + this->activeEG);
					else
						p.setParameter(PanelControl::CONTROL_GLOBAL_EGX1_SHAPE_MULTI + this->activeEG);
				} else
				{
					if ( p.getSubId() == PanelControl::PARAM_TYPE_MAIN_SLIDER)
						p.setParameter(PanelControl::CONTROL_PARAM_EG_ATTACK);
					else
						p.setParameter(PanelControl::CONTROL_PARAM_EG_SHAPE_MULTI);
				}
				break;

			case PanelControl::PANEL_CONTROL_EG_DECAY:
			{
				if (this->activeEG < 4) // global EG adjustment
				{
					p.setPage(PanelControl::CONTROL_GLOBAL); // global
					if ( p.getSubId() == PanelControl::PARAM_TYPE_MAIN_SLIDER)
						p.setParameter(PanelControl::CONTROL_GLOBAL_EGX1_DECAY + this->activeEG);
					else
						p.setParameter(PanelControl::CONTROL_GLOBAL_EGX1_RESTART + this->activeEG);
				} else
				{
					if ( p.getSubId() == PanelControl::PARAM_TYPE_MAIN_SLIDER)
						p.setParameter(PanelControl::CONTROL_PARAM_EG_DECAY);
					else
						p.setParameter(PanelControl::CONTROL_PARAM_EG_RESTART);
				}

				if (p.getSubId() > PanelControl::PARAM_TYPE_MAIN_SLIDER)
				{
					uint8_t* pEG;
					if (this->activeEG < 4)
						pEG = &EGRestart[this->activeEG];
					else
						pEG = &EGRestart[4];


					// bitfield
					switch (p.getPanelValue())
					{
					default:
						*pEG &= 0x01; // leave lowest bit alone, all others off
						break;
					case 1:
						*pEG &= 0x01;
						*pEG |= 0x02;
						break;
					case 2:
						*pEG &= 0x01;
						*pEG |= 0x04;
						break;
					}
					/*if (p.getPanelValue() == 0)
						*pEG &= 0x01;
					else
						*pEG |= 0x02;

					*/
DBG("Set panel combo" + String(*pEG));
					p.setValue(*pEG);
				}

				break;
			}
			case PanelControl::PANEL_CONTROL_EG_SUSTAIN:

				if (this->activeEG < 4) // global EG adjustment
				{
					p.setPage(PanelControl::CONTROL_GLOBAL); // global
					if ( p.getSubId() == PanelControl::PARAM_TYPE_MAIN_SLIDER)
						p.setParameter(PanelControl::CONTROL_GLOBAL_EGX1_SUSTAIN + this->activeEG);
					else
						p.setParameter(PanelControl::CONTROL_GLOBAL_EGX1_RESTART + this->activeEG);
				} else
				{
					if ( p.getSubId() == PanelControl::PARAM_TYPE_MAIN_SLIDER)
						p.setParameter(PanelControl::CONTROL_PARAM_EG_SUSTAIN);
					else
						p.setParameter(PanelControl::CONTROL_PARAM_EG_RESTART);
				}

				if (p.getSubId() > PanelControl::PARAM_TYPE_MAIN_SLIDER)
				{
					uint8_t* pEG;
					if (this->activeEG < 4)
						pEG = &EGRestart[this->activeEG];
					else
						pEG = &EGRestart[4];

					if (p.getPanelValue() == 0)
						*pEG &= ~0x01; // turn off lowest bit
					else
						*pEG |= 0x01; // turn on lowest bit

					DBG("Set panel switch" + String(*pEG));


					p.setValue(*pEG);
				}


				break;
			case PanelControl::PANEL_CONTROL_EG_RELEASE:

				if (this->activeEG < 4) // global EG adjustment
				{
					p.setPage(PanelControl::CONTROL_GLOBAL); // global
					if ( p.getSubId() == PanelControl::PARAM_TYPE_MAIN_SLIDER)
					{
						p.setParameter(PanelControl::CONTROL_GLOBAL_EGX1_RELEASE + this->activeEG);
					}
					else
					{
						p.setParameter(PanelControl::CONTROL_GLOBAL_EGX1_UNIPOLAR + this->activeEG);
					}


				} else
				{
					if ( p.getSubId() == PanelControl::PARAM_TYPE_MAIN_SLIDER)
						p.setParameter(PanelControl::CONTROL_PARAM_EG_RELEASE);
					else
						p.setParameter(PanelControl::CONTROL_PARAM_EG_UNIPOLAR);
				}

				break;
		}
	}
	this->activeParameter = &p;
//	holdingWindow->midiProcessor->sendParameterChangeLowLevel(p);


	NRPNmessage_t n = p.convertParameterDataToNRPN();


	holdingWindow->midiProcessor->commandRequest(MIDIProcessor::COMMAND_REQUEST_PARAMETER_CHANGED, n);

	//
/*
	editActivePatch(p);


	//currentValueEdit.setText(String(p.getPanelValue()), NotificationType::dontSendNotification);
	//currentValueName.setText(String(p.getName()), NotificationType::dontSendNotification);

	// show data is edited

	if (activeParameterPage != previousActiveParameterPage || refreshAllSettings == TRUE)
	{
		// update the other controls if the page has changed, read from the patch buffer
		updatePanel((tAllParams*) holdingWindow->appCommon->activePatch);
		previousActiveParameterPage = activeParameterPage;
	} else
	{
		updatePatchComboText();
	}
	*/
}


void MainComponent::setParameterPage(ParameterData &p, int panelControlId)
{

	//if (id != activeParameterPage)
	if (p.isParameterPageFixed)
	{
		this->activeParameterPage = p.getPage();
		this->activeParameter = &p;

		holdingWindow->midiProcessor->commandRequest(MIDIProcessor::COMMAND_REQUEST_GUI_UPDATE, 0);
		//updatePanel();
		//this->midiComms->sendParameterChange(p); // do this to update screen to this page that has been clicked on


		for (int i = 0; i < controlsArray.size(); i++)
		{
			PanelControl* pa = controlsArray[i];
			pa->disableParameterButton();
		}
	}

	this->activePanelControlId = panelControlId;

	//this->midiComms->sendSysEx("bitmap cmd", MIDI_SYSEX_REQUEST_BITMAP_COMPRESSED,  0);


}

void MainComponent::updateMotasScreenshot(uint8_t* imageBuffer, int size)
{

	//DBG("Bitmap size: " + String(size));
	if (size < 4096)
		return;


#if JUCE_MAC || JUCE_IOS

    Graphics g(screenshotImage);

    int x = 0;
    int y = 0;
    int i = 0;
    uint8_t prevData = 0;
    for (i = 0; i < 4096; i++)
    {
        for (int n = 0; n < 2; n++)
        {
            uint8_t val  = imageBuffer[i];
            uint8_t data;
            if (n == 0)
            {
                data = (val & 0x0F) ;
            }
            else
            {
                data = ((val >> 4) & 0x0F) ;
            }
            // appy gamma correction. Incoming is from 0 to 15
            float corrected = sqrtf((float) data) / sqrtf(15);
            // rescale to max
            data = (uint8_t) (corrected * 255.0f);

            if (data != prevData)
            {
                g.setColour(Utilities::getPixelColour(data));
                prevData = data;
            }
            g.fillRect(screenshotScalingFactor*x, screenshotScalingFactor*y, screenshotScalingFactor, screenshotScalingFactor);

            x++;
            if (x >= 128)
            {
                x = 0;
                y++;
            }
        }
    }
#else

/* THIS METHOD DOES NOT WORK ON macOS FOR UNKNOWN REASON, use one above which creates new Graphics each time..

*/
    int x = 0;
	int y = 0;
	int i = 0;
	uint8_t prevData = 0;
	for (i = 0; i < 4096; i++)
	{
		for (int n = 0; n < 2; n++)
		{
			uint8_t val  = imageBuffer[i];
			uint8_t data;
			if (n == 0)
			{
				data = (val & 0x0F) ;
			}
			else
			{
				data = ((val >> 4) & 0x0F) ;
			}

			// appy gamma correction. Incoming is from 0 to 15
			float corrected = sqrtf((float) data) / sqrtf(15);

			// rescale to max
			data = (uint8_t) (corrected * 255.0f);


			if (data != prevData)
			{
				graphicsScreenshot->setColour(Utilities::getPixelColour(data));
				prevData = data;
			}


			graphicsScreenshot->fillRect(screenshotScalingFactor*x, screenshotScalingFactor*y, screenshotScalingFactor, screenshotScalingFactor);

			x++;
			if (x >= 128)
			{
				x = 0;
				y++;
			}
		}
	}
#endif
	motasScreenshot->repaint();
}



void MainComponent::buttonClicked(Button* b)
{

	if (b == &sendAllPatchSettingsButton)
	{
		uint8_t index = (uint8_t) patchesCombo.getSelectedItemIndex();
		//holdingWindow->midiProcessor->sendPatch(0, holdingWindow->appCommon->activePatch);
		holdingWindow->midiProcessor->commandRequest(MIDIProcessor::COMMAND_REQUEST_PATCH_CHANGE, index);
	} else if (b == &requestAllPatchSettingsButton)
	{
		//holdingWindow->midiProcessor->sendSysEx("get patch cmd", MIDI_SYSEX_REQUEST_PATCH, 0);
		holdingWindow->midiProcessor->commandRequest(MIDIProcessor::COMMAND_REQUEST_PATCH, 0);
	} else if (b == &toggleButtonLiveScreenShot)
	{

		holdingWindow->appCommon->saveAppPropertyBool
										(ApplicationCommon::APP_PROPERTY_LIVE_SCREENSHOT, b->getToggleState());

	}
	else if (b == &toggleButtonTestMIDI)
	{
#if INCLUDE_TEST_FUNCTIONS == 1

	#define NUM_NOTES 1
#define BUFFER_LEN (NUM_NOTES * 12)
	   uint8_t buffer[BUFFER_LEN];

	   int i = 0;
	   uint8_t ch =  holdingWindow->midiProcessor->getMIDIChannel() -1;
	   for (int j = 0; j < NUM_NOTES; j++)
	   {
		   uint8_t note = 0x3D  + j;


		   buffer[i++] = 0x90 | ch; // note on
		   buffer[i++] = note;
		   buffer[i++] =  0x40;


		   buffer[i++] = 0x90 | ch; // note on
		   buffer[i++] = note;
		   buffer[i++] =  0x40;



		   buffer[i++] = 0x80 | ch; // note off
		   buffer[i++] = note;
		   buffer[i++] =  0;

		   buffer[i++] = 0x80 | ch; // note off
		   buffer[i++] = note;
		   buffer[i++] =  0;


	   }

	   bool retry =  holdingWindow->midiProcessor->commandRequest(
			   MIDIProcessor::COMMAND_REQUEST_TEST_MIDI, buffer, BUFFER_LEN);
	   if (retry)
	   {
		   DBG("BUSY Keyboard note ON");
	   }

#endif
	}
	else if (b == &toggleButtonChangePage)
	{
		//holdingWindow->midiProcessor->autoPageChange = toggleButtonChangePage.getToggleState();
		holdingWindow->appCommon->saveAppPropertyBool
									(ApplicationCommon::APP_PROPERTY_ENABLE_AUTO_PAGE_CHANGE, b->getToggleState());
	}
	else if (b == &toggleButtonMonitorOnly)
		{
			//holdingWindow->midiProcessor->autoPageChange = toggleButtonChangePage.getToggleState();
			holdingWindow->appCommon->saveAppPropertyBool
										(ApplicationCommon::APP_PROPERTY_ENABLE_SHOW_MONITOR_ONLY, b->getToggleState());
		}
	else if (b == &increaseButton
			|| b == &decreaseButton
			|| b == &decreaseButtonBiggerSteps
			|| b == &increaseButtonBiggerSteps)
	{


	/*	if (currentValueEdit.getText().isEmpty())
			return;

		int32_t value = currentValueEdit.getText().getIntValue();

		DBG("Max: " + String(activeParameter->getMax()) + " min: " + String(activeParameter->getMin()));
*/

		if (this->activeParameter != nullptr)
		{
			int32_t value = activeParameter->getPanelValue();

			int32_t inc;
			if (b == &increaseButtonBiggerSteps || b == &decreaseButtonBiggerSteps)
				inc = activeParameter->getIncrementStep(true);
			else
				inc = activeParameter->getIncrementStep(false);


			if (b == &increaseButton || b == &increaseButtonBiggerSteps)
				value += inc;
			else
				value -= inc;

			activeParameter->setValueFromPatch(value);
			parameterChangeGUI(*this->activeParameter);

		}


		/*int range = activeParameter->getMax() - activeParameter->getMin();

		int change =  range / 50;
		if (change <= 0)
			change = 1;
		if (b == &increaseButton)
			value += change;
		else
			value -= change;
		if (this->activeParameter != nullptr)
		{
			this->activeParameter->setValueFromPatch(value);
			parameterChangeGUI(*this->activeParameter);
		}*/

	} else if (b == &zeroParameter)
	{
		if (this->activeParameter != nullptr)
		{
			//this->activeParameter->setValueFromPatch(0);
			activeParameter->setToDefault();

			parameterChangeGUI(*this->activeParameter);
		}
	} else if (b == &resetPage)
	{
		if (this->activeParameter != nullptr)
		{

			NRPNmessage_t n = activeParameter->convertParameterDataToNRPN();
			//holdingWindow->midiProcessor->sendPatch(0, holdingWindow->appCommon->activePatch);
			holdingWindow->midiProcessor->commandRequest(MIDIProcessor::COMMAND_REQUEST_RESET_PARAMETER_PAGE, n);
		}
	} else if (b == &this->savePatchChangeButton)
	{
#if BUILD_STANDALONE_EXECUTABLE == 0
	ApplicationCommon* a = holdingWindow->appCommon;
#else
	ApplicationCommon* a = holdingWindow->appCommon.get();
#endif
		int index = patchesCombo.getSelectedItemIndex();
		DBG("Save patch " + String(index));

		a->storeEditedPatch(index);
		updatePanel();
		updatePresetComboBox(false);
#if BUILD_STANDALONE_EXECUTABLE == 1
		holdingWindow->appCommon->savePresets();
#endif

	}
	else if (b == &this->restorePatchChangeButton)
	{
#if BUILD_STANDALONE_EXECUTABLE == 0
	ApplicationCommon* a = holdingWindow->appCommon;
#else
	ApplicationCommon* a = holdingWindow->appCommon.get();
#endif
		int index = patchesCombo.getSelectedItemIndex();
		DBG("Restore patch " + String(index));
		a->restoreEditedPatch(index);
		updatePanel();
		updatePresetComboBox(false);
		buttonClicked(&sendAllPatchSettingsButton);

	} else if (b == &renamePatchButton)
	{
		if (renamePatchButton.getToggleState())
		{
			editingComboPatchName  = true;
			patchesCombo.setEditableText(true);
			patchesCombo.setText(holdingWindow->appCommon->getCurrentPatchName());
			patchesCombo.showEditor();
		} else
		{
			editingComboPatchName = false;
			//holdingWindow->appCommon->changeCurrentPatchName(patchesCombo.getText());
			changePatchName(patchesCombo.getText());
			leavePatchNameEditMode();

		}
	}

}

void MainComponent::updatePanel()
{


	DBG("MainComponent Update panel");

#if BUILD_STANDALONE_EXECUTABLE == 0
	ApplicationCommon* a = holdingWindow->appCommon;
	PatchSetup* patchSetup = holdingWindow->patchSetup.get();

#else
	ApplicationCommon* a = holdingWindow->appCommon.get();
	PatchSetup* patchSetup = holdingWindow->patchSetup.get();
#endif
	AdvancedModSetup* advancedModSetupSetup = holdingWindow->advancedModSetup.get();

	const tAllParams* patch = a->getGUIPatch();



	for (int i = 0; i < NUMBER_OF_CC_MAPPINGS; i++)
	{
		int page = this->getCCPage(0, (uint8_t) i);
		int dest = this->getCCParam(0, (uint8_t) i);

		patchSetup->updateCCMapping(i, page, dest);
	}

	for (int i = 0; i < NUMBER_OF_MODULATION_OF_MODULATIONS; i++)
	{
		advancedModSetupSetup->updateAdvancedModSettings(i,
				&patch->params.singleParams[i].more.lfo2Mod_And_ModofMod.modOfmodulation, patch->advancedModulationMode);
	}



	nameEdit->setText(a->getCurrentPatchName(), NotificationType::dontSendNotification);
	DBG("Update panel");
	//tAllParams* patch = &this->activePatch;

	updatePatchComboText();

/*	if (buffer != nullptr)
	{
		memcpy((void*) patch, buffer, PATCH_LENGTH_BYTES_UNPACKED);
	}*/
	// now set the controls to match the values!
	const singleParam* sp;
	int page = this->activeParameterPage;

	if (page > 0 && page <= 29)
		sp = &patch->params.singleParams[page - 1];
	else if (page > 29 && page < 35)
		sp = &patch->params.singleParams[page - 2];
	else
		sp = &patch->params.paramNames.VcaMaster;

	activeEG = sp->EG1.source;
	activeLFO = sp->LFO1.source;





	const int32_t VALUE_NOT_USED = 0x7FFFFFFF;
	for (int i = 0; i < controlsArray.size(); i++)
	{
		ParameterData* p = &controlsArray[i]->paramDataMain;
		int32_t val, val2ndSlider, val3rdSlider, valCombo, valCombo2, valButton;
		val = VALUE_NOT_USED;
		val2ndSlider = val;
		val3rdSlider = val;
		valCombo = val;
		valCombo2 = val;
		valButton = val;

		if (!p->isParameterPageFixed)
			page = this->activeParameterPage;
		else
			page = p->getPage();
		//	p->setPage(this->activeParameterPage);

		int parameter = p->getParameter();
		if (page > 0 && page <= 29)
			sp = &patch->params.singleParams[page - 1];
		else if (page > 29 && page < 35)
			sp = &patch->params.singleParams[page - 2];
		else
			sp = &patch->params.paramNames.VcaMaster;
		if (p->isParameterPageFixed)
		{
			switch (parameter)
			{
				case PanelControl::CONTROL_PARAM_OFFSET:
				{
					//val = sp->offsetIncludingPot;
					val = sp->basicOffset  + sp->potentiometerValue;
			//		DBG("Value: "+ String(val) + " Page: " + String(page) + " parameter:" + String(parameter) + " sub id:" + String(subId) + " pageFixed?:" + String((int) pageFixed));
					if (page == PanelControl::PANEL_CONTROL_OSC1_PITCH
							|| page == PanelControl::PANEL_CONTROL_OSC2_PITCH
							|| page == PanelControl::PANEL_CONTROL_OSC3_PITCH)
						val /= 3;
					p->setValueFromPatch(val);

					valCombo = sp->option1;
					ParameterData* p2 = &controlsArray[i]->paramDataComboBox;
					p2->setValueFromPatch(valCombo); // combobox
					valCombo2 = sp->option2.value;
					p2 = &controlsArray[i]->paramDataComboBoxSecondary;
					p2->setValueFromPatch(valCombo2); // combobox



	//				if (page == this->activeParameterPage)
	//					this->midiComms->sendParameterChange(p); // do this to update screen to this page that has been clicked on
					break;
				}
				default:
					break;
			}
		} else // not a fixed page
		{
			switch (p->getPanelControlId())
			{

				case PanelControl::PANEL_CONTROL_LFO_FREQ:
					val = sp->LFO2.freq;
					valCombo = sp->LFO2.trigger;
					val2ndSlider = sp->more.lfo2Mod_And_ModofMod.LFO2_freqModFromLFO1;
					val3rdSlider = sp->more.lfo2Mod_And_ModofMod.LFO2_freqModFromEG;


					break;
				case PanelControl::PANEL_CONTROL_LFO_WAVEFORM:
					valCombo = sp->LFO2.waveformType;
					valButton = sp->LFO2.extras.singleShotLFO2;
					break;
				case PanelControl::PANEL_CONTROL_LFO_MOD_DEPTH:
					valCombo = sp->LFO2.outputMode;
					val = sp->LFO2.modDepth;
					valCombo2 = sp->LFO2.extras.pitchTrackLFO2;
				//	DBG("Set LFO choice: " + String(valCombo));
					break;
				case PanelControl::PANEL_CONTROL_EG_DELAY:
					if (activeEG < 4)
					{
						val = patch->commonEGs[this->activeEG].delay;
						valCombo = patch->commonEGs[this->activeEG].egRetrig;
					}
					else
					{
						val = sp->EG0.delay;
						valCombo = sp->EG0.egRetrig;
					}
					break;
				case PanelControl::PANEL_CONTROL_EG_ATTACK:
					if (activeEG < 4)
					{
						if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
							val = patch->commonEGs[this->activeEG].attack;
						else
							val = Utilities::convertEGTimeToValue(patch->commonEGs[this->activeEG].attack);

						val2ndSlider = patch->commonEGs[this->activeEG].egShape;;
					}
					else
					{
						if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
							val = sp->EG0.attack;
						else
							val = Utilities::convertEGTimeToValue(sp->EG0.attack);

						val2ndSlider = sp->EG0.egShape;
					}
					break;
				case PanelControl::PANEL_CONTROL_EG_DECAY:
					if (activeEG < 4)
					{
						if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
							val = patch->commonEGs[this->activeEG].decay;
						else
							val =Utilities::convertEGTimeToValue( patch->commonEGs[this->activeEG].decay);
						//valButton = patch->commonEGs[this->activeEG].egRestartOnTrigger & 0x02;
						switch (patch->commonEGs[this->activeEG].egRestartOnTrigger >> 1)
						{
						default:
							valCombo = 0;
							break;
						case 1:
							valCombo = 1;
							break;
						case 2:
						case 3:
							valCombo = 2;
							break;
						}

					}
					else
					{
						if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
							val = sp->EG0.decay;
						else
							val = Utilities::convertEGTimeToValue(sp->EG0.decay);
						//valButton = sp->EG0.egRestartOnTrigger & 0x02;

						switch (sp->EG0.egRestartOnTrigger >> 1)
						{
						default:
							valCombo = 0;
							break;
						case 1:
							valCombo = 1;
							break;
						case 2:
						case 3:
							valCombo = 2;
							break;
						}
					}
					break;
				case PanelControl::PANEL_CONTROL_EG_SUSTAIN:
					if (activeEG < 4)
					{
						val = patch->commonEGs[this->activeEG].sustain;
						valButton = patch->commonEGs[this->activeEG].egRestartOnTrigger & 0x01;
					}
					else
					{
						val = sp->EG0.sustain;
						valButton = sp->EG0.egRestartOnTrigger & 0x01;
					}
					break;
				case PanelControl::PANEL_CONTROL_EG_RELEASE:
					if (activeEG < 4)
					{
						if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
							val = patch->commonEGs[this->activeEG].release;
						else
							val = Utilities::convertEGTimeToValue(patch->commonEGs[this->activeEG].release);
						valButton = patch->commonEGs[this->activeEG].egUnipolar;
					}
					else
					{
						if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
							val = sp->EG0.release;
						else
							val = Utilities::convertEGTimeToValue(sp->EG0.release);
						valButton = sp->EG0.egUnipolar;
					}
					break;
				case PanelControl::PANEL_CONTROL_EG_MOD_DEPTH:
					val = sp->EG1.modDepth;
					if (activeEG < 4)
						valCombo = activeEG + 1;
					else
						valCombo = 0;
			//		DBG("Set EG choice: " + String(valCombo));
					//DBG("Value: "+ String(val) + " Page: " + String(page) + " parameter:" + String(parameter) + " sub id:" + String(subId) + " pageFixed?:" + String((int) pageFixed));
					break;


				case PanelControl::PANEL_CONTROL_LFOX_FREQ:
					if (activeLFO < 4)
					{
						val = patch->commonLFOs[this->activeLFO].freq;
						//valCombo = val;
						//valCombo2 = patch->commonLFOs[this->activeLFO].trigger;
						valCombo = patch->commonLFOs[this->activeLFO].trigger;

					//	DBG("Set LFOx freq: " + String(val) + " combo: " + String(valCombo));
					}
					break;
				case PanelControl::PANEL_CONTROL_LFOX_WAVEFORM:
					valCombo = patch->commonLFOs[this->activeLFO].waveformType;
					valButton = patch->commonLFOs[this->activeLFO].extras.singleShot;
					break;

				case PanelControl::PANEL_CONTROL_LFOX_MOD_DEPTH:
					val = sp->LFO1.modDepth;
					valCombo = sp->LFO1.source;
					valCombo2 = patch->commonLFOs[this->activeLFO].extras.pitchTrack;
					break;


				case PanelControl::PANEL_CONTROL_NOTE:
					val = sp->MIDINote.modDepth;
					val2ndSlider = sp->MIDINote.modDepthAlternate;
					valCombo = sp->MIDINote.destination;
					valButton = sp->MIDINote.unipolar;
					break;
				case PanelControl::PANEL_CONTROL_VELOCITY:
					val = sp->MIDIVelocity.modDepth;
					val2ndSlider = sp->MIDIVelocity.modDepthAlternate;
					valCombo = sp->MIDIVelocity.destination;
					valButton = sp->MIDIVelocity.unipolar;
					break;

				case PanelControl::PANEL_CONTROL_M1:
					val = sp->MIDICon1.modDepth;
					val2ndSlider = sp->MIDICon1.modDepthAlternate;
					valCombo = sp->MIDICon1.destination;
					valButton = sp->MIDICon1.unipolar;

					break;
				case PanelControl::PANEL_CONTROL_M2:
					val = sp->MIDICon2.modDepth;
					val2ndSlider = sp->MIDICon2.modDepthAlternate;
					valCombo = sp->MIDICon2.destination;
					valButton = sp->MIDICon2.unipolar;
					break;
				case PanelControl::PANEL_CONTROL_M3:
					val = sp->MIDICon3.modDepth;
					val2ndSlider = sp->MIDICon3.modDepthAlternate;
					valCombo = sp->MIDICon3.destination;
					valButton = sp->MIDICon3.unipolar;
					break;
				case PanelControl::PANEL_CONTROL_M4:
					val = sp->MIDICon4.modDepth;
					val2ndSlider = sp->MIDICon4.modDepthAlternate;
					valCombo = sp->MIDICon4.destination;
					valButton = sp->MIDICon4.unipolar;
					break;

				default:
					break;
			}
			if (val != VALUE_NOT_USED)
			{
				p->setValueFromPatch(val); // main slider

			}
			if (val2ndSlider != VALUE_NOT_USED)
			{
				ParameterData* p2 = &controlsArray[i]->paramDataSecondSlider;
				p2->setValueFromPatch(val2ndSlider); // second slider

			}
			if (val3rdSlider != VALUE_NOT_USED)
			{
				ParameterData* p2 = &controlsArray[i]->paramDataThirdSlider;
				p2->setValueFromPatch(val3rdSlider); // third slider

			}
			if (valCombo != VALUE_NOT_USED)
			{
				ParameterData* p2 = &controlsArray[i]->paramDataComboBox;
				p2->setValueFromPatch(valCombo); // combobox
			}
			if (valCombo2 != VALUE_NOT_USED)
			{
				ParameterData* p2 = &controlsArray[i]->paramDataComboBoxSecondary;
				p2->setValueFromPatch(valCombo2); // combobox
			}
			if (valButton != VALUE_NOT_USED)
			{
				ParameterData* p2 = &controlsArray[i]->paramDataButton;
				p2->setValueFromPatch(valButton); // combobox
			}
		}
	}
}
void MainComponent::changePatchName(String text)
{


	if (0 != oldName.compare(text))
	{
		oldName = text;
		Utilities::populateNameBuffer(text, patchName);

		DBG("Update patch name");


		holdingWindow->midiProcessor->commandRequest(MIDIProcessor::COMMAND_REQUEST_PATCH_NAME_CHANGE,
					(uint8_t*) patchName, NAME_LENGTH + 1);

			/*
		int32_t value = t.getText().getIntValue();

		DBG("Text: " + t.getText() + " value: " + String(value));

		if (this->activeParameter != nullptr)
		{
			this->activeParameter->setValueFromPatch(value);
			parameterChangeGUI(*this->activeParameter);
		}
	*/
	} else
	{
		DBG("Patch name UNCHANGED");
	}

}


void MainComponent::sendProgramChange(uint8_t newProgram)
{

	holdingWindow->midiProcessor->commandRequest(MIDIProcessor::COMMAND_REQUEST_SEND_PROGRAM_CHANGE, newProgram);
}


void MainComponent::mouseWheelMove(const MouseEvent& event,
		const MouseWheelDetails& wheel)
{
	DBG("Mouse wheel  " );

	if (event.eventComponent == &this->currentValueEdit)
	{
		if (currentValueEdit.getText().isEmpty())
			return;

		float change = wheel.deltaY;
		int32_t value = currentValueEdit.getText().getIntValue();

		//DBG("Max: " + String(activeParameter->getMax()) + " min: " + String(activeParameter->getMin()));

		//int range = activeParameter->getMax() - activeParameter->getMin();

		//int change =  range / 50;
		if (change <= 0)
			change = -1;
		else
			change = 1;
		//
		value += (int32_t) change;
		DBG("Mouse wheel change: " + String(change));

		if (this->activeParameter != nullptr)
		{
			this->activeParameter->setValueFromPatch(value);
			parameterChangeGUI(*this->activeParameter);
		}
	}
}

void MainComponent::setColours(Colour c, int indexScreenshot)
{
	this->mainColour = c;
	screenShotColour = indexScreenshot;
}

void MainComponent::comboBoxChanged(ComboBox* c)
{
	if (c == &this->patchesCombo)
	{
		//updatePresetComboBox();


		//holdingWindow->appCommon->setPatchNumber(index);
		//changePatch();
		if (!editingComboPatchName)
			buttonClicked(&sendAllPatchSettingsButton);
	}
}


void MainComponent::updatePatchComboText()
{
#if BUILD_STANDALONE_EXECUTABLE == 0
	ApplicationCommon* a = holdingWindow->appCommon;
#else
	ApplicationCommon* a = holdingWindow->appCommon.get();
#endif

	if (a->isActivePatchEdited())
	{
		updatePresetComboBox(false);
	}

}

void MainComponent::updatePresetComboBox(bool firstTime)
{

	DBG("Update combobox");
#if BUILD_STANDALONE_EXECUTABLE == 0
	ApplicationCommon* a = holdingWindow->appCommon;
#else
	ApplicationCommon* a = holdingWindow->appCommon.get();
#endif
	int currentIndex = a->getPatchNumber();
	//patchesCombo.clear(NotificationType::dontSendNotification);
	for (int i = 0; i < a->patchPresetsArray.size(); ++i)
	{
		int itemId = i + 1;
		bool isEdited = (0 != memcmp(a->patchPresetsArray[i], a->patchPresetsEditedArray[i], PATCH_LENGTH_BYTES_UNPACKED));
		String itemName = a->getPatchName(i);
		String dispName;
		if (i < 9)
			dispName = String((String("[0") + String(itemId) + "] " + itemName));
		else
			dispName= String((String("[") + String(itemId) + "] " + itemName));
		if (isEdited)
			dispName = "*" + dispName;
		if (firstTime)
			patchesCombo.addItem(dispName, itemId);
		else
		{
			patchesCombo.changeItemText(itemId, dispName);
			if (i == currentIndex)
			{
				patchesCombo.setText(dispName, NotificationType::dontSendNotification);
				//DBG("*********** patchesCombo " + String(i) + " " + dispName);
			} else
			{
				//DBG("*********** patchesCombo FAIL " + String(i) + " " + String(patchesCombo.getSelectedItemIndex()));
			}


		}
	}
	if (holdingWindow != nullptr && holdingWindow->patchPatternSequence != nullptr)
	{
		holdingWindow->patchPatternSequence->updatePresetsDisplay();
	}

}



void MainComponent::sliderValueChanged(Slider* slider)
{


	int controllerType = 1;

	if (slider == &modulation1)
	{
		controllerType = 1;

	} else if (slider == &modulation2)
	{
		controllerType = 2;
	}
	else if (slider == &modulation3)
	{
		controllerType = 3;
	}
 	 else if (slider == &modulation4)
	{
 		controllerType = 4;
	}

	uint8_t value = (uint8_t) slider->getValue();

	uint8_t buffer[3];
	buffer[0] = 0xB0;
	buffer[1] = (uint8_t) controllerType;
	buffer[2] = value;
	holdingWindow->midiProcessor->commandRequest(MIDIProcessor::COMMAND_REQUEST_SEND_GUI_NOTES_CONTROLLERS, buffer, 3);


	 /*
	MidiMessage m (MidiMessage::controllerEvent(holdingWindow->midiProcessor->channelMIDI, controllerType, value));
	m.setTimeStamp (Time::getMillisecondCounterHiRes() * 0.001);
	MidiBuffer buf;
	buf.addEvent(m, 1);
	holdingWindow->midiProcessor->sendToOutputs(buf, true, 0, MIDI_DATA_SEND_TYPE_CONTROLLER_FROM_MOTAS_EDIT, "Controller from MotasEdit");
*/




}

void MainComponent::changePatch()
{
	//holdingWindow->midiProcessor->commandRequest(MIDIProcessor::COMMAND_REQUEST_PATCH_CHANGE, buffer, 2);



	//updatePanel();
}

void MainComponent::changePatchFromDAW()
{
	int patchNum = holdingWindow->appCommon->getPatchNumber();
	patchesCombo.setSelectedItemIndex(patchNum, NotificationType::dontSendNotification);
	updatePanel();

}

void MainComponent::restoreParameterValue(ParameterData* p, bool secondarySlider, bool tertiarySlider)
{

	DBG("Restore value");
	const tAllParams* patch = holdingWindow->appCommon->getOriginalPatch();




	const singleParam* sp;
	int page = this->activeParameterPage;

	if (page > 0 && page <= 29)
		sp = &patch->params.singleParams[page - 1];
	else if (page > 29 && page < 35)
		sp = &patch->params.singleParams[page - 2];
	else
		sp = &patch->params.paramNames.VcaMaster;

	activeEG = sp->EG1.source;
	activeLFO = sp->LFO1.source;

	const int32_t VALUE_NOT_USED = 0x7FFFFFFF;


	int32_t val= VALUE_NOT_USED;;
	int32_t val2ndSlider = VALUE_NOT_USED;
	int32_t val3rdSlider = VALUE_NOT_USED;
	//int32_t  valCombo, valCombo2;
//	int32_t  valButton;



	page = p->getPage();
	int parameter = p->getParameter();
	if (page > 0 && page <= 29)
		sp = &patch->params.singleParams[page - 1];
	else if (page > 29 && page < 35)
		sp = &patch->params.singleParams[page - 2];
	else
		sp = &patch->params.paramNames.VcaMaster;
	if (p->isParameterPageFixed)
	{
		switch (parameter)
		{
			case PanelControl::CONTROL_PARAM_OFFSET:
			{
				val = sp->basicOffset  + sp->potentiometerValue;
				if (page == PanelControl::PANEL_CONTROL_OSC1_PITCH
						|| page == PanelControl::PANEL_CONTROL_OSC2_PITCH
						|| page == PanelControl::PANEL_CONTROL_OSC3_PITCH)
					val /= 3;
				p->setValueFromPatch(val);

				break;
			}
			default:
				break;
		}
	} else // not a fixed page
	{
		switch (p->getPanelControlId())
		{
			case PanelControl::PANEL_CONTROL_LFO_FREQ:
				val = sp->LFO2.freq;
				val2ndSlider = sp->more.lfo2Mod_And_ModofMod.LFO2_freqModFromLFO1;
				val3rdSlider = sp->more.lfo2Mod_And_ModofMod.LFO2_freqModFromEG;
				//valCombo = sp->LFO2.trigger;
				break;
			case PanelControl::PANEL_CONTROL_LFO_WAVEFORM:
				//valCombo = sp->LFO2.waveformType;
	//			valButton = sp->LFO2.singleShotLFO2;
				break;
			case PanelControl::PANEL_CONTROL_LFO_MOD_DEPTH:
				//valCombo = sp->LFO2.outputMode;
				val = sp->LFO2.modDepth;
				break;
			case PanelControl::PANEL_CONTROL_EG_DELAY:
				if (activeEG < 4)
				{
					val = patch->commonEGs[this->activeEG].delay;
					//valCombo = patch->commonEGs[this->activeEG].egRetrig;
				}
				else
				{
					val = sp->EG0.delay;
					//valCombo = sp->EG0.egRetrig;
				}
				break;
			case PanelControl::PANEL_CONTROL_EG_ATTACK:
				if (activeEG < 4)
				{
					if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
						val = patch->commonEGs[this->activeEG].attack;
					else
						val = Utilities::convertEGTimeToValue(patch->commonEGs[this->activeEG].attack);
					val2ndSlider = patch->commonEGs[this->activeEG].egShape;
				//	valCombo = patch->commonEGs[this->activeEG].egShape;
				}
				else
				{
					if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
						val = sp->EG0.attack;
					else
						val = Utilities::convertEGTimeToValue(sp->EG0.attack);
					val2ndSlider = sp->EG0.egShape;
				//	valCombo = sp->EG0.egShape;
				}
				break;
			case PanelControl::PANEL_CONTROL_EG_DECAY:
				if (activeEG < 4)
				{
					if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
						val = patch->commonEGs[this->activeEG].decay;
					else
						val =Utilities::convertEGTimeToValue( patch->commonEGs[this->activeEG].decay);
				//	valButton = patch->commonEGs[this->activeEG].egRestartOnTrigger & 0x02;
				}
				else
				{
					if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
						val = sp->EG0.decay;
					else
						val = Utilities::convertEGTimeToValue(sp->EG0.decay);
				//	valButton = sp->EG0.egRestartOnTrigger & 0x02;

				}
				break;
			case PanelControl::PANEL_CONTROL_EG_SUSTAIN:
				if (activeEG < 4)
				{
					val = patch->commonEGs[this->activeEG].sustain;
				//	valButton = patch->commonEGs[this->activeEG].egRestartOnTrigger & 0x01;
				}
				else
				{
					val = sp->EG0.sustain;
				//	valButton = sp->EG0.egRestartOnTrigger & 0x01;
				}
				break;
			case PanelControl::PANEL_CONTROL_EG_RELEASE:
				if (activeEG < 4)
				{
					if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
						val = patch->commonEGs[this->activeEG].release;
					else
						val = Utilities::convertEGTimeToValue(patch->commonEGs[this->activeEG].release);
				}
				else
				{
					if (patch->header.versionNumber >= T_ALL_PARAMS_VERSION_7)
						val = sp->EG0.release;
					else
						val = Utilities::convertEGTimeToValue(sp->EG0.release);
				}
				break;




			case PanelControl::PANEL_CONTROL_EG_MOD_DEPTH:
				val = sp->EG1.modDepth;
	/*			if (activeEG < 4)
					valCombo = activeEG + 1;
				else
					valCombo = 0;*/
		//		DBG("Set EG choice: " + String(valCombo));
				//DBG("Value: "+ String(val) + " Page: " + String(page) + " parameter:" + String(parameter) + " sub id:" + String(subId) + " pageFixed?:" + String((int) pageFixed));
				break;


			case PanelControl::PANEL_CONTROL_LFOX_FREQ:
				if (activeLFO < 4)
				{
					val = patch->commonLFOs[this->activeLFO].freq;
			//		valCombo = val;
			//		valCombo2 = patch->commonLFOs[this->activeLFO].trigger;

				//	DBG("Set LFOx freq: " + String(val) + " combo: " + String(valCombo));
				}
				break;
			case PanelControl::PANEL_CONTROL_LFOX_WAVEFORM:
			//	valCombo = patch->commonLFOs[this->activeLFO].waveformType;
			//	valButton = patch->commonLFOs[this->activeLFO].singleShot;
				break;

			case PanelControl::PANEL_CONTROL_LFOX_MOD_DEPTH:
				val = sp->LFO1.modDepth;
		//		valCombo = sp->LFO1.source;
				break;


			case PanelControl::PANEL_CONTROL_NOTE:
				val = sp->MIDINote.modDepth;
				val2ndSlider = sp->MIDINote.modDepthAlternate;
			//	valCombo = sp->MIDINote.destination;
				break;
			case PanelControl::PANEL_CONTROL_VELOCITY:
				val = sp->MIDIVelocity.modDepth;
				val2ndSlider = sp->MIDIVelocity.modDepthAlternate;
			//	valCombo = sp->MIDIVelocity.destination;
				break;

			case PanelControl::PANEL_CONTROL_M1:
				val = sp->MIDICon1.modDepth;
				val2ndSlider = sp->MIDICon1.modDepthAlternate;
			//	valCombo = sp->MIDICon1.destination;

				break;
			case PanelControl::PANEL_CONTROL_M2:
				val = sp->MIDICon2.modDepth;
				val2ndSlider = sp->MIDICon2.modDepthAlternate;
			//	valCombo = sp->MIDICon2.destination;
				break;
			case PanelControl::PANEL_CONTROL_M3:
				val = sp->MIDICon3.modDepth;
				val2ndSlider = sp->MIDICon3.modDepthAlternate;
			//	valCombo = sp->MIDICon3.destination;
				break;
			case PanelControl::PANEL_CONTROL_M4:
				val = sp->MIDICon4.modDepth;
				val2ndSlider = sp->MIDICon4.modDepthAlternate;
			//	valCombo = sp->MIDICon4.destination;
				break;

			default:
				break;
		}
		if (val != VALUE_NOT_USED)
		{
			if (tertiarySlider)
			{
				if (val3rdSlider != VALUE_NOT_USED)
					p->setValueFromPatch(val3rdSlider); // main slider
			}
			else if (secondarySlider)
			{
				if (val2ndSlider != VALUE_NOT_USED)
					p->setValueFromPatch(val2ndSlider); // main slider
			}
			else
				p->setValueFromPatch(val); // main slider
		}
	}
}

void MainComponent::adjustFontToFit()
{
	//float size = currentValueEdit.getHeight();
/*	float len = currentValueEdit.getText().length();
	if (len < 6.0f)
		len = 6.0f;*/

//	currentValueEdit.applyFontToAllText(4.0f * size / len, true);

}

void MainComponent::textEditorFocusLost(TextEditor& t)
{
	textEditorReturnKeyPressed(t);
}

void MainComponent::leavePatchNameEditMode()
{


	//patchesCombo.hidePopup();
	patchesCombo.setEditableText(false);
	updatePresetComboBox(false);
	renamePatchButton.setToggleState(false, NotificationType::dontSendNotification);

}

void MainComponent::setComboBoxIndex(int index)
{
	this->patchesCombo.setSelectedItemIndex(index, NotificationType::sendNotification);
}
/*
bool MainComponent::keyPressed(const KeyPress& key,
		Component* originatingComponent)
{
	patchesCombo.setSelectedItemIndex(2);
	return false;

}*/




static uint8_t getCCPageLL(uint8_t controller, t_CCControlStruct *s)
{
	uint8_t page = 0;
	uint8_t index = (controller * 3UL) / 2;
	if (index < NUMBER_OF_CC_MAPPINGS_BYTES)
	{
		if (controller % 2 == 0)
		{
			page = s->mapByte[index] >> 2;
		} else
		{
			page = ((s->mapByte[index] & 0x0F) << 2) | ((s->mapByte[index + 1] & 0xC0) >> 6);
		}
	}
	return page;
}


static uint8_t getCCParamLL(uint8_t controller, t_CCControlStruct *s)
{
	uint8_t param = 0;
	uint8_t index = (controller * 3UL) / 2;
	if (index < NUMBER_OF_CC_MAPPINGS_BYTES)
	{
		if (controller % 2 == 0)
		{
			param = ((s->mapByte[index] & 0x03) << 4) | ((s->mapByte[index + 1] & 0xF0) >> 4);
		} else
		{
			param = s->mapByte[index + 1] & 0x3F;
		}
	}
	return param;
}

static void setCCPageLL(t_CCControlStruct *s, uint8_t controller, uint8_t page)
{
	uint8_t index = (controller * 3UL) / 2;
	if (index < NUMBER_OF_CC_MAPPINGS_BYTES)
	{
		page &= 0x3F;// limit to 6 bits
		if (controller % 2 == 0)
		{
			s->mapByte[index] = (page << 2) | (s->mapByte[index] & 0x03);
		} else
		{
			s->mapByte[index] = (page >> 2) | (s->mapByte[index] & 0xF0);
			s->mapByte[index + 1] = ((page & 0x03) << 6) | (s->mapByte[index + 1] & 0x3F);
		}
	}
}

static void setCCParamLL(t_CCControlStruct *s, uint8_t controller, uint8_t param)
{
	uint8_t index = (controller * 3UL) / 2;
	if (index < NUMBER_OF_CC_MAPPINGS_BYTES)
	{
		param &= 0x3F;// limit to 6 bits
		if (controller % 2 == 0)
		{
			s->mapByte[index] = (param >> 4) | (s->mapByte[index] & 0xFC);
			s->mapByte[index + 1] = (param << 4) | (s->mapByte[index + 1] & 0x0F);
		} else
		{
			s->mapByte[index + 1] = param  | (s->mapByte[index + 1] & 0xC0);
		}
	}
}




tAllParams * MainComponent::getPatch()
{


#if BUILD_STANDALONE_EXECUTABLE == 0
	ApplicationCommon* a = holdingWindow->appCommon;
#else
	ApplicationCommon* a = this->holdingWindow->appCommon.get();
#endif

	tAllParams* patch = (tAllParams*) a->getActivePatch();


	return patch;
}

t_CCControlStruct * MainComponent::getCCControl(uint8_t mode)
{



	tAllParams* patch = getPatch();


	t_CCControlStruct *ccControl = nullptr;
	switch (mode)
	{

		case 0: // patch mode
	//		ccControl = &gV.patchInUse->params.paramNames.patchCCControl;
			ccControl = &patch->params.paramNames.patchCCControl;
			break;
		default:
		case 1:
	//		ccControl = &gGlobalData.globalCCControl;
			break;
	}
	return ccControl;
}





void MainComponent::setCCPage(uint8_t mode, uint8_t controller, uint8_t page)
{

	DBG("Set CC page " + String(page));
	t_CCControlStruct* s = getCCControl(mode);
	if (s != nullptr)
	{
		setCCPageLL(s, controller, page);

		indicatePatchDataChanged();
	}

/*
#if BUILD_STANDALONE_EXECUTABLE == 0
	return this->pluginProcessor->appCommonPtr;
#else
	ApplicationCommon* a = this->holdingWindow->appCommon.get();
#endif


	a->updateActivePatchCRC();
*/


}




void MainComponent::setCCParam(uint8_t mode, uint8_t controller, uint8_t param)
{
	DBG("Set CC param " + String(param));

	t_CCControlStruct* s = getCCControl(mode);
	if (s != nullptr)
	{
		setCCParamLL(s, controller, param);
		indicatePatchDataChanged();
	}
/*
#if BUILD_STANDALONE_EXECUTABLE == 0
	return this->pluginProcessor->appCommonPtr;
#else
	ApplicationCommon* a = this->holdingWindow->appCommon.get();
#endif


	a->updateActivePatchCRC();

*/


}

uint8_t MainComponent::getCCPage(uint8_t mode, uint8_t controller)
{
	t_CCControlStruct* s = getCCControl(mode);
	return getCCPageLL(controller, s);
}

uint8_t MainComponent::getCCParam(uint8_t mode, uint8_t controller)
{
	t_CCControlStruct* s = getCCControl(mode);
	return getCCParamLL(controller, s);
}





