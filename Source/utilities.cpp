

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






#include "./Headers/utilities.h"

#include <stdint.h>

//#include "./Headers/MIDIComms.h"
#include "./Headers/crc.h"
#include  "Headers/ParameterData.h"
#include "Headers/PanelControl.h"


//#include "Headers/MIDIProcessor.h"





typedef union {
    struct {
        uint32_t ALL;
    } all;
    struct {
        uint8_t cin_cable;
        uint8_t evnt0;
        uint8_t evnt1;
        uint8_t evnt2;
    }evnt_0;
    struct {
        uint8_t type:4;
        uint8_t cable:4;
        uint8_t chn:4;
        uint8_t event:4;
        uint8_t value1;
        uint8_t value2;
    }evnt_1;

} midi_package_t;





Colour Utilities::screenshotColour = Colours::yellow;
Colour Utilities::screenshotColourBG = Colours::black;
//bool Utilities::invertScreenColour = false;


Utilities::Utilities()
{
	  msbByte = 0;
	  msbCount = 0;

	  maxRLEIndex = 0;
}



Colour Utilities::textYellow = Colour(0xFF, 0xF1, 0x2D);

Colour Utilities::buttonYellow = Colour(0xFF, 0xF1, 0x2D);
Colour Utilities::screenWhite = Colour(0xFF, 0xFF, 0xFF);
Colour Utilities::screenYellow = Colour(0xFF, 0xFF, 0x00);
//Colour c(0xB0, 0xB0, 0xB0);
Colour Utilities::backgroundColour= Colour(0xA0, 0xA0, 0xA0);

int32_t Utilities::checkValuePitch(NRPNmessage_t* msg)
{

	// must need to piece together the MSB and LSB data

	int32_t result = msg->valueLSB + (uint16_t) msg->valueMSB * 128;

	// no need to offset the result here
	result *= 3;

	if (result < 0)
		result =  0;
	else if (result > 36000)
		result = 36000;
	return result;
}

int32_t Utilities::checkNRPNValue(int low, int high, NRPNmessage_t* msg)
{
	// must need to piece together the MSB and LSB data
	int32_t result = msg->valueLSB + (uint16_t) msg->valueMSB * 128;
	// now offset the result
	result -= 8192;
	if (result < low)
	{
		result =  low;
	}
	else if (result > high)
	{
		result = high;
	}
	return result;
}

const float EG_MULT_LOG = 394.76f;


int32_t Utilities::convertEGTimeToValue(int32_t value)
{
/*
	if (value < 1) //  protect against corrupted data
		value = 1;

	int32_t val =  4095 - (int32_t) (393.9f * logf((float) EG_RATE_MAX_VALUE / value ) );
	if (val < 0)
		val = 0;
	return val;
	*/
	if (value < 1) //  protect against corrupted data
		value = 1;

	int32_t val = (int32_t) (EG_MULT_LOG * logf((float) EG_RATE_MAX_VALUE / value ) );
	if (val < 0)
		val = 0;
	else if (val > 4095)
		val = 4095;
	return val;
}

uint32_t Utilities::convertEGValueToTime(int32_t value)
{/*
	if (value > 4095)
		return EG_RATE_MAX_VALUE;
	else
		return (uint32_t) (EG_RATE_MAX_VALUE * expf((value - 4095) / 393.9f)) ;
		*/
	if (value <= 0)
		return EG_RATE_MAX_VALUE;
	else
		return (uint32_t) (EG_RATE_MAX_VALUE * expf((-value) / EG_MULT_LOG)) ;
}



String Utilities::getName(uint8_t* buf)
{
	uint8_t nameBuf[NAME_LENGTH + 1];
	for (int i = 0; i < NAME_LENGTH; i++)
	{
		nameBuf[i] = buf[i + 8];
		if (nameBuf[i] < 32 || nameBuf[i] >= 0x7F)
			nameBuf[i] = 32;
	}
	nameBuf[NAME_LENGTH] = 0;
	return String((char*) nameBuf);
}

void Utilities::setName(String newName, uint8_t* buf)
{
	uint8_t nameBuf[NAME_LENGTH + 1];
	populateNameBuffer(newName, (char*) nameBuf);
	setName(nameBuf, buf);
}

void Utilities::setName(const uint8_t* newNameBuffer, uint8_t* buf)
{
	// clear the name
	memset(&buf[8], 32, NAME_LENGTH);
	// write new name in
	for (int i = 0; i < NAME_LENGTH ; i++)
	{
		uint8_t val = newNameBuffer[i];
		if (val < 32 || val >= 0x7F)
			continue;
		buf[i + 8] = val;
	}
}

void Utilities::populateNameBuffer(String& name, char* nameBuf)
{
	name = name.substring(0, NAME_LENGTH);
	memset(nameBuf, 32, NAME_LENGTH); // set to space characters
	name.copyToUTF8(nameBuf, NAME_LENGTH + 1);
}

String Utilities::getTimeString(uint32_t val)
{
	String disp = "?";
	if (val >= 1000000)
	{
		val /= 10000;
		disp = String(((float) val) / 10.0f, 1) + " seconds";
	}
	else if (val >= 100000)
	{
		val /= 1000;
		disp = String(((float) val) / 100.0f, 2) + " seconds";

	} else if (val >= 10000)
	{
		val /= 100;
		disp = String(val) + " ms";
	}
	else if (val >= 1000)
	{
		val /= 10;
		disp = String(((float) val) / 10.0f, 1) + " ms";
	}
	else
	{
		disp = String(((float) val) / 100.0f, 2) + " ms";
	}
	return disp;
}




int32_t Utilities::convertLFOrate(int32_t rate)
{
	// convert values from 0.. 4095 to values that get more large at the top end so LFO freq. range is non-linear
	//return rate + (rate * rate) / 274;

	// aim for 256 Hz at the top-end

	// prevent calculation overflow
	if (rate > 8379)
		rate = 8379;
	else if (rate < -8379)
		rate = -8379;
	return rate + (rate * ((rate * rate) / 274)) / 900;
}




/*
String Utilities::getParameterDisplay(int page, int parameter, int value)
{
	ParameterData p(0);
	p.setPage(page);
	p.setParameter(parameter);
	p.setValue(value);
	return getParameterDisplay(&p);
}
*/
/*
String Utilities::getParameterDisplay(ParameterData* p)
{
	String disp = "?";
	int32_t value = p->getPanelValue();
	int page = -1;
	int displayMode = -1;
	bool showPolarityPlus = false;
	if (p->getPage() > 0)
	{
		switch (p->getParameter())
		{
			default:
				displayMode = DISPLAY_MODE_NUMERIC; // just show the number
				break;
			case PanelControl::CONTROL_PARAM_LFO_WAVE:
			case PanelControl::CONTROL_PARAM_LFO_SOURCE:
			case PanelControl::CONTROL_PARAM_LFO_OUTPUT_MODE:
			case PanelControl::CONTROL_PARAM_LFO_TRIGGER:
			case PanelControl::CONTROL_PARAM_M1_DESTINATION:
			case PanelControl::CONTROL_PARAM_M2_DESTINATION:
			case PanelControl::CONTROL_PARAM_M3_DESTINATION:
			case PanelControl::CONTROL_PARAM_M4_DESTINATION:
			case PanelControl::CONTROL_PARAM_NOTE_DESTINATION:
			case PanelControl::CONTROL_PARAM_VELOCITY_DESTINATION:
			//case PanelControl::CONTROL_PARAM_EG_SHAPE:
			case PanelControl::CONTROL_PARAM_EG_SOURCE:
			case PanelControl::CONTROL_PARAM_EG_RETRIG:
			case PanelControl::CONTROL_PARAM_PAGE_OPTION1:
			case PanelControl::CONTROL_PARAM_PAGE_OPTION2:
				displayMode = DISPLAY_MODE_COMBO;
				break;
			case PanelControl::CONTROL_PARAM_EG_DELAY:
				displayMode = DISPLAY_MODE_DELAY_TIME;
				break;
			case PanelControl::CONTROL_PARAM_EG_ATTACK:
			case PanelControl::CONTROL_PARAM_EG_DECAY:
			case PanelControl::CONTROL_PARAM_EG_RELEASE:
				displayMode = DISPLAY_MODE_EG_TIME;
				break;
			case PanelControl::CONTROL_PARAM_LFO_FREQ:
				displayMode = DISPLAY_MODE_LFO_RATE;
				break;

			case PanelControl::CONTROL_PARAM_EG_SUSTAIN:
				displayMode = DISPLAY_MODE_PERCENT; // always show % even on freq/VCF pages
				break;
			case PanelControl::CONTROL_PARAM_M1_ALT_LEVEL:
			case PanelControl::CONTROL_PARAM_M2_ALT_LEVEL:
			case PanelControl::CONTROL_PARAM_M3_ALT_LEVEL:
			case PanelControl::CONTROL_PARAM_M4_ALT_LEVEL:
			case PanelControl::CONTROL_PARAM_VELOCITY_ALT_LEVEL:
			case PanelControl::CONTROL_PARAM_NOTE_ALT_LEVEL:
				showPolarityPlus = true;
				displayMode = DISPLAY_MODE_PERCENT; // always show % even on freq/VCF pages
				break;
			case PanelControl::CONTROL_PARAM_OFFSET:
				displayMode = DISPLAY_MODE_PARAMETER_OFFSET;
				break;
			case PanelControl::CONTROL_PARAM_EG_MOD_DEPTH:
			case PanelControl::CONTROL_PARAM_LFOX_MOD_DEPTH:
			case PanelControl::CONTROL_PARAM_LFO_MOD_DEPTH:
			case PanelControl::CONTROL_PARAM_M1_LEVEL:
			case PanelControl::CONTROL_PARAM_M2_LEVEL:
			case PanelControl::CONTROL_PARAM_M3_LEVEL:
			case PanelControl::CONTROL_PARAM_M4_LEVEL:
			case PanelControl::CONTROL_PARAM_VELOCITY_LEVEL:
			case PanelControl::CONTROL_PARAM_NOTE_LEVEL:
				showPolarityPlus = true;
				displayMode = DISPLAY_MODE_DEFAULT; // sometimes %, sometimes pitch
				break;
		}
		switch (p->getPage())
		{
			default:
				page = PAGE_DEFAULT;
				break;
			case PanelControl::CONTROL_MASTER_PITCH:
				page = PAGE_MASTER_FREQ;
				break;

			case PanelControl::CONTROL_OSC1_PITCH:
			case PanelControl::CONTROL_OSC2_PITCH:
			case PanelControl::CONTROL_OSC3_PITCH:
				page = PAGE_OSC_FREQ;
				break;
			case PanelControl::CONTROL_LPF1:
				page = PAGE_LPF1_FREQ;
				break;
			case PanelControl::CONTROL_LPF2:
				page = PAGE_LPF2_FREQ;
				break;
			case PanelControl::CONTROL_HPF:
				page = PAGE_HPF_FREQ;
				break;

		}
	} else // globals, no page involved
	{
		switch (p->getParameter())
		{
			default:
				displayMode = DISPLAY_MODE_NUMERIC; // just show the number
				break;
			case PanelControl::CONTROL_GLOBAL_EGX1_DELAY:
			case PanelControl::CONTROL_GLOBAL_EGX2_DELAY:
			case PanelControl::CONTROL_GLOBAL_EGX3_DELAY:
			case PanelControl::CONTROL_GLOBAL_EGX4_DELAY:
				displayMode = DISPLAY_MODE_DELAY_TIME;
				break;
			case PanelControl::CONTROL_GLOBAL_LFOX1_TRIGGER:
			case PanelControl::CONTROL_GLOBAL_LFOX1_WAVEFORM:
			case PanelControl::CONTROL_GLOBAL_LFOX2_TRIGGER:
			case PanelControl::CONTROL_GLOBAL_LFOX2_WAVEFORM:
			case PanelControl::CONTROL_GLOBAL_LFOX3_TRIGGER:
			case PanelControl::CONTROL_GLOBAL_LFOX3_WAVEFORM:
			case PanelControl::CONTROL_GLOBAL_LFOX4_TRIGGER:
			case PanelControl::CONTROL_GLOBAL_LFOX4_WAVEFORM:
			case PanelControl::CONTROL_GLOBAL_EGX1_SHAPE:
			case PanelControl::CONTROL_GLOBAL_EGX1_TRIGGER:
			case PanelControl::CONTROL_GLOBAL_EGX2_SHAPE:
			case PanelControl::CONTROL_GLOBAL_EGX2_TRIGGER:
			case PanelControl::CONTROL_GLOBAL_EGX3_SHAPE:
			case PanelControl::CONTROL_GLOBAL_EGX3_TRIGGER:
			case PanelControl::CONTROL_GLOBAL_EGX4_SHAPE:
			case PanelControl::CONTROL_GLOBAL_EGX4_TRIGGER:
				displayMode = DISPLAY_MODE_COMBO;
				break;
			case PanelControl::CONTROL_GLOBAL_EGX1_ATTACK:
			case PanelControl::CONTROL_GLOBAL_EGX2_ATTACK:
			case PanelControl::CONTROL_GLOBAL_EGX3_ATTACK:
			case PanelControl::CONTROL_GLOBAL_EGX4_ATTACK:
			case PanelControl::CONTROL_GLOBAL_EGX1_DECAY:
			case PanelControl::CONTROL_GLOBAL_EGX2_DECAY:
			case PanelControl::CONTROL_GLOBAL_EGX3_DECAY:
			case PanelControl::CONTROL_GLOBAL_EGX4_DECAY:
			case PanelControl::CONTROL_GLOBAL_EGX1_RELEASE:
			case PanelControl::CONTROL_GLOBAL_EGX2_RELEASE:
			case PanelControl::CONTROL_GLOBAL_EGX3_RELEASE:
			case PanelControl::CONTROL_GLOBAL_EGX4_RELEASE:
				displayMode = DISPLAY_MODE_EG_TIME;
				break;
			case PanelControl::CONTROL_GLOBAL_LFOX1_FREQ:
			case PanelControl::CONTROL_GLOBAL_LFOX2_FREQ:
			case PanelControl::CONTROL_GLOBAL_LFOX3_FREQ:
			case PanelControl::CONTROL_GLOBAL_LFOX4_FREQ:
				displayMode = DISPLAY_MODE_LFO_RATE;
				break;
			case PanelControl::CONTROL_GLOBAL_EGX1_SUSTAIN:
			case PanelControl::CONTROL_GLOBAL_EGX2_SUSTAIN:
			case PanelControl::CONTROL_GLOBAL_EGX3_SUSTAIN:
			case PanelControl::CONTROL_GLOBAL_EGX4_SUSTAIN:
				displayMode = DISPLAY_MODE_PERCENT;
				break;
		}
	}



	if (displayMode == DISPLAY_MODE_NUMERIC)
		disp = String(value);
	else if (displayMode == DISPLAY_MODE_COMBO)
	{
		disp = p->getComboText();
	}
	else if (displayMode == DISPLAY_MODE_EG_TIME)
	{
		value = (int32_t) convertEGValueToTime(value);
		disp = 	getTimeString((100 * EG_RATE_MAX_VALUE) / (uint32_t) value);
	}
	else if (displayMode == DISPLAY_MODE_DELAY_TIME)
		disp =  getTimeString((( 100000 * (uint32_t) value ) / PARAMETER_UPDATE_RATE_IN_HZ));
	else if (displayMode == DISPLAY_MODE_LFO_RATE)
	{
		// multiply by 1000 to deal with decimal point
		uint32_t freq = (uint32_t) convertLFOrate(value);
		if (value >= 0)
		{
			if (freq < 4096)
				freq = (((uint32_t) (1000 / 2) * PARAMETER_UPDATE_RATE_IN_HZ * freq)) / LFO_MAX_AMPLITUDE ;
			else if (freq < 4096 * 16)
				freq = (((uint32_t) (1000 / (2 * 4)) * PARAMETER_UPDATE_RATE_IN_HZ * (freq / 4))) / (LFO_MAX_AMPLITUDE / 16) ;
			// halve since LFO goes from LFO_MAX_AMPLITUDE to -LFO_MAX_AMPLITUDE
			else
				freq = (((uint32_t) (1000 / (2 * 4)) * PARAMETER_UPDATE_RATE_IN_HZ * (freq / (4 * 64)))) / (LFO_MAX_AMPLITUDE / (16 * 64));

			if (freq > 100000)
			{
				disp = String(freq / 1000) + " Hz";
			}
			else if (freq > 10000)
			{
				disp = String(((float) (freq / 100)) / 10.0f, 1) + " Hz";
			}
			else if (freq > 1000)
			{
				disp = String(((float) (freq / 10)) / 100.0f, 2) + " Hz";
			} else
			{
				disp = String(((float) freq) / 1000.0f, 3) + " Hz";
			}
		} else
		{
			disp =  p->getComboText();
		}
	}
	else
	{
		if (page == PAGE_OSC_FREQ || page == PAGE_MASTER_FREQ)
		{
			if (displayMode == DISPLAY_MODE_PERCENT)
			{
				int32_t percent = (100*value) / MAX_LEVEL_VALUE;
				disp = String(percent) + " %";
				if (percent > 0 && showPolarityPlus)
					disp = "+" + disp;
			}
			else
			{
				int32_t semitones = (100*value) / (300 / 4); // x 100
				if (displayMode == DISPLAY_MODE_PARAMETER_OFFSET)
				{
					if (page == PAGE_MASTER_FREQ)
						semitones  = (100*(value)) / (300 / 4); // divide by 4 since pitch has rescale factor 4 at low level
					else
					{
						value *= 3;
						semitones  = (100*(value - (MAX_LEVEL_VALUE_FREQ / 2))) /300; // 300 units per semitone
					}
				}
				if (semitones >= 100 || semitones <= -100)
				{
					disp = String(((float) semitones) / 100.0f , 2) + " semi-tones";
				}
				else
				{
					disp = String( semitones) + " cents";
				}
				if (semitones > 0 )
					disp = "+" + disp;

			}
		} else if (page == PAGE_LPF1_FREQ
				|| page == PAGE_LPF2_FREQ
				|| page == PAGE_HPF_FREQ)
		{
			switch (displayMode)
			{
				case DISPLAY_MODE_PERCENT:
					{
						int32_t percent = (100*value) / MAX_LEVEL_VALUE;
						disp = String( percent) + " %";
						if (value > 0)
							disp = "+" + disp;
					}
					break;
				default:
					{
						int32_t octave = (100 * value) / 333; // x 100

						if (octave >= 1000 || octave  <= -1000)
							disp = String(((float) (octave / 10)) / 10.0f , 1) + " octave";
						else
							disp = String(((float) octave) / 100.0f, 2) + " octave";

						if (octave > 0)
							disp = "+" + disp;
					}
					break;
				case DISPLAY_MODE_PARAMETER_OFFSET:
					if (value <= 4095)
					{
						float b =  logf(2.0f) / VCF_STEPS_PER_OCTAVE; // this parameter is from the scaling factors from DAC to exponential current generation circuitry.
						float a;
						switch (page)
						{
							default:
							case PAGE_LPF1_FREQ:
								a = 110.0f / expf(VCF1_VALUE_FOR_110Hz * (logf(2.0f) / VCF_STEPS_PER_OCTAVE)); // the frequency when the voltage from DAC is zero volts.
								break;
							case PAGE_LPF2_FREQ:
								a = 110.0f / expf(VCF2_VALUE_FOR_110Hz * (logf(2.0f) / VCF_STEPS_PER_OCTAVE));
								break;
							case PAGE_HPF_FREQ:
								a = 110.0f / expf(HPF_VALUE_FOR_110Hz * (logf(2.0f) / VCF_STEPS_PER_OCTAVE));
								break;
						}
						int32_t freq = a * expf(b * value);
						if (freq >= 24000)
							freq = 24000;
						if (freq > 10000)
						{
							disp = String(((float) (freq / 100)) / 10.0f , 1) + " kHz";
						}
						else if (freq > 1000)
						{
							disp = String(((float) (freq / 10 )) / 100.0f , 2) + " kHz";
						}
						else
						{
							disp = String(freq) + " Hz";
						}
					}
					break;
			}
		}
		else
		{
			// just show percent with 1% resolution
			int32_t percent = (100*value) / MAX_LEVEL_VALUE;
			disp = String(percent) + " %";

			if (percent > 0 && showPolarityPlus)
				disp = "+" + disp;
		}
	}
	return disp;
}
*/
uint32_t Utilities::convertRawMIDI(uint8_t* buf, uint32_t totalSize)
{
    if (totalSize < 16)
    {
      	DBG("Error in size - too small: "  + String(totalSize));
        return 0 ;
    }

    uint32_t bufIndex = SYSEX_HEADER_BYTES;
    uint32_t convertedIndex = 0;
    uint16_t index = 0;
    while (bufIndex < totalSize)
    {
        // decode the MIDI buffer to the original form (with values > 0x7F)

 //       if (bufIndex < 4096* 3)
 //           qDebug() << "Buffer: " << QString::number(buf[bufIndex], 16) << " bufIndex: " << bufIndex;
        if (buf[bufIndex] == 0xF7)
        {
            // restart
            index = 0;
            bufIndex += SYSEX_HEADER_BYTES + 1;
        }
        if (bufIndex >= totalSize)
            break;
        int16_t c = convertMIDIbytes(buf[bufIndex], index);
        if (c >= 0)
        {
            buf[convertedIndex++] = (uint8_t) c;
        }
        index++;
        bufIndex++;
     }
    return convertedIndex;
}


uint32_t Utilities::prepareSysExPacketsFromUnpacked(uint8_t* outBuf, uint8_t* buffer, uint32_t totalDataSize, uint8_t type,
                                                       uint8_t msg0, uint8_t msg1, uint8_t msg2, uint8_t model, uint8_t channel )
{
    #define MAX_PACKET_LENGTH 64
    uint32_t bytesProcessed = 0;
    uint8_t  outBuffer[MAX_PACKET_LENGTH  + ((MAX_PACKET_LENGTH  + 6) / 7)];

    uint32_t bytesSent = 0;
    uint8_t startStopContinue = MIDI_SYSEX_START_TRANSFER;

    uint32_t bufferSize = totalDataSize;

    if (totalDataSize % 4 != 0)
    {
        // QMessageBox::warning(motasUtility, "Error", "Error in transfer - must be divisible by 4");
    	DBG("Error in transfer - must be divisible by 4");
         return 0;
    }

    uint32_t crc32 = crcCalculator::crcFastSTMF4(buffer, totalDataSize); // calculate CRC of all data
    totalDataSize += 4;// make room for CRC32;

    do
    {
        uint32_t k = 0;
        uint32_t packetLength = totalDataSize - bytesProcessed;
        if (packetLength > MAX_PACKET_LENGTH)
            packetLength = MAX_PACKET_LENGTH;
        uint32_t transferSize = packetLength  + ((packetLength  + 6) / 7);

        // convert to data suitable for MIDI;
        while (k < transferSize) // there is an MSB byte to be added for every 7 bytes, since cannot transmit values > 0x7F
        {
            uint8_t msb = 0;
            uint32_t j;
            for (j = 0; j < 7; j++)
            {
                uint32_t offset = bytesProcessed + j;
                if (offset >= totalDataSize)
                    break;
                if (offset < bufferSize)
                {
                    if (buffer[offset] > 0x7F)
                        msb |= (0x40 >> j);
                } else
                {
                    if (((crc32 >> ((offset - bufferSize) * 8)) & 0xFF) > 0x7F)
                        msb |= (0x40 >> j);
                }

            }
            outBuffer[k++] = msb;
            for (j = 0; j < 7; j++)
            {
                if (bytesProcessed >= totalDataSize)
                    break;
                if (k >= transferSize)
                    break;

                if ((bytesProcessed) < bufferSize)
                {
                    outBuffer[k++] =  buffer[bytesProcessed] & 0x7F;
                } else
                {
                    outBuffer[k++] =  (crc32 >> (bytesProcessed - bufferSize) * 8) & 0x7F;
                }
                bytesProcessed++;
            }
        }
        if (bytesProcessed >= totalDataSize)
        {
            startStopContinue = MIDI_SYSEX_END_TRANSFER;
            if (totalDataSize < MAX_PACKET_LENGTH)
            {
                startStopContinue = MIDI_SYSEX_START_AND_END_TRANSFER;
                //qDebug() << "Sending single packet";
            }
        }
        else if (bytesProcessed > packetLength)
            startStopContinue = MIDI_SYSEX_CONTINUE_TRANSFER;
        bytesSent += Utilities::lowLevelCreateMIDIStream(outBuffer, transferSize, type, startStopContinue, msg0, msg1, msg2,  &outBuf[bytesSent],
                                                         model, channel);
    } while (bytesProcessed < totalDataSize);

    return bytesSent;
}




int16_t Utilities::convertMIDIbytes(uint8_t currentByte, uint32_t i)
{


    int16_t outVal = -1;
    if ((i % 8) == 0) //  this byte is the MSB for next 7 bytes, since cannot transmit values > 0x7F over MIDI
    {
        msbByte = currentByte;
        msbCount = 1;
    } else
    {
        outVal = currentByte | ((msbByte << msbCount) & 0x80);
        msbCount++;
    }
    return outVal;
}


uint8_t Utilities::getNibble(uint8_t* in, uint16_t index)
{
    uint8_t nibble;
    int i = index / 2;
    if (i >= maxRLEIndex)
    	return 0;
    if (index % 2 == 0)
    {
        nibble =  in[i] & 0x0F;
    } else
    {
        nibble =  (in[i] >> 4) & 0x0F;
    }
    return nibble;
}


/**
 * Used to decode the screenshot bitmap data from Motas
 */

uint16_t  Utilities::runLengthDecode(uint8_t* in, uint16_t length, uint8_t* out, uint16_t maxOutIndex)
{
	maxRLEIndex = length;
    uint16_t i, j;
    uint8_t d;
    uint16_t c = 0;
    uint8_t code;
    bool repeated;
    length *= 2; // convert to nibbles
    for (i = 0; i < length; )
    {
        d = getNibble(in, i++);

        if ((d & 0x08) != 0)
            repeated = true;
        else
            repeated = false;
        d &= 0x07; // remove repeated flag
        switch (d)
        {
            default:
            case 0:
                code = 0x00 ;
                break;
            case 1:
                code = 0xFF;
                break;
            case 2:
                code = 0x0F;
                break;
            case 3:
                code = 0xF0;
                break;
            case 4:
                code = 0x10;
                break;
            case 5:
                code = 0x01;
                break;
            case 6:
                code = 0x11;
                break;
            case 7: // will always be 'repeated'
                code = (getNibble(in, i + 1) << 4)  +  getNibble(in, i + 2);
                break;

        }

        if (repeated)
        {
            uint8_t repeats = 1 + getNibble(in, i++);
            if (d == 7)
                i += 2;
            for (j = 0; j < repeats; j++)
            {
                if (c >= maxOutIndex)
                    break;
                 out[c++] = code;
            }
        } else
        {
            if (c >= maxOutIndex)
                break;
            out[c++] = code;
        }

        if (c >= maxOutIndex)
            break;
    }
    return c;
}




uint32_t Utilities::lowLevelCreateMIDIStream(uint8_t* buf, uint32_t size,  uint8_t type, uint8_t startStopContinue,
		uint8_t msg0, uint8_t msg1, uint8_t msg2, uint8_t* outBuf, uint8_t model, uint8_t channel)
{
    midi_package_t d;
    int32_t i = -4;

    uint16_t remainder = size % 3;
    uint32_t wholePackets = size / 3;
    uint32_t packetsSent = 0;
    uint32_t outCount = 0;

    // channel should be 1.. 16 here
    if (channel > 0)
    	channel -= 1;
    while (1)
    {
     //   d.cin_cable = 0x04; // start/continue sysex
        if (i == -4)
        {
            d.evnt_0.evnt0 = 0xF0; // start sysex
            d.evnt_1.value1 = MIDI_stuff::BYTE_SYSEX_ID1;
            d.evnt_1.value2 = MIDI_stuff::BYTE_SYSEX_ID2;
            i++;
        }
        else if (i == -3)
        {
            d.evnt_0.evnt0 =  MIDI_stuff::BYTE_SYSEX_ID3;
            d.evnt_1.value1 = model;
            d.evnt_1.value2 = channel & 0x0F;
            i++;
        }
        else if (i == -2)
        {
            d.evnt_0.evnt0 = 0;
            d.evnt_1.value1 = type;
            d.evnt_1.value2 = startStopContinue;
            i++;
        }
        else if (i == -1 && startStopContinue != MIDI_SYSEX_START_TRANSFER && startStopContinue != MIDI_SYSEX_START_AND_END_TRANSFER)
        {
            i++;
            continue; // skip a transfer
        }
        else if (i == -1)
        {
            // other meta-data
            d.evnt_0.evnt0 = msg0;
            d.evnt_1.value1 = msg1;
            d.evnt_1.value2 = msg2;
            i++;
        }
        else if (packetsSent < wholePackets)
        {
            d.evnt_0.evnt0 = buf[i++];
            d.evnt_1.value1 = buf[i++];
            d.evnt_1.value2 = buf[i++];
            packetsSent++;
        } else // final byte
        {
            break;
        }

        outBuf[outCount++] = d.evnt_0.evnt0;
        outBuf[outCount++] = d.evnt_1.value1;
        outBuf[outCount++] = d.evnt_1.value2;
    }

    // final packet
    if (remainder == 0)
    {
    //    d.cin_cable = 0x05; // end sysex
        d.evnt_0.evnt0 = 0xF7;
        d.evnt_1.value1 = 0;
        d.evnt_1.value2 = 0;
        outBuf[outCount++] = d.evnt_0.evnt0;
    }
    else if (remainder == 1)
    {
     //   d.cin_cable = 0x06; // end sysex
        d.evnt_0.evnt0 = buf[i] ;
        d.evnt_1.value1 = 0xF7;
        d.evnt_1.value2 = 0;
        outBuf[outCount++] = d.evnt_0.evnt0;
        outBuf[outCount++] = d.evnt_1.value1;
    } else if (remainder == 2)
    {
      //  d.cin_cable = 0x07; // end sysex
        d.evnt_0.evnt0 = buf[i++] ;
        d.evnt_1.value1 = buf[i] ;
        d.evnt_1.value2 = 0xF7;
        outBuf[outCount++] = d.evnt_0.evnt0;
        outBuf[outCount++] = d.evnt_1.value1;
        outBuf[outCount++] = d.evnt_1.value2;
    }

    return outCount;
}








Array<uint8_t> Utilities::prepareSysExPacket(int j, uint8_t* buffer, uint32_t packetSize, uint32_t fileLength, int32_t lastPacket, uint8_t type,
                                              uint8_t msg0, uint8_t msg1, uint8_t msg2, uint8_t modelNum, uint8_t channel )
{
	Array<uint8_t> message;
    message.add(MIDI_stuff::BYTE_SYSEX_START);
    message.add(MIDI_stuff::BYTE_SYSEX_ID1);
    message.add(MIDI_stuff::BYTE_SYSEX_ID2);

    message.add(MIDI_stuff::BYTE_SYSEX_ID3);
    message.add(modelNum);
    //message.append(MIDI_stuff::BYTE_SYSEX_PRODUCT_ID_2);


     // channel is from 1... 16 as called

    if (channel > 0)
    	channel -= 1;


    message.add(channel & 0x0F);

    message.add(0);
    message.add(type);
    if (j == 0)
        message.add(MIDI_SYSEX_START_TRANSFER);
    else if (j == lastPacket - 1)
    {
        message.add(MIDI_SYSEX_END_TRANSFER);
        //qDebug() << "MidiSettingsDialog:  FINAL firmware packet. " ;
    }
    else
        message.add(MIDI_SYSEX_CONTINUE_TRANSFER);


    if (j == 0)
    {
        message.add(msg0 & 0x7F);
        message.add(msg1 & 0x7F);
        message.add(msg2 & 0x7F);
    }
    for (uint32_t i = 0 ; i < packetSize; i++)
    {
        uint32_t index = i + (uint32_t) j * packetSize;
        if (index >= fileLength)
            break;
        message.add(buffer[index]);
    }
    message.add(MIDI_stuff::BYTE_SYSEX_END);
    return message;
}



uint32_t Utilities::processBufferForMIDISend(char* outBuffer, char* buffer, uint32_t originalDataLength)
{
    // now adjust binary file to allow send over MIDI - change data format to encode values > 0x7F
   // uint32_t j = 0;
    uint32_t k = 0;
    uint8_t msb = 0;
    for (uint32_t i = 0; i < originalDataLength; i += 7) // there is a MSB byte to be added for every 7 bytes, since cannot transmit values > 0x7F
    {
        msb = 0;
        for (uint32_t j = 0; j < 7; j++)
        {
            if ((i + j) >= originalDataLength)
                break;
            if ((uint8_t) buffer[i + j] > 0x7F)
                msb |= (0x40 >> j);
        }
        outBuffer[k++] = (char) msb;
        for (uint32_t j = 0; j < 7; j++)
        {
            if ((i + j) >= originalDataLength)
                break;
            outBuffer[k++] = ((uint8_t) buffer[i + j]) & 0x7F;
        }
    }
    return k;
}


void Utilities::write32(uint32_t val, int offset, uint8_t* buffer)
{

    for (int i = 0; i < 4; i++)
    {
         //qDebug() << (((uint32_t) this->midiStreamPatch->at(offset + i)) << (8 * (i)));
       buffer[offset + i]  = (val >> (8 * i)) & 0xFF;
    }
}

uint32_t Utilities::read32(int offset, uint8_t* buffer)
{
    uint32_t val = 0;
    if (buffer == nullptr)
    	return 0;
    for (int i = 0; i < 4; i++)
    {
         //qDebug() << (((uint32_t) this->midiStreamPatch->at(offset + i)) << (8 * (i)));
        val += ((uint32_t) buffer[offset + i]) << (8 * (i));
    }
    return val;
}
void  Utilities::setScreenShotColour(int choice)
{
	//invertScreenColour = false;
	switch (choice)
	{
		default:
		case 0:
			screenshotColour = Colours::white;
			screenshotColourBG  = Colours::black;
			break;
		case 1:
			screenshotColour = Colours::yellow;
			screenshotColourBG  = Colours::black;

			break;
		case 2:
			screenshotColour = Colours::red;
			screenshotColourBG  = Colours::black;
			break;
		case 3:
			screenshotColour = Colours::green;
			screenshotColourBG  = Colours::black;
			break;
		case 4:
			screenshotColour = Colours::blue;
			screenshotColourBG  = Colours::black;
			break;

		case 5:
			screenshotColour = Colours::blue;
			screenshotColourBG = screenshotColour.contrasting(0.5f);
			break;
		case 6:
			screenshotColour = Colour(88, 91, 79);
			screenshotColourBG = screenshotColour.contrasting(0.5f);
			//invertScreenColour = true;
			break;

	}

}
Colour Utilities::getPixelColour(uint8_t data)
{

	/*if (invertScreenColour)
	{
		 return Colour( (uint8_t) (screenshotColour.getFloatRed() * (255 - data)),
				(uint8_t) ( screenshotColour.getFloatGreen() * (255 - data)),
				(uint8_t) (screenshotColour.getFloatBlue() * (255 - data)));
	} else*/
	{
		if (data == 0)
			return	screenshotColourBG;
		else
		 return Colour( (uint8_t) (screenshotColour.getFloatRed() * data),
				(uint8_t) ( screenshotColour.getFloatGreen() * data),
				(uint8_t) (screenshotColour.getFloatBlue() * data));

	}


}





void Utilities::sendMIDISysExCommand(String , char msg1, char msg2)
{


	DBG("Send SysEx command");
    //if (this->midiOut->isPortOpen())
    {
        Array<uint8_t> message;
        message.add(MIDI_stuff::BYTE_SYSEX_START);
        message.add(MIDI_stuff::BYTE_SYSEX_ID1);
        message.add(MIDI_stuff::BYTE_SYSEX_ID2);

        message.add(MIDI_stuff::BYTE_SYSEX_ID3);
  //      message.add(ui->spinBoxMotasModel->value());
  //      message.add(ui->spinBoxChannel->value() - 1);

        message.add(6);
        message.add(1);


        message.add( 0);
        message.add((uint8_t) msg1);
        message.add((uint8_t)msg2);

        message.add(MIDI_stuff::BYTE_SYSEX_END);



      // std::vector<unsigned char> m = message.;
       // countDownTimerMIDIOut = 6;
      //  this->midiOut->sendMessage( &m );

      //  qDebug() << "MidiSettingsDialog:  sending SYSEX command: " << name;

    }/* else
    {
        QMessageBox::warning(this, "Error", "No MIDI out port!.");
        ui->checkBoxSendBitmap->setChecked(false);
        ui->checkBoxRequestOscilloscopeData->setChecked(false);
    }*/
}



