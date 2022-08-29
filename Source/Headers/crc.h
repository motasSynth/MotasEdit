/**********************************************************************
 *
 * Filename:    crc.h
 * 
 * Description: A header file describing the various CRC standards.
 *
 * Notes:       
 *
 * 
 * Copyright (c) 2000 by Michael Barr.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 *
 *
 **********************************************************************/


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




#include "stdint.h"
#include "ApplicationCommon.h"
#include "motasDefines.h"


#ifndef _crc_h
#define _crc_h


#define FALSE	0
#define TRUE	!FALSE

/*
 * Select the CRC standard from the list that follows.
 */
#define CRC32_STMF4


#if defined(CRC32_STMF4)



#define CRC_NAME			"CRC-32-STMF4"
#define POLYNOMIAL			0x04C11DB7
#define INITIAL_REMAINDER	0xFFFFFFFF
#define FINAL_XOR_VALUE		0x00000000
#define CHECK_VALUE			0x00


const int patchCRCNumBytes = (4096 - 8);


#else

#error

#endif

class crcCalculator
{
public:
	static uint32_t   crcFastSTMF4(const uint8_t message[], uint32_t nBytes);
	uint32_t   crcFastSTMF4Patch(const tAllParams* const patch);
	void updatePatchCRC(tAllParams* patch);
private:

	uint8_t patchBuffer[patchCRCNumBytes];
	static uint32_t  crcTable[];
	static bool isInitialised;
	static void  crcInit(void);
	static uint32_t   crcFast(const uint8_t message[], uint32_t nBytes);
};




#endif /* _crc_h */

