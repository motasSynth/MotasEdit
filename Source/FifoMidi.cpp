

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
 * FifoMidiIn.cpp

 */

#include "Headers/FifoMidi.h"

FifoMidi::FifoMidi(int size)  : abstractFifo (size), maxSize(size)
{
	myBuffer = std::make_unique<uint8_t[]>((uint32_t) size);
}


bool FifoMidi::addToFifo (const uint8_t* someData, int numItems)
{
	int freeSpace = abstractFifo.getFreeSpace();
	if (freeSpace < numItems)
	{
		DBG(" ");
		DBG(" ");
		DBG("FIFO full!!");
		DBG(" ");
		DBG(" ");

		jassert(false);

		return false;
	}

	int start1, size1, start2, size2;
	abstractFifo.prepareToWrite (numItems, start1, size1, start2, size2);
	if (size1 > 0)
		copySomeData (myBuffer.get() + start1, someData, size1);
	if (size2 > 0)
		copySomeData (myBuffer.get() + start2, someData + size1, size2);
	abstractFifo.finishedWrite (size1 + size2);
	return true;
}
void FifoMidi::readFromFifo (uint8_t* someData, int numItems, bool commit)
{
	int start1, size1, start2, size2;
	abstractFifo.prepareToRead (numItems, start1, size1, start2, size2);
	if (size1 > 0)
		copySomeData (someData, myBuffer.get() + start1, size1);
	if (size2 > 0)
		copySomeData (someData + size1, myBuffer.get() + start2, size2);
	if (commit)
		abstractFifo.finishedRead (size1 + size2);
}

bool FifoMidi::isAnyDataReady()
{
	return abstractFifo.getNumReady() > 0;
}
int FifoMidi::getNumReady()
{
	return abstractFifo.getNumReady();
}
void FifoMidi::copySomeData(uint8_t* location, const uint8_t* data, int size)
{
	memcpy(location, data, (size_t) size);
}
int FifoMidi::readSomeFromFifo (uint8_t* someData, int maxItems, bool commit)
{
	int numItems = abstractFifo.getNumReady();

	if (numItems > maxItems)
		numItems = maxItems;

	readFromFifo(someData, numItems, commit);
	return numItems;
}
int FifoMidi::readAllFromFifo(uint8_t* someData, int minBlockSize)
{
	int numItems = abstractFifo.getNumReady();



	readFromFifo(someData, minBlockSize * (numItems / minBlockSize), true);
	return numItems;

}
