
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

    PatchPatternSequence.h


  ==============================================================================
*/

#pragma once

#include "ApplicationCommon.h"

#include "CustomListBox.h"
//#include "MIDIComms.h"
#include "MIDIProcessor.h"

class CustomListBox;
class HoldingWindowComponent;

struct ListEntry : ReferenceCountedObject
{

	ListEntry (const String& deviceName) : name (deviceName) {}

    String name;
    //std::unique_ptr<MidiInput> inDevice;
    //std::unique_ptr<MidiOutput> outDevice;

    typedef ReferenceCountedObjectPtr<ListEntry> Ptr;
};
typedef enum
{
	CUSTOM_LFO_EDIT_VALUE,
	CUSTOM_LFO_EDIT_TIME,
	CUSTOM_LFO_EDIT_START_TIME
}CUSTOM_LFO_EDITS;
typedef enum
{
	CUSTOM_LFO_MODE_STEPPED,

	CUSTOM_LFO_MODE_GLIDE
} CUSTOM_LFOS_MODES;
#define CUSTOM_LFO_VERSION_NUMBER 0x01


#define MOD_SOURCE_MODULATION_WHEEL 2
//==============================================================================
/*
*/
class PatchPatternSequence    : public Component, public Button::Listener, public TextEditor::Listener,
public DragAndDropContainer, private ComboBox::Listener
{
public:
	void populateCurrentPatch(uint8_t* buffer);
	void populateAllPatches(uint8_t* buffer);

	void populateCurrentPattern(uint8_t* buffer);
	void populateAllPatterns(uint8_t* buffer);


	void populateCurrentSequence(uint8_t* buffer);
	void populateAllSequences(uint8_t* buffer);

    PatchPatternSequence(HoldingWindowComponent* hw, LookAndFeel* l);
    ~PatchPatternSequence();

    void paint (Graphics&) override;
    void resized() override;
    ReferenceCountedArray<ListEntry> listBoxItems;
    ReferenceCountedArray<ListEntry> listBoxPresetsItems;

    void mouseDrag(const MouseEvent &event) override;

    void updatePatchUI(int row, bool dataAdjusted, bool final, int type);
    void updatePresetPatch(int row);
    void changeListBoxRow(int newRow);
   // MIDIComms* midiComms;
   // MIDICommsNoUI* midiCommsNoUI;
   // MainComponent* mainComponent;
    HoldingWindowComponent* holdingWindow;
    void sendData(uint8_t bank, uint8_t preset);


    void saveData(int type, uint8_t* buffer);

    void copyPatchToPresets(int source, int dest);
    void copyPatchToBank(int source, int dest);
    void updatePresetsDisplay();
    static void convertPatchToLatestVersion(tAllParams* source);
    static int32_t convertEGTimeToValue_CONVERSION(int32_t value);
    static void resetCustomLFOsInPatch(tAllParams* p);
    static void clearAdvancedMod(t_modOfMod* m);
    static void clearAdvancedModPatch(tAllParams* p);


private:

    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;
    ComboBox factoryCombo;

    void getFactoryPatches(int bankNum);
    uint8_t* bankOfPatches[BANK_OF_PATCHES_LENGTH];
    Array<uint8_t*> bankOfPatterns;
    Array<uint8_t*> bankOfSequences;

    //using DragAndDropContainer::dragOperationStarted;

   // void dragOperationStarted(const DragAndDropTarget::SourceDetails &) override;

    uint8_t rawTransferBuffer[LARGEST_BUFFER_LENGTH];
    void refreshListBox();
    int currentListBoxRow;



    std::unique_ptr<TextEditor> nameEdit;

    int currentModePatchPatternBank;
    int patchSelection;
    int patternSelection;
    int sequenceSelection;

    bool isModeBank;

    void buttonClicked (Button*) override;

    void updateCurrentDisplayData();

    void loadSave(bool isSaving, String itemName);

    void textEditorTextChanged(TextEditor & t)	 override;
    //std::unique_ptr<LookAndFeel> laf;

    ComboBox comboBoxDestination;

    std::unique_ptr<TextButton> patchesButton;
    std::unique_ptr<TextButton> patternsButton;
    std::unique_ptr<TextButton> sequencesButton;

    TextButton singleButton;
	TextButton bankButton;
	TextButton saveButton;
	TextButton loadButton;
	TextButton clearSlotsButton;

	TextButton copyAllPatchesToPresets;
	TextButton copyAllPatchesFromPresets;


    std::unique_ptr<TextButton> requestButton;
    std::unique_ptr<TextButton> sendButton;


    Label listLabel;
    Label patchPresetsLabel;
    std::unique_ptr<TabbedComponent> tabbedComponent;


    uint8_t* patchBuffer;
    uint8_t* patternBuffer;
    uint8_t* sequenceBuffer;
    std::unique_ptr<CustomListBox > listBox;
    std::unique_ptr<CustomListBox > listBoxPresets;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PatchPatternSequence)
};
