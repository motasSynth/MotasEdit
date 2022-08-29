/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "ApplicationCommon.h"
class MIDIProcessor;
class HoldingWindowComponent;
#include "FifoMidi.h"
#include "ApplicationCommon.h"
class MotasPluginParameter;
//==============================================================================
/**
*/
class MotasEditPluginAudioProcessor  : public AudioProcessor, public ChangeBroadcaster
{
public:
    //==============================================================================
    MotasEditPluginAudioProcessor();
    ~MotasEditPluginAudioProcessor();


    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    using AudioProcessor::processBlock;


    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

   // void setupHoldingWindows(HoldingWindowComponent* h);

    MidiBuffer midiBuffer;
    std::unique_ptr<MIDIProcessor> midiProcessor;
    FifoMidi fifoMidiIn{1024};
    FifoMidi fifoMidiInFromDAWParameters{1024};
    FifoMidi fifoMidiParameters{1024};
    FifoMidi fifoMidiUI{1024 * 512UL}; // how large? Large enough to hold patch bank (4096 * 50)

   // MIDIProcessor* getMIDIHandler();
    HashMap<int, MotasPluginParameter*> hashMapParameters{35*50};


    HoldingWindowComponent* getActiveHoldingWindow();

    ApplicationCommon* appCommonPtr;


private:

    void addPluginParamsForPage(String baseName, int page, int options = 0);
    void addPluginParam(String name, int paramPage, int param, int min = 0, int max = 4095);

    void addPluginGlobalParam(String name, int param);
    void addPluginLocalParam(String name, int paramPage, int param);


    String getReadableValue(int paramPage, int param, int value, int maxLength);
    ApplicationCommon appCommon;
    //==============================================================================
    //AudioProcessorValueTreeState parametersTreeState;

	#define SIZE_OF_DAW_MIDI_OUT_BUFFER (1024*8)
    uint8_t  midiParamBuffer[1024];
    uint8_t  midiUIBuffer[SIZE_OF_DAW_MIDI_OUT_BUFFER];

    int MIDIoutSampleCount;

    //The base class (AudioProcessor) takes ownership of the parameter objects,
    //which is why we use raw pointers to store our parameters in the
    //derived processor class. This is safe because you know for certain
    //that the base class will be deconstructed after our derived class.
    //In addition to this, you can also assume that the processor's editor component
    //will be deleted before the processor object.


   // MotasPluginParameter* filt;
   // MotasPluginParameter* osc1Level;
    //AudioParameterInt* mainVol;
   // HoldingWindowComponent* holdingWindow; // n.b. NOT a scoped pointer as will be deleted by someone else....
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MotasEditPluginAudioProcessor)
};
