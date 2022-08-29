
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
  ==============================================================================

    PanelGraphic.h


  ==============================================================================
*/

#pragma once
#include "ApplicationCommon.h"
class PanelGraphic
{
public:

	PanelGraphic(const char* resourceDataImage, const int resourceDataSize, float x, float y);


	void drawGraphic(Graphics& g, float scaleX, float scaleY, Colour &c);

private:
	std::unique_ptr<Drawable> image;
	float h;
	float w;

	float x;
	float y;
};
