
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
 * MIDIComms.cpp
 *

 */

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
/**
 *
 * Deals with the MIDI communication stuff
 */


MIDIComms::MIDIComms(HoldingWindowComponent* parent, LookAndFeel* l) //: midiKeyboard (keyboardState, MidiKeyboardComponent::horizontalKeyboard)
: holdingWindow(parent),
  midiInputSelector(parent->midiProcessor->midiInputSelector.get()),
  midiOutputSelector(parent->midiProcessor->midiOutputSelector.get()),
  midiInputSelectorMotas(parent->midiProcessor->midiInputSelectorMotas.get()),
  midiOutputSelectorMotas(parent->midiProcessor->midiOutputSelectorMotas.get())
{

	//jassert(1 == 0);

	std::unique_ptr<Drawable> image =  Drawable::createFromImageData(MotasEditResources::motasEdit_256_png,MotasEditResources::motasEdit_256_pngSize);


	if (image.get() == nullptr)
	{
		std::unique_ptr<DrawableText> t = std::make_unique<DrawableText>();
		t->setText("Image N/A!");
		image = std::move(t); // move ownership
	}

	buttonSize = 12;

	int i = 0;
	MIDIroutingBitfield = 0;


#if BUILD_STANDALONE_EXECUTABLE == 0

	guiSizeLabel.setLookAndFeel(l);
	addAndMakeVisible(guiSizeLabel);

	guiSizeCombo.setLookAndFeel(l);
	guiSizeCombo.setTooltip("Choose size of this plugin UI (close and re-open to see the change)");
	addAndMakeVisible(guiSizeCombo);

	guiSizeCombo.addItem("tiny (640 x 480 pixels)", ++i);
	guiSizeCombo.addItem("small (800 x 600 pixels)", ++i); // x 0.75
	guiSizeCombo.addItem("medium (1024 x 768 pixels)", ++i);
	guiSizeCombo.addItem("large (1200 x 900 pixels)", ++i);
	guiSizeCombo.addItem("large wide (1280 x 720 pixels)", ++i);
	guiSizeCombo.addItem("large x-wide (1600 x 900 pixels)", ++i);
	guiSizeCombo.addItem("x-large (1920 x 1080 pixels)", ++i);
	guiSizeCombo.addListener(this);


#endif

	i = 0;
	colourSelectorScreenshot.addItem("white", ++i);
	colourSelectorScreenshot.addItem("yellow", ++i);
	colourSelectorScreenshot.addItem("red", ++i);
	colourSelectorScreenshot.addItem("green", ++i);
	colourSelectorScreenshot.addItem("blue", ++i);
	colourSelectorScreenshot.addItem("blue LCD", ++i);
	colourSelectorScreenshot.addItem("mono LCD", ++i);

	deviceCheckDivider = 8;


	fileNameLocationDefault = String("/");
#if BUILD_STANDALONE_EXECUTABLE == 1

	motasEditLogoButton = std::make_unique<DrawableButton>("motasEditLogo", DrawableButton::ImageFitted);
	motasEditLogoButton->setImages(image.get(), nullptr, nullptr, nullptr, nullptr, nullptr , nullptr, nullptr);
	addAndMakeVisible(motasEditLogoButton.get());

	//MIDISendRate = 20000;
	//MIDISendIntervalMilliSeconds = 1;

	midiInputSelector  =  std::make_unique<MIDIDeviceListBox> ("Midi Input Selector",  *holdingWindow->midiProcessor, true, false);
	midiOutputSelector = std::make_unique<MIDIDeviceListBox> ("Midi Output Selector", *holdingWindow->midiProcessor, false, false);
	midiInputSelector->setTooltip("Choose MIDI input(s) to connect to (DAW and MIDI controllers)");
	midiOutputSelector->setTooltip("Choose MIDI output(s) to connect to (DAW)");
	addLabelAndSetStyle (midiInputLabel);
	addLabelAndSetStyle (midiOutputLabel);
	midiInputLabel.setLookAndFeel(l);
	midiOutputLabel.setLookAndFeel(l);
	addAndMakeVisible (midiInputSelector.get());
	addAndMakeVisible (midiOutputSelector.get());


	addLabelAndSetStyle(routingMatrixLabel);
	routingMatrixLabel.setLookAndFeel(l);


	addLabelAndSetStyle(sysExLabel);
	sysExLabel.setLookAndFeel(l);
	addLabelAndSetStyle(dawOutput);
	dawOutput.setLookAndFeel(l);
	addLabelAndSetStyle(motasOutput);
	motasOutput.setLookAndFeel(l);
	addLabelAndSetStyle(dawOutput2);
	dawOutput2.setLookAndFeel(l);
	addLabelAndSetStyle(motasOutput2);
	motasOutput2.setLookAndFeel(l);

	addLabelAndSetStyle(dawInput);
	dawInput.setLookAndFeel(l);
	dawInput.setJustificationType(Justification::centredRight);

	addLabelAndSetStyle(motasEdit);
	motasEdit.setLookAndFeel(l);

	motasEdit.setJustificationType(Justification::centredRight);


	addLabelAndSetStyle(dawOutput3);
	dawOutput3.setLookAndFeel(l);
	addLabelAndSetStyle(motasInput);
	motasInput.setLookAndFeel(l);
	motasInput.setJustificationType(Justification::centredRight);

#else


	addLabelAndSetStyle(routingMatrixLabel);
	routingMatrixLabel.setLookAndFeel(l);



	addLabelAndSetStyle(sysExLabel);
	sysExLabel.setLookAndFeel(l);
	addLabelAndSetStyle(dawOutput);
	dawOutput.setLookAndFeel(l);
	addLabelAndSetStyle(motasOutput);
	motasOutput.setLookAndFeel(l);
	addLabelAndSetStyle(dawOutput2);
	dawOutput2.setLookAndFeel(l);
	addLabelAndSetStyle(motasOutput2);
	motasOutput2.setLookAndFeel(l);

	addLabelAndSetStyle(dawInput);
	dawInput.setLookAndFeel(l);
	dawInput.setJustificationType(Justification::centredRight);

	addLabelAndSetStyle(motasEdit);
	motasEdit.setLookAndFeel(l);

	motasEdit.setJustificationType(Justification::centredRight);


	addLabelAndSetStyle(dawOutput3);
	dawOutput3.setLookAndFeel(l);
	addLabelAndSetStyle(motasInput);
	motasInput.setLookAndFeel(l);
	motasInput.setJustificationType(Justification::centredRight);



#endif

	addAndMakeVisible(midiChannelLabel);


	addLabelAndSetStyle (midiInputMotasLabel);
	addLabelAndSetStyle (midiOutputMotasLabel);
	autoConnectMotasDevice.setLookAndFeel(l);

	midiInputMotasLabel.setLookAndFeel(l);
	midiOutputMotasLabel.setLookAndFeel(l);
	autoConnectMotasDevice.setLookAndFeel(l);
	addAndMakeVisible(autoConnectMotasDevice);
	addAndMakeVisible (midiInputSelectorMotas);
	addAndMakeVisible (midiOutputSelectorMotas);



	for (i = 0; i < 16; i++)
	{
		midiChannelCombo.addItem(String(i+1), 1 + i);
	}
	midiChannelCombo.setSelectedItemIndex(0, NotificationType::dontSendNotification);
	midiChannelCombo.setTooltip("Set MIDI channel for SysEx and NRPN (for both IN and OUT)");
	midiChannelCombo.addListener(this);


	addAndMakeVisible(midiChannelCombo);


/*
	i = 0;
	comboBoxUpdateInterval.addItem("fastest ", ++i);
	comboBoxUpdateInterval.addItem("very fast ",++i);
	comboBoxUpdateInterval.addItem("fast ", ++i);
	comboBoxUpdateInterval.addItem("medium", ++i);
	comboBoxUpdateInterval.addItem("slow ", ++i);
	comboBoxUpdateInterval.addItem("very slow ", ++i);
	comboBoxUpdateInterval.addItem("slowest", ++i);
	addAndMakeVisible(comboBoxUpdateInterval);

	addAndMakeVisible(updateRateLabel);
	updateRateLabel.setTooltip("Faster settings improve screenshot update rate but will use more CPU overhead");
	comboBoxUpdateInterval.setTooltip("Faster settings improve screenshot update rate but will use more CPU overhead");
	updateRateLabel.setLookAndFeel(l);*/
//	int index = holdingWindow->appProperties.getUserSettings()->getIntValue("MIDIupdateRate", 4);


//	holdingWindow->appCommon->loadAppPropertyInteger
//				(ApplicationCommon::APP_PROPERTY_MIDI_UPDATE_INTERVAL);
	//comboBoxUpdateInterval.addListener(this);

//	if (index <= 0 || index > comboBoxUpdateRate.getNumItems())
//		index = 4;
//	comboBoxUpdateRate.setSelectedId(index, NotificationType::dontSendNotification);

	//comboBoxChanged(&comboBoxUpdateInterval);


	i = 0;
	comboBoxSendRate.addItem("fastest", ++i);
	comboBoxSendRate.addItem("very fast", ++i);
	comboBoxSendRate.addItem("fast", ++i);
	comboBoxSendRate.addItem("moderate", ++i);
	comboBoxSendRate.addItem("slow", ++i);
	comboBoxSendRate.addItem("very slow", ++i);
	comboBoxSendRate.addItem("slowest", ++i);
	addAndMakeVisible(comboBoxSendRate);
	addAndMakeVisible(sendRateLabel);
	addAndMakeVisible(OLEDColourLabel);
	sendRateLabel.setLookAndFeel(l);
	OLEDColourLabel.setLookAndFeel(l);

	colourSelectorLabel.setTooltip("Set colour of background");
	colourSelectorScreenshotLabel.setTooltip("Choose Motas screenshot styling");
	sendRateLabel.setTooltip("A faster setting speeds up data transfer to Motas but uses more CPU overhead and data may not be received by Motas correctly");
	comboBoxSendRate.setTooltip("A faster setting speeds up data transfer to Motas but uses more CPU overhead and data may not be received by Motas correctly."
			"\nIf you are using the MIDI DIN ports (instead of USB) the faster speeds will cause data loss and so will not transfer correctly.");

	colourSelectorScreenshot.setTooltip("Set colour theme of live OLED display");
	//index = holdingWindow->appCommon->loadAppPropertyInteger
	//				(ApplicationCommon::APP_PROPERTY_MIDI_SEND_RATE);


	//index = holdingWindow->appProperties.getUserSettings()->getIntValue("MIDIsendRate", 4);
	//if (index <= 0 || index > comboBoxSendRate.getNumItems())
//		index = 4;
//	comboBoxSendRate.setSelectedId(index, NotificationType::dontSendNotification);
	comboBoxSendRate.addListener(this);
	//comboBoxChanged(&comboBoxSendRate);



	autoConnectMotasDevice.addListener(this);

	for (i = 0; i < (int) NUM_ROUTING_BUTTONS; i++)
	{
		addAndMakeVisible(midiRoutingTextButton[i]);
		midiRoutingTextButton[i].addListener(this);
		midiRoutingTextButton[i].setClickingTogglesState(true);
		String text;

		switch (i)
		{
		case 0:
			text = "Set to allow SysEx data from MotasEdit to Motas output";
			break;
		case 1:
			text = "Set to allow NRPN data from MotasEdit to Motas output";
			break;
		case 2:
			text = "Set to allow other data (notes, controllers, etc) from MotasEdit to Motas output";
			break;
		case 3:
			text = "Set to allow SysEx data from MotasEdit to DAW output";
			break;
		case 4:
			text = "Set to allow NRPN data from MotasEdit to DAW output";
			break;
		case 5:
			text = "Set to allow other data (notes, controllers, etc) data from MotasEdit to DAW output";
			break;
		case 6:
			text = "Set to allow SysEx data from DAW input to  Motas output";
			break;
		case 7:
			text = "Set to allow NRPN data from DAW input to Motas output";
			break;
		case 8:
			text = "Set to allow other data (notes, controllers, etc) from DAW input to Motas output";
			break;
		case 9:
			text = "Set to allow SysEx data from DAW input to DAW output";
			break;
		case 10:
			text = "Set to allow NRPN data from DAW input to DAW output";
			break;
		case 11:
			text = "Set to allow other data (notes, controllers, etc) data from DAW input to DAW output";
			break;
		case 12:
			text = "Set to allow SysEx data from Motas input to DAW output";
			break;
		case 13:
			text = "Set to allow NRPN data from Motas input to DAW output";
			break;
		case 14:
			text = "Set to allow other data (notes, controllers, etc) data from Motas input to DAW output";
			break;

		default:
			text = " ";
			break;
		}
		midiRoutingTextButton[i].setTooltip(text);
	}
/*
	addAndMakeVisible(midiMEOutMotasSysEx);
	addAndMakeVisible(midiMEOutMotasNRPN);
	addAndMakeVisible(midiMEOutMotasOther);

	addAndMakeVisible(midiMEOutDawSysEx);
	addAndMakeVisible(midiMEOutDawNRPN);
	addAndMakeVisible(midiMEOutDawOther);

	addAndMakeVisible(midiDawOutMotasSysEx);
	addAndMakeVisible(midiDawOutMotasNRPN);
	addAndMakeVisible(midiDawOutMotasOther);

	addAndMakeVisible(midiDawOutDawSysEx);
	addAndMakeVisible(midiDawOutDawNRPN);
	addAndMakeVisible(midiDawOutDawOther);


	midiMEOutMotasSysEx.addListener(this);
	midiMEOutMotasNRPN.addListener(this);
	midiMEOutMotasOther.addListener(this);

	midiMEOutDawSysEx.addListener(this);
	midiMEOutDawNRPN.addListener(this);
	midiMEOutDawOther.addListener(this);

	midiDawOutMotasSysEx.addListener(this);
	midiDawOutMotasNRPN.addListener(this);
	midiDawOutMotasOther.addListener(this);

	midiDawOutDawSysEx.addListener(this);
	midiDawOutDawNRPN.addListener(this);
	midiDawOutDawOther.addListener(this);




	midiMEOutMotasSysEx.setClickingTogglesState(true);
	midiMEOutMotasNRPN.setClickingTogglesState(true);
	midiMEOutMotasOther.setClickingTogglesState(true);

	midiMEOutDawSysEx.setClickingTogglesState(true);
	midiMEOutDawNRPN.setClickingTogglesState(true);
	midiMEOutDawOther.setClickingTogglesState(true);

	midiDawOutMotasSysEx.setClickingTogglesState(true);
	midiDawOutMotasNRPN.setClickingTogglesState(true);
	midiDawOutMotasOther.setClickingTogglesState(true);

	midiDawOutDawSysEx.setClickingTogglesState(true);
	midiDawOutDawNRPN.setClickingTogglesState(true);
	midiDawOutDawOther.setClickingTogglesState(true);


*/

	toggleToolTips.addListener(this);


	addAndMakeVisible(toggleToolTips);

	addAndMakeVisible(toggleHideNonMotasDevices);
	toggleHideNonMotasDevices.setTooltip("Remove non-Motas devices from the 'Motas' in and out device lists, remove Motas devices from the DAW in and out lists.");
	toggleHideNonMotasDevices.addListener(this);

	toggleHideNonMotasDevices.setLookAndFeel(l);


	toggleToolTips.setLookAndFeel(l);

	addAndMakeVisible(colourSelector);

	colourSelector.addChangeListener(this);
	colourSelectorScreenshot.addListener(this);
	addAndMakeVisible(colourSelectorScreenshot);



	colourSelectorScreenshotLabel.setFont(14.0f);
	colourSelectorScreenshotLabel.setLookAndFeel(l);
	addAndMakeVisible(colourSelectorScreenshotLabel);

	colourSelectorLabel.setFont(14.0f);
	colourSelectorLabel.setLookAndFeel(l);
	addAndMakeVisible(colourSelectorLabel);
	initPanelSettings(); // DO THIS LAST after combo boxes set up etc




}

MIDIComms::~MIDIComms()
 {
	//setLookAndFeel (nullptr);

	DBG("MIDIComms destructor");


 }
/*
void MIDIComms::updateSettings(bool forwardSysExtoDAW, bool hideNonMotas, bool toolTipsOn, bool autoConnectMotas)
{

	toggleForwardSyExToDAW.setToggleState(forwardSysExtoDAW, NotificationType::dontSendNotification);
	toggleHideNonMotasDevices.setToggleState(hideNonMotas, NotificationType::dontSendNotification);
	toggleToolTips.setToggleState(toolTipsOn, NotificationType::dontSendNotification);
	autoConnectMotasDevice.setToggleState(autoConnectMotas, NotificationType::dontSendNotification);
	updateMIDISettings();

}*/

void MIDIComms::changeListenerCallback(ChangeBroadcaster* source)
{
	if (source == &colourSelector)
	{
		holdingWindow->appCommon->saveAppPropertyString(
				ApplicationCommon::APP_PROPERTY_BG_COLOUR, colourSelector.getCurrentColour().toString());

		this->holdingWindow->setColours(colourSelector.getCurrentColour(), colourSelectorScreenshot.getSelectedItemIndex());
	}


}





//==============================================================================
void MIDIComms::addLabelAndSetStyle (Label& label)
{
    label.setFont (Font (15.00f, Font::plain));
    label.setJustificationType (Justification::centredLeft);
    label.setEditable (false, false, false);
    label.setColour (TextEditor::textColourId, Colours::black);
    label.setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (label);
}


void MIDIComms::resized()
{

	localBoundsRect = getLocalBounds().reduced(10);


	int width = localBoundsRect.getWidth() / 2;
    boundsHeight =  localBoundsRect.getHeight();

    fieldHeight = (int) (boundsHeight *0.06f);
    if (fieldHeight > 40)
    {
    	fieldHeight = 40;
    }
    Rectangle<int> r2 = localBoundsRect.removeFromTop(fieldHeight);
	widthMatrix = ( (width * 2)/3);


	midiChannelLabel.setBounds (r2.removeFromLeft (widthMatrix /2));
	midiChannelCombo.setBounds(r2.removeFromLeft (widthMatrix/2));

	r2 = localBoundsRect.removeFromTop(fieldHeight);

	toggleHideNonMotasDevices.setBounds(r2.removeFromLeft (widthMatrix/2));
	autoConnectMotasDevice.setBounds(r2.removeFromLeft (widthMatrix/2));


	r2 = localBoundsRect.removeFromTop(fieldHeight);


	midiInputMotasLabel.setBounds(r2.removeFromLeft (widthMatrix));
	midiOutputMotasLabel.setBounds(r2.removeFromRight (widthMatrix));



	r2 = localBoundsRect.removeFromTop(boundsHeight / 4);

	midiInputSelectorMotas->setBounds (r2.removeFromLeft (widthMatrix));
	midiOutputSelectorMotas->setBounds (r2.removeFromRight (widthMatrix));


// middle section is left remaining

	buttonSize = jmin(widthMatrix / 16, 3 * fieldHeight/2 );


	auto r3 = r2;
	for (int i = 0; i < 5; i++)
	{
		r2 = r3;
		r2.removeFromTop(buttonSize * i);
		r2.removeFromLeft (buttonSize);
		if (i == 0)
			motasEdit.setBounds(r2.removeFromLeft (buttonSize * 4).removeFromTop(buttonSize *2));
		else if (i == 2)
			dawInput.setBounds(r2.removeFromLeft (buttonSize * 4).removeFromTop(buttonSize * 2));
		else if (i == 3 || i == 1)
			r2.removeFromLeft(buttonSize * 4);
		else
			motasInput.setBounds(r2.removeFromLeft (buttonSize * 4).removeFromTop(buttonSize));


		r2.removeFromLeft (2 * buttonSize);
		midiRoutingTextButton[3*i].setBounds (r2.removeFromLeft (buttonSize).removeFromTop( buttonSize));
		midiRoutingTextButton[3*i+1].setBounds (r2.removeFromLeft (buttonSize).removeFromTop(buttonSize));
		midiRoutingTextButton[3*i+2].setBounds (r2.removeFromLeft (buttonSize).removeFromTop( buttonSize));
		r2.removeFromLeft (2 * buttonSize);

		if (i == 0)
			motasOutput.setBounds(r2.removeFromLeft (buttonSize * 8).removeFromTop(buttonSize));
		else if (i == 1)
			dawOutput.setBounds(r2.removeFromLeft (buttonSize * 8).removeFromTop(buttonSize));
		else if (i == 2)
			motasOutput2.setBounds(r2.removeFromLeft (buttonSize * 8).removeFromTop(buttonSize));
		else if (i == 3)
			dawOutput2.setBounds(r2.removeFromLeft (buttonSize * 8).removeFromTop(buttonSize));
		else
			dawOutput3.setBounds(r2.removeFromLeft (buttonSize * 8).removeFromTop( buttonSize));
	}

	r3.removeFromLeft(5 * buttonSize);
	r3.removeFromTop(5*buttonSize);
	routingMatrixLabel.setBounds(r3);

#if BUILD_STANDALONE_EXECUTABLE == 1


	r2 = localBoundsRect.removeFromTop((int) fieldHeight);
	midiInputLabel.setBounds(r2.removeFromLeft (widthMatrix));
	midiOutputLabel.setBounds(r2.removeFromRight (widthMatrix));

	r2 = localBoundsRect.removeFromTop((int) (boundsHeight / 4.0f));

	midiInputSelector->setBounds (r2.removeFromLeft (widthMatrix));
#endif






#if BUILD_STANDALONE_EXECUTABLE == 1

	midiOutputSelector->setBounds (r2.removeFromRight (widthMatrix));


#endif






	r2 = localBoundsRect.removeFromTop((int)fieldHeight);
	sendRateLabel.setBounds (r2.removeFromLeft (width));

	OLEDColourLabel.setBounds (r2);


	r2 = localBoundsRect.removeFromTop((int)fieldHeight);
	comboBoxSendRate.setBounds (r2.removeFromLeft (width));

#if BUILD_STANDALONE_EXECUTABLE == 0
	r2 = localBoundsRect.removeFromTop((int)fieldHeight);
	guiSizeLabel.setBounds (r2.removeFromLeft (width));
	colourSelectorScreenshotLabel.setBounds(r2);
	r2 = localBoundsRect.removeFromTop((int)fieldHeight);
	guiSizeCombo.setBounds (r2.removeFromLeft (width));


#endif




	colourSelectorScreenshot.setBounds(r2);



	r2 = localBoundsRect.removeFromTop((int)(fieldHeight*8));

	toggleToolTips.setBounds(r2.removeFromLeft (width));
	colourSelectorLabel.setBounds(r2.removeFromTop((int)fieldHeight));
	colourSelector.setBounds(r2);



}



void MIDIComms::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{

	if (comboBoxThatHasChanged == &comboBoxSendRate)
	{
		int index = comboBoxSendRate.getSelectedId();

		holdingWindow->appCommon->saveAppPropertyInteger
							(ApplicationCommon::APP_PROPERTY_MIDI_SEND_RATE, index);
	} else if (comboBoxThatHasChanged == &colourSelectorScreenshot)
	{
		int i = colourSelectorScreenshot.getSelectedItemIndex();
		holdingWindow->appCommon->saveAppPropertyInteger
									(ApplicationCommon::APP_PROPERTY_SCREENSHOT_COLOUR, i);
		this->holdingWindow->setColours(colourSelector.getCurrentColour(), i);
	}
	else if (comboBoxThatHasChanged == &this->midiChannelCombo)
	{
		int channel = this->midiChannelCombo.getSelectedId();
		setMIDIChannel(channel);

	}
/*	else if (comboBoxThatHasChanged == &comboBoxUpdateInterval)
	{

		holdingWindow->appCommon->saveAppPropertyInteger
							(ApplicationCommon::APP_PROPERTY_MIDI_UPDATE_INTERVAL, comboBoxUpdateInterval.getSelectedId());

	}*/
#if BUILD_STANDALONE_EXECUTABLE == 0
	else if (comboBoxThatHasChanged == &guiSizeCombo)
	{
		//holdingWindow->appProperties.getUserSettings()->setValue("guiSize", index);
		holdingWindow->appCommon->saveAppPropertyInteger(ApplicationCommon::APP_PROPERTY_PLUGIN_GUI_SIZE,  guiSizeCombo.getSelectedId());
	}
#endif

}


void MIDIComms::buttonClicked(Button* b)
{

	if (b == &toggleToolTips)
	{
		holdingWindow->appCommon->saveAppPropertyBool
													(ApplicationCommon::APP_PROPERTY_SHOW_TOOLTIPS, b->getToggleState());
		//holdingWindow->appProperties.getUserSettings()->setValue("toolTips", b->getToggleState());
	}
	else if (b == &toggleHideNonMotasDevices)
	{
		holdingWindow->appCommon->saveAppPropertyBool
													(ApplicationCommon::APP_PROPERTY_HIDE_NON_MOTAS_DEVICES, b->getToggleState());
		//holdingWindow->appProperties.getUserSettings()->setValue("hideNonMotas", b->getToggleState());
	} else if (b == & autoConnectMotasDevice)
	{
		holdingWindow->appCommon->saveAppPropertyBool
							(ApplicationCommon::APP_PROPERTY_AUTO_CONNECT_MOTAS, b->getToggleState());
	}
	else
	{
		for (int i = 0; i < NUM_ROUTING_BUTTONS; i++)
		{
			if (b == &midiRoutingTextButton[i])
				setRoutingBitfield(1 << i, b->getToggleState());
		}

	}

	//updateMIDISettings();
}




#if BUILD_STANDALONE_EXECUTABLE == 0
void MIDIComms::updateWindowSize()
{
	int guiSize = guiSizeCombo.getSelectedId();
	switch (guiSize)
	{
		case 1:
			holdingWindow->setSize (640, 480);
			break;
		default:
		case 2:
			holdingWindow->setSize (800, 600);
			break;
		case 3:
			holdingWindow->setSize (1024, 768);
			break;
		case 4:
			holdingWindow->setSize (1200, 900);
			break;
		case 5:
			holdingWindow->setSize (1280, 720);
			break;
		case 6:
			holdingWindow->setSize (1600, 900);
			break;
		case 7:
			holdingWindow->setSize (1920, 1080);
			break;
	}
	//guiSizeCombo.setSelectedId(guiSize, NotificationType::dontSendNotification);
}


#endif

void MIDIComms::initPanelSettings()
{


#if BUILD_STANDALONE_EXECUTABLE == 0
	guiSizeCombo.setSelectedId(holdingWindow->appCommon->
			loadAppPropertyInteger(ApplicationCommon::APP_PROPERTY_PLUGIN_GUI_SIZE),
			NotificationType::dontSendNotification);
#endif

	colourSelector.setCurrentColour(Colour::fromString(holdingWindow->appCommon->
				loadAppPropertyString(ApplicationCommon::APP_PROPERTY_BG_COLOUR)),
			NotificationType::dontSendNotification);

	colourSelectorScreenshot.setSelectedItemIndex(holdingWindow->appCommon->
					loadAppPropertyInteger(ApplicationCommon::APP_PROPERTY_SCREENSHOT_COLOUR),
				NotificationType::dontSendNotification);



	toggleToolTips.setToggleState(holdingWindow->appCommon->
			loadAppPropertyBool(ApplicationCommon::APP_PROPERTY_SHOW_TOOLTIPS),
			NotificationType::dontSendNotification);



	toggleHideNonMotasDevices.setToggleState(holdingWindow->appCommon->
				loadAppPropertyBool(ApplicationCommon::APP_PROPERTY_HIDE_NON_MOTAS_DEVICES),
			NotificationType::dontSendNotification);

	autoConnectMotasDevice.setToggleState(holdingWindow->appCommon->
					loadAppPropertyBool(ApplicationCommon::APP_PROPERTY_AUTO_CONNECT_MOTAS),
			NotificationType::dontSendNotification);


	comboBoxSendRate.setSelectedId(holdingWindow->appCommon->
				loadAppPropertyInteger(ApplicationCommon::APP_PROPERTY_MIDI_SEND_RATE),
			NotificationType::dontSendNotification);



	int ch  = holdingWindow->appCommon->
				loadAppPropertyInteger(ApplicationCommon::APP_PROPERTY_MIDI_BASIC_CHANNEL);

	this->setMIDIChannel(ch);


	MIDIroutingBitfield = (uint16_t) holdingWindow->appCommon->
				loadAppPropertyInteger(ApplicationCommon::APP_PROPERTY_MIDI_ROUTING_BITFIELD);


	for (int i = 0; i < NUM_ROUTING_BUTTONS; i++)
	{
		uint16_t val = MIDIroutingBitfield & (1 << i);

		if (val == 0)
			midiRoutingTextButton[i].setToggleState(false, NotificationType::dontSendNotification);
		else
			midiRoutingTextButton[i].setToggleState(true, NotificationType::dontSendNotification);
	}

}



void MIDIComms::paint(Graphics& g)
{

	g.setColour(LineColour);

#if BUILD_STANDALONE_EXECUTABLE >= 0

	float lineThickness = 2.0f;
	float arrowHeadLength = 10.0f;
	float arrowHeadWidth = 9.0f;


	Line<float> l;
	for (int i = 0; i < 3; i++)
	{
		GlyphArrangement ga;

		if (i == 0)
			ga.addLineOfText(Font(buttonSize * 0.6f), "SysEx", 0, 0);
		else if (i == 1)
			ga.addLineOfText(Font(buttonSize* 0.6f), "NRPN", 0, 0);
		else
			ga.addLineOfText(Font(buttonSize* 0.6f), "Other", 0, 0);
		Path p;
		ga.createPath(p);

		auto pathBounds = p.getBounds();

		p.applyTransform(AffineTransform()
						 .rotated(degreesToRadians(-45.0f),
								  pathBounds.getBottomLeft().x,
								  pathBounds.getBottomLeft().y)
						 .translated(widthMatrix  + buttonSize * (i + 8.0f),  fieldHeight * 3.0f)
						 );
		g.setColour (Colours::black);
		g.fillPath(p);
	}

	for (int i = 0; i < 10; i++)
	{
		float xOff = buttonSize*0.8f;
		int yOff = fieldHeight * 3;
		switch (i)
		{
			default:
			case 0:

				l = Line<float>(widthMatrix  + 5.0f*buttonSize + xOff + 1,  1.5f*buttonSize + yOff,
								widthMatrix  + 5.5f*buttonSize + xOff + 1, 1.5f*buttonSize + yOff);
				g.drawLine(l,lineThickness);


				l = Line<float>(widthMatrix  + 5.5f*buttonSize + xOff ,   buttonSize* 1.5f + yOff,
								widthMatrix  + 6.25f*buttonSize + xOff,  1.0f *buttonSize + yOff);
				g.drawLine(l,lineThickness);

				l = Line<float>(widthMatrix  + 5.5f*buttonSize + xOff ,  buttonSize* 1.5f + yOff,
								widthMatrix  + 6.25f*buttonSize + xOff,  buttonSize*2.0f + yOff);
				g.drawLine(l,lineThickness);
				break;
			case 1:
				yOff += buttonSize ;
				break;
			case 2:
				yOff += buttonSize *2;

				l = Line<float>(widthMatrix  + 5.0f*buttonSize + xOff + 1,  1.5f*buttonSize + yOff,
								widthMatrix  + 5.5f*buttonSize + xOff + 1,  1.5f*buttonSize + yOff);
				g.drawLine(l,lineThickness);


				l = Line<float>(widthMatrix  + 5.5f*buttonSize + xOff ,   buttonSize* 1.5f + yOff,
								widthMatrix  + 6.25f*buttonSize + xOff, 1.0f * buttonSize + yOff);
				g.drawLine(l,lineThickness);

				l = Line<float>(widthMatrix  + 5.5f*buttonSize + xOff ,  buttonSize* 1.5f + yOff,
								widthMatrix  + 6.25f*buttonSize + xOff,  buttonSize*2.0f + yOff);
				g.drawLine(l,lineThickness);
				break;
			case 3:
				yOff += buttonSize * 3;
				break;
			case 4:
				yOff += buttonSize * 4;
				l = Line<float>(widthMatrix  + 5.0f*buttonSize + xOff ,   1.0f *buttonSize + yOff,
										widthMatrix  + 6.25f*buttonSize + xOff,  1.0f *buttonSize + yOff);
				g.drawLine(l,lineThickness);
				break;
			case 5:
				xOff += buttonSize  * 5.0f;
				break;
			case 6:
				xOff += buttonSize  * 5.0f;
				yOff += buttonSize;
				break;
			case 7:
				xOff += buttonSize  * 5.0f;
				yOff += buttonSize*2;
				break;
			case 8:
				xOff += buttonSize  * 5.0f;
				yOff += buttonSize*3;
				break;
			case 9:
				xOff += buttonSize  * 5.0f;
				yOff += buttonSize*4;
				break;

		}

		if (i >= 5)
		{
			l = Line<float>(widthMatrix  + 5.0f*buttonSize + xOff ,  1.0f *buttonSize + yOff,
							widthMatrix  + 6.0f*buttonSize + xOff,  1.0f *buttonSize + yOff);
			g.drawArrow(l, lineThickness, arrowHeadWidth, arrowHeadLength);
		}

	}

#endif

}

void MIDIComms::setRoutingBitfield(uint16_t bit, bool enable)
{

	//DBG("setRoutingBitfield called");
	//DBG(String(bit));
	//DBG(String((int) enable));

	if (enable)
		MIDIroutingBitfield |= bit;
	else
		MIDIroutingBitfield &= ~bit;


	holdingWindow->appCommon->saveAppPropertyInteger
				(ApplicationCommon::APP_PROPERTY_MIDI_ROUTING_BITFIELD, MIDIroutingBitfield);



}

void MIDIComms::setMIDIChannel(int ch)
{
	if (ch >= 1 && ch <= 16)
	{
		midiChannelCombo.setSelectedItemIndex(ch - 1,
					NotificationType::dontSendNotification);

#if BUILD_STANDALONE_EXECUTABLE == 0
		MIDIProcessor* mp = holdingWindow->midiProcessor;
#else
	MIDIProcessor* mp = holdingWindow->midiProcessor.get();
#endif



		if (mp != nullptr)
		{
			mp->setMIDIChannel(ch);
		}


		holdingWindow->appCommon->saveAppPropertyInteger(ApplicationCommon::APP_PROPERTY_MIDI_BASIC_CHANNEL, ch);

	}


}
