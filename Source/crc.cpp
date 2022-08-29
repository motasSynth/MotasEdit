/**********************************************************************
 *
 * Filename:    crc.c
 * 
 * Description: Slow and fast implementations of the CRC standards.
 *
 * Notes:       The parameters for each supported CRC standard are
 *				defined in the header file crc.h.  The implementations
 *				here should stand up to further additions to that list.
 *
 * 
 * Copyright (c) 2000 by Michael Barr.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 *
 *

 *
 *
 *
 *


 **********************************************************************/
 
/*
Copyright
2023
J. M. Hayes
Motas Electronics Limited
Modified 2018 (turned into a c++ class and customised for STMF4 algorithm)

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


#include "./Headers/crc.h"

#include "Headers/utilities.h"
/*
 * Derive parameters from the standard-specific parameters in crc.h.
 */
#define WIDTH    (8UL * sizeof(uint32_t))
#define TOPBIT   (1UL << (WIDTH - 1UL))



bool crcCalculator::isInitialised = false;
uint32_t crcCalculator::crcTable[256];

/*********************************************************************
 *
 * Function:    crcInit()
 * 
 * Description: Populate the partial CRC lookup table.
 *
 * Notes:		This function must be rerun any time the CRC standard
 *				is changed.  If desired, it can be run "offline" and
 *				the table results stored in an embedded system's ROM.
 *
 * Returns:		None defined.
 *
 *********************************************************************/
void crcCalculator::crcInit(void)
{
	uint32_t	remainder;
    uint32_t 	dividend;
    uint8_t  	bit;

    /*
     * Compute the remainder of each possible dividend.
     */
    for (dividend = 0; dividend < 256; ++dividend)
    {
        /*
         * Start with the dividend followed by zeros.
         */
        remainder = dividend << (WIDTH - 8);

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for (bit = 8; bit > 0; --bit)
        {
            /*
             * Try to divide the current data bit.
             */			
            if (remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }

        /*
         * Store the result into the table.
         */
        crcTable[dividend] = remainder;
    }
}   /* crcInit() */


/*********************************************************************
 *
 * Function:    crcFast()
 * 
 * Description: Compute the CRC of a given message.
 *
 * Notes:		crcInit() must be called first.
 *
 * Returns:		The CRC of the message.
 *
 *********************************************************************/
uint32_t crcCalculator::crcFast(uint8_t const message[], uint32_t nBytes)
{
	uint32_t remainder = INITIAL_REMAINDER;
    uint8_t  data;
    uint32_t  byte;

    if (!isInitialised)
    {
    	crcInit();
    	isInitialised = true;
    }

    /*
     * Divide the message by the polynomial, a byte at a time.
     */
    for (byte = 0; byte < nBytes; ++byte)
    {
        data = message[byte] ^ (remainder >> (WIDTH - 8));
  		remainder = crcTable[data] ^ (remainder << 8);
    }

    /*
     * The final remainder is the CRC.
     */
    return (remainder ^ FINAL_XOR_VALUE);

}   /* crcFast() */


uint32_t crcCalculator::crcFastSTMF4(uint8_t const message[], uint32_t nBytes)
{
    if (nBytes % 4 != 0)
        return 0;
    uint8_t* buf = new uint8_t[nBytes];
    uint32_t i = 0;
    for (i = 0; i < nBytes; i += 4)
    {
    	// reverse for endian correction
        buf[i] = message[i + 3];
        buf[i+1] = message[i + 2];
        buf[i+2] = message[i + 1];
        buf[i+3] = message[i];
    }
    uint32_t result =  crcFast(buf , nBytes);
    delete[] buf;
    return result;
}

//uint8_t crcCalculator::patchBuffer[patchCRCNumBytes];


/**
 *
 * Faster version for patches with memory allocation
 */
uint32_t crcCalculator::crcFastSTMF4Patch(const tAllParams* const patch)
{
    int32_t i = 0;
    uint8_t* p = (uint8_t*) patch;
    for (i = 0; i < patchCRCNumBytes; i += 4)
    {
    	// reverse for endian correction
    	patchBuffer[i] 	 = p[i + 8 + 3]; // offset 8 to jump over identifier and crc values
    	patchBuffer[i+1] = p[i + 8 + 2];
    	patchBuffer[i+2] = p[i + 8 + 1];
    	patchBuffer[i+3] = p[i + 8 + 0];
    }
    uint32_t result =  crcFast(patchBuffer, patchCRCNumBytes);
    return result;
}

void crcCalculator::updatePatchCRC(tAllParams* patch)
{
	#define PATCH_IDENTIFIER ((uint32_t) 0x3561706D)
	patch->header.identifier = PATCH_IDENTIFIER;

	uint32_t crcVal = crcFastSTMF4Patch(patch); // calculate CRC of all data (excluding first 8 bytes, and final 4)

	Utilities::write32(crcVal, 4, (uint8_t*) patch); // write new CRC to the patchbuffer

}



