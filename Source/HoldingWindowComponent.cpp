
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

    HoldingWindowComponent.cpp

  ==============================================================================
*/

#include "./Headers/HoldingWindowComponent.h"
#include "./Headers/MainComponent.h"
#include "./Headers/MIDIComms.h"
#include "./Headers/PatchSetup.h"
#include "Headers/advancedMod.h"
#include "./Headers/images.h"
#include "../binaryResources/MotasEditResources.h"
#include "./Headers/utilities.h"
#include "./Headers/Firmware.h"
#include "Headers/MIDIProcessor.h"
#include "Headers/PatchPatternSequence.h"
#include "Headers/CustomTabbedComponent.h"
#if BUILD_STANDALONE_EXECUTABLE == 0
	#include "./Headers/PluginProcessor.h"
#endif




//==============================================================================
#if BUILD_STANDALONE_EXECUTABLE == 1
HoldingWindowComponent::HoldingWindowComponent()
: appCommon(new ApplicationCommon())
#else
HoldingWindowComponent::HoldingWindowComponent(MotasEditPluginAudioProcessor& p)
:  AudioProcessorEditor (p),  appCommon(p.appCommonPtr), processor (&p)
#endif
{

#if BUILD_STANDALONE_EXECUTABLE == 1
	midiProcessor = std::make_unique<MIDIProcessor>(this);
	appCommon->setMIDIProcessor(midiProcessor.get());
	midiProcessor->updateMIDISettings();
#else
	midiProcessor = processor->midiProcessor.get();
#endif





	image = Drawable::createFromImageData(MotasEditResources::orangeCircle_svg, MotasEditResources::orangeCircle_svgSize);
	imageDown2 = Drawable::createFromImageData(MotasEditResources::greenCircle_svg, MotasEditResources::greenCircle_svgSize);
	imageDown = Drawable::createFromImageData(MotasEditResources::brightgreenCircle_svg, MotasEditResources::brightgreenCircle_svgSize);

	guiCommandPendingLED = std::make_unique<DrawableButton>("button", DrawableButton::ImageFitted);
	guiCommandPendingLED->setImages(image.get(), nullptr, nullptr, image.get(), nullptr, nullptr , nullptr, imageDown2.get());
	addAndMakeVisible(guiCommandPendingLED.get());
	guiCommandPendingLED->setTooltip("Message pending status to Motas");
	guiCommandPendingLED->setEnabled(false);


	midiInMotasLED = std::make_unique<DrawableButton>("button", DrawableButton::ImageFitted);
	midiInMotasLED->setImages(imageDown.get(), nullptr, nullptr, imageDown.get(), nullptr, nullptr , nullptr, imageDown2.get());
	addAndMakeVisible(midiInMotasLED.get());
	midiInMotasLED->setTooltip("MIDI in activity from Motas");
	midiInMotasLED->setEnabled(false);

	midiOutMotasLED = std::make_unique<DrawableButton>("button", DrawableButton::ImageFitted);
	midiOutMotasLED->setImages(imageDown.get(), nullptr, nullptr, imageDown.get(), nullptr, nullptr , nullptr, imageDown2.get());
	addAndMakeVisible(midiOutMotasLED.get());
	midiOutMotasLED->setTooltip("MIDI out activity to Motas");
	midiOutMotasLED->setEnabled(false);

	midiOutSysExMotasLED = std::make_unique<DrawableButton>("button", DrawableButton::ImageFitted);
	midiOutSysExMotasLED->setImages(imageDown.get(), nullptr, nullptr, imageDown.get(), nullptr, nullptr , nullptr, imageDown2.get());
	addAndMakeVisible(midiOutSysExMotasLED.get());
	midiOutSysExMotasLED->setTooltip("MIDI SysEx out activity to Motas");
	midiOutSysExMotasLED->setEnabled(false);


	midiInSysExMotasLED = std::make_unique<DrawableButton>("button", DrawableButton::ImageFitted);
	midiInSysExMotasLED->setImages(imageDown.get(), nullptr, nullptr, imageDown.get(), nullptr, nullptr , nullptr, imageDown2.get());
	addAndMakeVisible(midiInSysExMotasLED.get());
	midiInSysExMotasLED->setTooltip("MIDI SysEx in activity from Motas");
	midiInSysExMotasLED->setEnabled(false);




	for (int i = 0; i < NUMBER_OF_GUI_LED_TIMERS; i++)
		msgTimer[i] = 0;

	messageLabel.setColour (Label::textColourId, Colours::darkgoldenrod);

	addAndMakeVisible(messageLabel);
	addAndMakeVisible(messageLabelUpper);
	addAndMakeVisible(messageLabelRightUpper);

	messageLabelUpper.setColour (Label::textColourId, Colours::darkgoldenrod);
	addAndMakeVisible(messageLabelUpper);
	messageLabelRightUpper.setColour (Label::textColourId, Colours::darkgoldenrod);
	addAndMakeVisible(messageLabelRightUpper);


	messageLabelRight.setColour (Label::textColourId, Colours::darkgoldenrod);
	//messageLabelRight.setColour (TextEditor::backgroundColourId, Colours::black);
	addAndMakeVisible(messageLabelRight);


	//appProperties.getUserSettings()->setValue("autoConnect", 1);


	tabbedComponent = std::make_unique<CustomTabbedComponent>(TabbedButtonBar::Orientation::TabsAtTop);
	midiComms = std::make_unique<MIDIComms>(this, &laf);
	patchSetup = std::make_unique<PatchSetup>(this, &laf);
	advancedModSetup = std::make_unique<AdvancedModSetup>(this, &laf);

	mainComponent = std::make_unique<MainComponent>(this, &laf);

	patchPatternSequence = std::make_unique<PatchPatternSequence>(this, &laf);
	firmwareComponent = std::make_unique<Firmware>(this, &laf);

	//bool toolTipsOn = true;
	if (appCommon->loadAppPropertyBool(ApplicationCommon::APP_PROPERTY_SHOW_TOOLTIPS))
		//appProperties.getUserSettings()->getBoolValue("toolTips", true))
	{
		toolTipWindow = std::make_unique<TooltipWindow>();
		toolTipWindow->setMillisecondsBeforeTipAppears(1200);
		midiComms->toggleToolTips.setToggleState(true, NotificationType::dontSendNotification);
	} else
	{
	//	toolTipsOn = false;
	}

	//bool forwardSysExtoDAW = appCommon->loadAppPropertyInteger(ApplicationCommon::APP_PROPERTY_PASS_SYSEX);//appProperties.getUserSettings()->getBoolValue("forwardSysExtoDAW", false);
	//bool hideNonMotas = appCommon->loadAppPropertyBool(ApplicationCommon::APP_PROPERTY_HIDE_NON_MOTAS_DEVICES);//appProperties.getUserSettings()->getBoolValue("hideNonMotas", true);
	//midiComms->updateSettings(forwardSysExtoDAW, hideNonMotas, toolTipsOn);
	updateGUISettings();
	//String colour = appCommon->loadAppPropertyString(ApplicationCommon::APP_PROPERTY_BG_COLOUR);//appProperties.getUserSettings()->getValue("bgColour", "FF707070");



	DBG("add tabs");

	tabbedComponent->setLookAndFeel(&laf);
	tabbedComponent->addTab("Edit Patch", colour, mainComponent.get(), false);
	tabbedComponent->addTab("ParameterMap", colour, patchSetup.get(), false);
	tabbedComponent->addTab("Advanced Mod.", colour, advancedModSetup.get(), false);
	tabbedComponent->addTab("Settings", colour, midiComms.get(), false);
	tabbedComponent->addTab("Patch/Pattern/Sequence", colour, patchPatternSequence.get(), false);
	tabbedComponent->addTab("Firmware/Backup/Info", colour, firmwareComponent.get(), false);



	//setSize (1024, 576);
	//setSize (1800, 1012);
	//setSize (1200, 675);
#if BUILD_STANDALONE_EXECUTABLE == 1
	updateGUISettings();
	int w =	appCommon->loadAppPropertyInteger(ApplicationCommon::APP_PROPERTY_GUI_SIZE_X);//appProperties.getUserSettings()->getIntValue("windowWidth", 1280);
	int h =	appCommon->loadAppPropertyInteger(ApplicationCommon::APP_PROPERTY_GUI_SIZE_Y);//appProperties.getUserSettings()->getIntValue("windowHeight", 720);


	if (w > 200 && w < 1920 && h > 200 && h < 1080)
		setSize (w, h);
	else
		setSize (1280, 720);

#else
	midiComms->updateWindowSize();
#endif

	messageLabelRight.setFont(10.0f);
	messageLabel.setFont(10.0f);
	messageLabelRightUpper.setFont(10.0f);
		messageLabelUpper.setFont(10.0f);
	addAndMakeVisible(tabbedComponent.get());









}

HoldingWindowComponent::~HoldingWindowComponent()
{
#if BUILD_STANDALONE_EXECUTABLE == 1
	// save settings on exit
	DBG("Saving settings");
	appCommon->saveSettings();//appProperties.getUserSettings()->saveIfNeeded();
#endif
	DBG("HoldingWindowComponent destructor");
}

void HoldingWindowComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));   // clear the background
}

void HoldingWindowComponent::resized()
{
	 auto r = getLocalBounds().reduced(0);
    // This method is where you should set the bounds of any child
    // components that your component contains..

	 int textLabelHeight = 16;

	 auto r2 = r.removeFromBottom(textLabelHeight);
	 const int ledSize = 16;
	 //auto rTop = r2;//r.removeFromTop(ledSize);
	 guiCommandPendingLED->setBounds(r2.removeFromRight(ledSize));

	 midiOutSysExMotasLED->setBounds(r2.removeFromRight(ledSize));
	 midiOutMotasLED->setBounds(r2.removeFromRight(ledSize));
	 midiInSysExMotasLED->setBounds(r2.removeFromRight(ledSize));
	 midiInMotasLED->setBounds(r2.removeFromRight(ledSize));


	 messageLabelUpper.setBounds(r2.removeFromLeft(r.getWidth() / 4));

	 messageLabel.setBounds(r2.removeFromLeft(r.getWidth() / 4));


    //r = getLocalBounds().reduced(0);
    messageLabelRightUpper.setBounds(r2.removeFromLeft(r.getWidth() / 4));
    messageLabelRight.setBounds(r2);




    tabbedComponent->setBounds (r);



#if BUILD_STANDALONE_EXECUTABLE == 1
    int w = getLocalBounds().getWidth();
     int h = getLocalBounds().getHeight();
    if (w > 200 && w < 1920 && h > 200 && h < 1080)
    {
    	appCommon->saveAppPropertyInteger(ApplicationCommon::APP_PROPERTY_GUI_SIZE_X, w);
    	appCommon->saveAppPropertyInteger(ApplicationCommon::APP_PROPERTY_GUI_SIZE_Y, h);
		//appProperties.getUserSettings()->setValue("windowWidth", w);
		//appProperties.getUserSettings()->setValue("windowHeight", h);
    }
#endif
}

int HoldingWindowComponent::getActiveTabIndex()
{
	if (tabbedComponent != nullptr)
		return this->tabbedComponent->getCurrentTabIndex();
	else
		return -1;
}


void HoldingWindowComponent::updatePath(String newPath)
{
	defaultPath = newPath;
	appCommon->saveAppPropertyString(ApplicationCommon::APP_PROPERTY_DEFAULT_FILE_LOCATION, newPath);
	//appProperties.getUserSettings()->setValue("defaultPath", newPath);
}


void HoldingWindowComponent::setColours(const Colour& c, int indexScreenshot)
{
	DBG("Set colours");
	Colour cd = c.contrasting(1.0f);

	this->laf.setColour(Label::textColourId, cd);
	this->laf.setColour(ToggleButton::textColourId, cd);
	this->laf.setColour(ToggleButton::tickColourId, cd);
	this->laf.setColour(ToggleButton::tickDisabledColourId, cd);


	//appProperties.getUserSettings()->setValue("bgColour", c.toString());

	// we seem to need to check for nullptr since this can be called in a callbak... or remove the callback..
	if (tabbedComponent.get() != nullptr)
	{
		for (int i = 0; i < this->tabbedComponent->getNumTabs(); i++)
		{
			tabbedComponent->setTabBackgroundColour(i, c); // this sets the background of the app on each tab

		}
	}
	if (this->mainComponent != nullptr)
	{
		this->mainComponent->setColours(c, indexScreenshot);
	}

	if (this->midiComms != nullptr)
	{
		this->midiComms->LineColour = cd;
	}

	Utilities::setScreenShotColour(indexScreenshot);
}



void HoldingWindowComponent::indicateMessageIsPending(bool isPending)
{
	guiCommandPendingLED->setToggleState(!isPending, NotificationType::dontSendNotification);
}

void HoldingWindowComponent::indicateMidiInFromMotas()
{

	midiInMotasLED->setToggleState(!true, NotificationType::dontSendNotification);
	msgTimer[4] = 2;
}
void HoldingWindowComponent::indicateMidiOutToMotas()
{

	midiOutMotasLED->setToggleState(!true, NotificationType::dontSendNotification);
	msgTimer[5] = 2;
}
void HoldingWindowComponent::indicateMidiOutSysExToMotas()
{
	midiOutSysExMotasLED->setToggleState(!true, NotificationType::dontSendNotification);
	msgTimer[6] = 2;
}
void HoldingWindowComponent::indicateMidiInSysExToMotas()
{
	midiInSysExMotasLED->setToggleState(!true, NotificationType::dontSendNotification);
	msgTimer[7] = 2;
}




void HoldingWindowComponent::newMessage(const String &msg, int type)
{

	switch (type)
	{
		case MESSAGE_TYPE_TO_MOTAS:
			newMessageRightUpper(msg);
			break;
		case MESSAGE_TYPE_FROM_MOTAS:
			newMessageUpper(msg);
			break;
		case MESSAGE_TYPE_TO_DAW:
			newMessageRight(msg);
			break;
		case MESSAGE_TYPE_FROM_DAW:
			newMessage(msg);
			break;
	}
}

void HoldingWindowComponent::newMessageUpper(const String &msg)
{
	msgTimer[0] = 20;
	messageLabelUpper.setText("MOTAS INPUT: " +msg, NotificationType::dontSendNotification);
}



void HoldingWindowComponent::newMessage(const String &msg)
{
	msgTimer[1] = 20;
#if BUILD_STANDALONE_EXECUTABLE == 0
	messageLabel.setText("DAW INPUT: " + msg, NotificationType::dontSendNotification);
#else
	messageLabel.setText("EXT/DAW INPUT: " + msg, NotificationType::dontSendNotification);
#endif
}


void HoldingWindowComponent::newMessageRight(const String &msg)
{
	msgTimer[2] = 20;
	messageLabelRight.setText("DAW OUTPUT: " +msg, NotificationType::dontSendNotification);
}

void HoldingWindowComponent::newMessageRightUpper(const String &msg)
{
	msgTimer[3] = 20;
	messageLabelRightUpper.setText("MOTAS OUTPUT: " +msg, NotificationType::dontSendNotification);
}

String HoldingWindowComponent::getDefaultPath()
{
	return defaultPath;
}

void HoldingWindowComponent::setActiveTab(int newIndex)
{
	this->tabbedComponent->setCurrentTabIndex(newIndex, true);
}

void HoldingWindowComponent::updateGUISettings()
{
	defaultPath = appCommon->loadAppPropertyString(ApplicationCommon::APP_PROPERTY_DEFAULT_FILE_LOCATION);//appProperties.getUserSettings()->getValue("defaultPath", "");

	if (this->midiComms != nullptr)
	{
		midiComms->initPanelSettings();
	}

	if (this->mainComponent != nullptr)
	{
		mainComponent->initPanelSettings();
	}


/*
	if (this->patchSetup != nullptr)
	{
		patchSetup->initPanelSettings();
	}*/


	colour = Colour::fromString(appCommon->loadAppPropertyString(ApplicationCommon::APP_PROPERTY_BG_COLOUR));//appProperties.getUserSettings()->getValue("bgColour", "FF707070");
	int sc = appCommon->loadAppPropertyInteger(ApplicationCommon::APP_PROPERTY_SCREENSHOT_COLOUR);//appProperties.getUserSettings()->getValue("bgColour", "FF707070");
	setColours(colour, sc);
}


void HoldingWindowComponent::updateLEDs()
{
	if (msgTimer[0] == 0)
		messageLabelUpper.setText("", NotificationType::dontSendNotification);
	else
		msgTimer[0]--;

	if (msgTimer[1] == 0)
		messageLabel.setText("", NotificationType::dontSendNotification);
	else
		msgTimer[1]--;

	if (msgTimer[2] == 0)
		messageLabelRight.setText("", NotificationType::dontSendNotification);
	else
		msgTimer[2]--;

	if (msgTimer[3] == 0)
		messageLabelRightUpper.setText("", NotificationType::dontSendNotification);
	else
		msgTimer[3]--;

	if (msgTimer[4] == 0)
		midiInMotasLED->setToggleState(!false, NotificationType::dontSendNotification);
	else
		msgTimer[4]--;


	if (msgTimer[5] == 0)
		midiOutMotasLED->setToggleState(!false, NotificationType::dontSendNotification);
	else
		msgTimer[5]--;


	if (msgTimer[6] == 0)
		midiOutSysExMotasLED->setToggleState(!false, NotificationType::dontSendNotification);
	else
		msgTimer[6]--;

	if (msgTimer[7] == 0)
		midiInSysExMotasLED->setToggleState(!false, NotificationType::dontSendNotification);
	else
		msgTimer[7]--;


}
