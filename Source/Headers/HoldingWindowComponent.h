
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

    HoldingWindowComponent.h


  ==============================================================================
*/

#pragma once
#include "ApplicationCommon.h"
#include "../JuceLibraryCode/JuceHeader.h"
class MIDIProcessor;
class MIDIComms;
class PatchPatternSequence;
class MainComponent;
class CustomTabbedComponent;
class PatchSetup;
class AdvancedModSetup;

#define TAB_INDEX_EDIT 0
#define TAB_INDEX_PATCH_PATTERN 4

#include "CustomLook.h"
class Firmware;

#if BUILD_STANDALONE_EXECUTABLE == 0
class MotasEditPluginAudioProcessor;
#endif
#define NUMBER_OF_GUI_LED_TIMERS 8
class ApplicationCommon;
//==============================================================================
/*
*/
#if BUILD_STANDALONE_EXECUTABLE == 1

	class HoldingWindowComponent    : public Component, public ComponentListener
	{
	public:
		HoldingWindowComponent();

		std::unique_ptr<ApplicationCommon> appCommon;
#else

	class HoldingWindowComponent    : public AudioProcessorEditor, public ComponentListener
	{
	public:
		HoldingWindowComponent(MotasEditPluginAudioProcessor& p);
		ApplicationCommon* appCommon;
#endif

    ~HoldingWindowComponent();


    void updateGUISettings();


    void setActiveTab(int newIndex);
    int getActiveTabIndex();
  //  ApplicationProperties appProperties;
    void setColours(const Colour &c, int indexScreenshot);

    void updatePath(String newPath);
    String getDefaultPath();


    enum
    {
    	MESSAGE_TYPE_FROM_MOTAS,
    	MESSAGE_TYPE_TO_MOTAS,
    	MESSAGE_TYPE_FROM_DAW,
    	MESSAGE_TYPE_TO_DAW
    };

    void newMessage(const String &msg, int type);


    void indicateMessageIsPending(bool isPending);

    void indicateMidiInFromMotas();
    void indicateMidiOutToMotas();
    void indicateMidiOutSysExToMotas();
    void indicateMidiInSysExToMotas();


private:

   std::unique_ptr<Drawable> image;

   std::unique_ptr<Drawable> imageDown;
   std::unique_ptr<Drawable> imageDown2;
   std::unique_ptr<DrawableButton> guiCommandPendingLED;
   std::unique_ptr<DrawableButton> midiInMotasLED;
   std::unique_ptr<DrawableButton> midiOutMotasLED;
   std::unique_ptr<DrawableButton> midiOutSysExMotasLED;
   std::unique_ptr<DrawableButton>    midiInSysExMotasLED;



   void newMessage(const String &msg);
   void newMessageRight(const String &msg);
   void newMessageUpper(const String &msg);
   void newMessageRightUpper(const String &msg);

    /*
    *
    * Create CustomLook object FIRST
    * THIS MUST BE ABOVE ALL THE COMPONENTS THAT USE IT SO IS DESTROYED LAST AFTER ALL USERS ALREADY DESTROYED!
     *
     */

    CustomLook laf{0};
public:
    std::unique_ptr<MIDIComms> midiComms;
#if BUILD_STANDALONE_EXECUTABLE == 0
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MotasEditPluginAudioProcessor* processor;
    MIDIProcessor* midiProcessor;
#else
    std::unique_ptr< MIDIProcessor> midiProcessor;
#endif
    //

    std::unique_ptr<CustomTabbedComponent> tabbedComponent;
    std::unique_ptr<MainComponent> mainComponent;
    std::unique_ptr<PatchSetup> patchSetup;
    std::unique_ptr<AdvancedModSetup> advancedModSetup;

    std::unique_ptr<PatchPatternSequence> patchPatternSequence;
    std::unique_ptr<Firmware> firmwareComponent;

	void updateLEDs();
private:
    Colour colour;
    void paint (Graphics&) override;
    void resized() override;


	int msgTimer[NUMBER_OF_GUI_LED_TIMERS];
    String defaultPath;
    Label messageLabel;
    Label messageLabelRight;
    Label messageLabelUpper;
    Label messageLabelRightUpper;

  //  PropertiesFile::Options options;
    std::unique_ptr<TooltipWindow> toolTipWindow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HoldingWindowComponent)
};
