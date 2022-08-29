
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
 * MIDIComms.h
 *
 *
 */

#ifndef SOURCE_MIDICOMMS_H_
#define SOURCE_MIDICOMMS_H_
#pragma once

#include "ApplicationCommon.h"
//#include "MIDICommsNoUI.h"
#include "MIDIDeviceListBox.h"
#include "ParameterData.h"
#include "stdint.h"
//#include "MainComponent.h"
#include "utilities.h"
class HoldingWindowComponent;
class PatchPatternSequence;
class Firmware;
class MotasEditPluginAudioProcessor;




class MIDIComms  : public Component,  public ComboBox::Listener,
	public TextButton::Listener, public ChangeListener// , private MidiKeyboardStateListener

{
public:

	MIDIComms(HoldingWindowComponent* parent, LookAndFeel* laf);

	~MIDIComms();

	bool firstTimeSeenMotasSynthMIDI;

	String fileNameLocationDefault;


	ToggleButton toggleToolTips{String("Show tooltips (close and re-open to apply)")};
	ToggleButton toggleHideNonMotasDevices{String("Only 'Motas' devices")};
#if BUILD_STANDALONE_EXECUTABLE == 0
	void updateWindowSize();
#endif

	Colour LineColour;
	HoldingWindowComponent* holdingWindow;



	std::unique_ptr<MIDIDeviceListBox> midiInputSelector;
	std::unique_ptr<MIDIDeviceListBox>  midiOutputSelector;
	MIDIDeviceListBox* midiInputSelectorMotas;
	MIDIDeviceListBox* midiOutputSelectorMotas;
	void initPanelSettings();
private:

	uint16_t MIDIroutingBitfield;

	uint32_t MIDISendRate;

	int fieldHeight;
	Rectangle<int>localBoundsRect;

	int boundsHeight;

	int widthMatrix;
	int buttonSize;

	Colour mainColour{Utilities::backgroundColour};
	void changeListenerCallback (ChangeBroadcaster* source) override;
	ColourSelector colourSelector{ ColourSelector::ColourSelectorOptions::showColourspace | ColourSelector::ColourSelectorOptions::showColourAtTop, 2, 8};
	Label colourSelectorScreenshotLabel{"Motas screenshot styling", "Motas screenshot styling"};
	Label colourSelectorLabel{"Overall colour theme", "Overall colour theme"};
	ComboBox colourSelectorScreenshot;
	void buttonClicked(Button* b) override;

	bool isBusyWithSysEx;
	void comboBoxChanged (ComboBox *comboBoxThatHasChanged) override;


	void setRoutingBitfield(uint16_t bit, bool enable);
	ComboBox comboBoxSendRate;
	Label sendRateLabel { "Send rate Label", "MIDI SysEx send rate to Motas:" };
	Label OLEDColourLabel { " ", " " };

	Label midiInputMotasLabel    { "Midi Input Motas Label",  "MIDI input from Motas:" };
	Label midiOutputMotasLabel   { "Midi Output Motas Label", "MIDI output to Motas:" };

	ToggleButton autoConnectMotasDevice   {"Auto-connect Motas" };


	Label midiChannelLabel { "MIDI channel", "MIDI channel" };
	ComboBox midiChannelCombo;


#if BUILD_STANDALONE_EXECUTABLE == 1
	Label midiInputLabel    { "Midi Input Label",  "MIDI input from controllers and DAW (use loopback/software thru):" };
	Label midiOutputLabel   { "Midi Output Label", "MIDI output to DAW (use loopback/software thru):" };
#else
	Label guiSizeLabel { "GUI size", "Plugin GUI size:" };
#endif

	Label sysExLabel    { "SysEx",  "SysEx" };

	Label motasEdit   { "MotasEdit",  "MotasEdit" };
	Label dawInput    { "from DAW",  "from DAW" };
	Label motasOutput    { "to Motas",  "to Motas" };
	Label dawOutput    { "to DAW",  "to DAW" };
	Label motasOutput2    { "to Motas",  "to Motas" };
	Label dawOutput2    { "to DAW",  "to DAW" };
	Label motasInput    { "from Motas",  "from Motas" };
	Label dawOutput3    { "to DAW",  "to DAW" };


	Label routingMatrixLabel    { "ROUTING MATRIX", "ROUTING MATRIX" };


	#define NUM_ROUTING_BUTTONS 16
	TextButton midiRoutingTextButton[NUM_ROUTING_BUTTONS];

	uint8_t MIDISendIntervalMilliSeconds;
	std::unique_ptr<DrawableButton> motasEditLogoButton;
	int deviceCheckDivider;
	int MIDIupdateInterval;

	void addLabelAndSetStyle (Label& label);
	void resized() override;
#if BUILD_STANDALONE_EXECUTABLE == 0
	ComboBox guiSizeCombo;
#endif
    void paint (Graphics&) override;
    void setMIDIChannel(int ch);
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MIDIComms)
};




#endif /* SOURCE_MIDICOMMS_H_ */
