/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "ApplicationCommon.h"
//#include "ModulationButton.h"
//#include "PanelControl.h"
//#include "PanelGraphic.h"
//#include "SubWindow.h"
//#include "MIDIComms.h"
//#include "patchDefinition.h"
//#include "utilities.h"
#include "CustomLook.h"
#include "utilities.h"
//#include "CustomComboBox.h"
class PanelControl;
class PanelGraphic;
class ParameterData;
class HoldingWindowComponent;
class PatchSetup;
class AdvancedModSetup;
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

#define NUMBER_OF_CONTROLS 35


class MainComponent   : public Component, private TextButton::Listener, public ComboBox::Listener,
private TextEditor::Listener, private MidiKeyboardStateListener, Slider::Listener

{

public:

    //==============================================================================
    MainComponent(HoldingWindowComponent* holdingWindow, LookAndFeel* laf);
    ~MainComponent();

    HoldingWindowComponent* holdingWindow;
    //==============================================================================


    void parameterChangeGUI(ParameterData& p);
    //void editActivePatch(const ParameterData& p);
    void setParameterPage(ParameterData &p, int panelControlId);
    void updatePanel();
    void updateMotasScreenshot(uint8_t * imageBuffer, int size);
    void sendProgramChange(uint8_t newProgram);
    void setColours(Colour c, int sc);
    int activeParameterPage;

    void updateParamText();
    void initPanelSettings();
    void updatePresetComboBox(bool firstTime);
    void changePatchFromDAW();

    void restoreParameterValue(ParameterData* p, bool secondarySlider, bool tertiarySlider);
    void updatePatchComboText();
    void setComboBoxIndex(int index);

    tAllParams* getPatch();


    t_CCControlStruct * getCCControl(uint8_t mode);
    void setCCPage(uint8_t mode, uint8_t controller, uint8_t page);
    void setCCParam(uint8_t mode, uint8_t controller, uint8_t param);

    uint8_t getCCPage(uint8_t mode, uint8_t controller);

    uint8_t getCCParam(uint8_t mode, uint8_t controller);



    void indicatePatchDataChanged();


    ComboBox patchesCombo;




private:
    bool keyPressed (const KeyPress& key) override;



    void changePatchName(String text);

    void leavePatchNameEditMode();
    char patchName[NAME_LENGTH + 1];

    std::unique_ptr<TextEditor> nameEdit;

    void sliderValueChanged (Slider* slider) override;


    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;
   // bool 	keyPressed (const KeyPress &key, Component *originatingComponent) override;



    void changePatch();

    void paint (Graphics&) override;
    void resized() override;

    void adjustFontToFit();
    CustomLook knobLookAndFeel{1};

    void handleNoteOn (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff (MidiKeyboardState*, int midiChannel, int midiNoteNumber, float velocity) override;
    MidiKeyboardState keyboardState;
    MidiKeyboardComponent midiKeyboard{keyboardState, MidiKeyboardComponent::horizontalKeyboard};


    LookAndFeel* laf;

    void mouseWheelMove	(	const MouseEvent & 	event,    const MouseWheelDetails & 	wheel     )	override;
    Colour mainColour{Utilities::backgroundColour};
    int screenShotColour;

    ParameterData* activeParameter;

    int activePanelControlId;
    void 	textEditorTextChanged (TextEditor & t) override;
   // void 	textEditorReturnKeyPressed (TextEditor & t) override;
    void textEditorFocusLost(TextEditor& t) override;
    Label currentValueName;
    Label currentValueEditRaw;

    Label currentValueEdit;

    uint8_t EGRestart[5];

    void buttonClicked(Button* b) override;
    TextButton sendAllPatchSettingsButton;
    TextButton requestAllPatchSettingsButton;


    const int screenshotScalingFactor  = 4;
    Image screenshotImage;
    std::unique_ptr<Graphics> graphicsScreenshot;


    std::unique_ptr<DrawableButton> motasLogoButton;
    std::unique_ptr<ImageComponent> motasScreenshot;
   // Graphics motasScreenshotGraphics;

    int previousActiveParameterPage;
    uint8_t activeEG;
    uint8_t activeLFO;

    void initControls();

   // SubWindow* midiWindow;

   // TextButton buttonMIDIsettings  { "MIDI settings" };
   // ComboBox comboBoxLookAndFeel;
   // OwnedArray<LookAndFeel> lookAndFeels;
    TextButton toggleButtonTestMIDI{String("TEST MIDI")};
    TextButton toggleButtonLiveScreenShot{String("SC")};
    TextButton toggleButtonChangePage{String("APC")};
    TextButton toggleButtonMonitorOnly{String("MON")};

    void addLookAndFeel (LookAndFeel* laf, const String& name);
    void drawBackground(Graphics& g, Colour c);
    void populateLFOCombo(String value);
    void setAllLookAndFeels ();
   // void buttonMIDIclicked ();
    //==============================================================================
    // Your private member variables go here...
    TextButton changeThemeButton  { "Change theme" };


    // owned array handles deletion of objects, and always stores pointers so don't need th '*' symbol
    OwnedArray<PanelControl> controlsArray;
    OwnedArray<PanelGraphic> panelGraphicsArray;

    void drawLine(Graphics& g, float x1, float y1, float x2, float y2);

    TextButton increaseButton;
    TextButton decreaseButton;
    TextButton increaseButtonBiggerSteps;
    TextButton decreaseButtonBiggerSteps;
    TextButton renamePatchButton;

    TextButton savePatchChangeButton;
    TextButton restorePatchChangeButton;

    TextButton resetPage;
    TextButton zeroParameter;


    bool editingComboPatchName;


    Slider modulation1{Slider::LinearBarVertical, Slider::TextBoxBelow};
    Slider modulation2{Slider::LinearBarVertical, Slider::TextBoxBelow};
    Slider modulation3{Slider::LinearBarVertical, Slider::TextBoxBelow};
    Slider modulation4{Slider::LinearBarVertical, Slider::TextBoxBelow};
  ///  Slider modulation2{Slider::LinearVertical, Slider::NoTextBox};
  //  Slider modulation3{Slider::LinearVertical, Slider::NoTextBox};
  //  Slider modulation4{Slider::LinearVertical, Slider::NoTextBox};
    void resetParameterInPatch(ParameterData& p);

    String oldName;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
