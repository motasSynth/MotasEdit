
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

    Firmware.h

  ==============================================================================
*/

#ifndef SOURCE_FIRMWARE_H_
#define SOURCE_FIRMWARE_H_

#pragma once

#include "ApplicationCommon.h"
#include "motasDefines.h"
class HoldingWindowComponent;


class Firmware : public Component, private TextButton::Listener
{

public:
	Firmware(HoldingWindowComponent* holdingWindow, LookAndFeel* l);
	~Firmware();

	void updateInfo(uint8_t* buffer);

	void updateFirmwareVersionReported(uint8_t* version);
private:


	uint32_t firmwareVersionReported;
	std::unique_ptr<DrawableButton> motasLogoButton;
	std::unique_ptr<DrawableButton> motasElectronicsLogoButton;


	TextButton getMotasInfoButton;
	TextEditor infoEditLabel;

	HoldingWindowComponent* holdingWindow;

	LookAndFeel* laf;
	String version ;
	String softwareInfoText ;


	void resized() override;

	TextButton updateFirmwareButton;
	TextButton requestBulkButton;
	TextButton sendBulkButton;
	TextButton requestSetupButton;
	TextButton sendSetupButton;

    void buttonClicked (Button*) override;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Firmware)


};

#define NUMBER_OF_OSCILLATORS 3
#define NUMBER_OF_FILTERS 3
#define NUMBER_OF_COMPUTUNE_POINTS  7
#define SYSTEM_INFO_VERSION_EXISTS_IDENTIFIER 0x39FA790B

typedef union
{
	uint8_t buffer[16];
	struct
	{
		uint32_t lastTransferCRC32;
		uint32_t lastSinglePatchCRC32;
		uint32_t firmwareVersion;
	} data;
} tSysexStatus;


typedef union
{

    uint8_t holdingBuffer[256];
    struct
    {
        uint32_t ID1;
        uint32_t ID2;
        uint32_t ID3;

        uint32_t powerCycles;


        uint32_t unused;
        // set initial calibration values all the same (taken from actual measurements on OSC1)
        uint16_t  calibValuesOscillator[NUMBER_OF_OSCILLATORS][NUMBER_OF_COMPUTUNE_POINTS];

        // set values which will give ~ 110Hz, 220Hz, 440Hz...
        uint16_t  calibValuesFilter[NUMBER_OF_FILTERS][NUMBER_OF_COMPUTUNE_POINTS];

        int16_t VCO1PWMCentre ;
        int16_t VCO3PWMCentre ;

        uint16_t calibNoiseLevel ;
        uint16_t calibLpf1Resonance24Level ;
        uint16_t calibLpf1Resonance36Level ;
        uint16_t calibLpf2ResonanceLevel ;

        int32_t potentiometerRangeCalib;
        uint32_t versionValueExists ; //  unique unlikely value
        			// if this value is wrong assume that structure is pre-version and following variable not present
        uint16_t versionNumber ;

        uint32_t firmwareVersion;
    	// __DATE__
    	//	This macro expands to a string constant that describes the date on which the preprocessor is being run.
    	///	The string constant contains eleven characters and looks like "Feb 12 1996". If the day of the month is less than 10, it is padded with a space on the left.
    	char firmwareBuildDate[11];


        uint16_t UIPCBVersion;
        // New May 2018, v5 firmware. Present in versionNumber 0x0002
		char currentPatchName[LENGTH_OF_PATCH_NAME];
		uint32_t lastPatchTransferCRC;

    } data;

}tSystemInfo;



#endif
