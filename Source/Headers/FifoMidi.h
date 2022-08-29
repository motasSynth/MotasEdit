
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
 * FifoMidiIn.h
 *
 *
 */

#ifndef SOURCE_FIFOMIDIIN_H_
#define SOURCE_FIFOMIDIIN_H_

#include "ApplicationCommon.h"





class FifoMidi
{

public:
	FifoMidi(int size);
    bool addToFifo (const uint8_t* someData, int numItems);

    int readAllFromFifo (uint8_t* someData, int minBlockSize);

    int readSomeFromFifo (uint8_t* someData, int maxItems, bool commit);

    bool isAnyDataReady();
    int 	getNumReady();

private:
    void readFromFifo (uint8_t* someData, int numItems, bool commit);
    AbstractFifo abstractFifo;
    std::unique_ptr<uint8_t[]> myBuffer;
    int maxSize;
    void copySomeData(uint8_t* location, const uint8_t* data, int size);

};

#endif /* SOURCE_FIFOMIDIIN_H_ */
