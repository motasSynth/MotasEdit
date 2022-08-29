
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


#ifndef UTILITIES_H
#define UTILITIES_H
#include "ApplicationCommon.h"
#include "motasDefines.h"

#define PARAMETER_UPDATE_RATE_IN_HZ ((uint32_t) 1680U)




enum
{
	PAGE_DEFAULT,
	PAGE_OSC_FREQ,
	PAGE_MASTER_FREQ,
	PAGE_LPF1_FREQ,
	PAGE_LPF2_FREQ,
	PAGE_HPF_FREQ,

};

enum
{

	DISPLAY_MODE_NUMERIC,
	DISPLAY_MODE_COMBO,
	DISPLAY_MODE_DEFAULT,
	DISPLAY_MODE_PARAMETER_OFFSET,
	DISPLAY_MODE_PERCENT,
	DISPLAY_MODE_EG_TIME,
	DISPLAY_MODE_DELAY_TIME,
	DISPLAY_MODE_LFO_RATE,
	DISPLAY_MODE_SHAPE

} ;



typedef struct
{

	//uint8_t active;
	//uint8_t state;

	uint8_t parameterMSB;
	uint8_t parameterLSB;
	uint8_t valueMSB;
	uint8_t valueLSB;
} NRPNmessage_t;

#define LFO_MAX_AMPLITUDE (4096*16*8L)
#define MAX_LEVEL_VALUE ((int32_t)4095)
#define VCF_STEPS_PER_OCTAVE 333
#define VCF1_VALUE_FOR_110Hz 700
#define VCF2_VALUE_FOR_110Hz 500
#define HPF_VALUE_FOR_110Hz 500
#define MAX_LEVEL_VALUE_FREQ ((int32_t) 36000)

class ParameterData;

class Utilities
{



public:
	static Colour textYellow;
	static Colour buttonYellow;
	static Colour screenWhite;
	static Colour screenYellow;
	static Colour backgroundColour;
	static Colour getPixelColour(uint8_t data);

	static void setScreenShotColour(int choice);

	static Colour screenshotColour;
	static Colour screenshotColourBG;
	//static bool invertScreenColour;
	Utilities();

    void convertBitmap(String fileNameBitmapDefault);
    int16_t convertMIDIbytes(uint8_t currentByte, uint32_t i);
    static uint32_t lowLevelCreateMIDIStream(uint8_t* buf, uint32_t size,  uint8_t type, uint8_t startStopContinue,
            uint8_t msg0, uint8_t msg1, uint8_t msg2, uint8_t* outBuf, uint8_t model, uint8_t channel);

    static uint32_t processBufferForMIDISend(char* outBuffer, char* buffer, uint32_t originalDataLength);

    static void write32(uint32_t val, int offset, uint8_t* buffer);

    static uint32_t read32(int offset, uint8_t* buffer);

    static Array<uint8_t> prepareSysExPacket(int j, uint8_t* buffer, uint32_t packetSize, uint32_t fileLength, int32_t lastPacket,
    		uint8_t type, uint8_t msg0, uint8_t msg1, uint8_t msg2, uint8_t modelNum, uint8_t channel);


   // void setMotasModel(int modelNum);
    uint16_t  runLengthDecode(uint8_t* in, uint16_t length, uint8_t* out, uint16_t maxIndex);
    static void sendMIDISysExCommand(String name, char msg1, char msg2);

    static uint32_t prepareSysExPacketsFromUnpacked(uint8_t* outBuf, uint8_t* buffer, uint32_t totalDataSize, uint8_t type,
                                                           uint8_t msg0, uint8_t msg1, uint8_t msg2, uint8_t model, uint8_t channel );


    uint32_t convertRawMIDI(uint8_t* buf, uint32_t totalSize);

    static int32_t convertEGTimeToValue(int32_t value);
    static uint32_t convertEGValueToTime(int32_t value);
    static int32_t checkValuePitch(NRPNmessage_t* msg);
    static int32_t checkNRPNValue(int low, int high, NRPNmessage_t* msg);

   // void updatePatchCRC(tAllParams* patch);

  //  static String getParameterDisplay(ParameterData* p);
    //static String getParameterDisplay(int page, int parameter, int value);

    static String getName(uint8_t* buf);
    static void setName(String newName, uint8_t* buf);
    static void setName(const uint8_t* newName, uint8_t* buf);
    static void populateNameBuffer(String& name, char* nameBuf);
    static int32_t convertLFOrate(int32_t rate);
    static String getTimeString(uint32_t val);


private:


    uint8_t getNibble(uint8_t* in, uint16_t index);



    uint16_t maxRLEIndex;

    uint8_t msbByte ;
    uint8_t msbCount ;



};


namespace MIDI_stuff
{
    const uint8_t BYTE_SYSEX_START   = 0xF0;
    const uint8_t BYTE_SYSEX_END     = 0xF7;


    const uint8_t BYTE_SYSEX_ID1     = 0x00; // Motas Electronics Limited assigned MIDI SYSEX IDs
    const uint8_t BYTE_SYSEX_ID2     = 0x21; // Motas Electronics Limited assigned MIDI SYSEX IDs
    const uint8_t BYTE_SYSEX_ID3     = 0x2F; // Motas Electronics Limited assigned MIDI SYSEX IDs

    const uint8_t BYTE_SYSEX_PRODUCT_ID_1  = 0x06; // product identifier (motas 5, 6,...)


    const uint8_t BYTE_CONTROL_CHANGE = 0xB0;

    // NRPN method
    const uint8_t BYTE_CONTROL_SET_PAGE = 0x63; // undefined controller
    const uint8_t BYTE_CONTROL_SET_PARAMETER = 0x62; // undefined controller
    const uint8_t BYTE_CONTROL_SET_VALUE_MSB = 0x06; // data
    const uint8_t BYTE_CONTROL_SET_VALUE_LSB = BYTE_CONTROL_SET_VALUE_MSB + 32;

}



#endif // UTILITIES_H
