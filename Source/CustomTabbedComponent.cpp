
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
 * CustomTabbedComponent.cpp
 *
 *
 */

#include "Headers/CustomTabbedComponent.h"
#include "../binaryResources/MotasEditResources.h"
CustomTabbedComponent::CustomTabbedComponent(TabbedButtonBar::Orientation orientation)
: TabbedComponent (orientation)
{
	// TODO Auto-generated constructor stub

	bgImage = ImageFileFormat::loadFrom(MotasEditResources::ss_jpg, MotasEditResources::ss_jpgSize);
	//image = ImageFileFormat::loadFrom(MotasEditResources::knobBlur_png, MotasEditResources::knobBlur_pngSize);

	backgroundDrawn = false;
	useBGImage = !true;
}



void CustomTabbedComponent::paint2(Graphics& g)
{


	//DBG("Tabbed component: paint");

	//TabbedComponent::paint(g);
	if (useBGImage && !backgroundDrawn && bgImage.isValid())
	{
		//g.drawImageAt (bgImage, 0, 0);


		AffineTransform af = AffineTransform::translation(-bgImage.getWidth()/2.0f, -bgImage.getHeight()/2.0f);


		af = af.scaled(g.getClipBounds().getWidth()/ (float) bgImage.getWidth(),
				g.getClipBounds().getHeight()/ (float) bgImage.getHeight(), 0,0);

		af = af.translated(g.getClipBounds().getWidth()/2.0f, g.getClipBounds().getHeight()/2.0f);


		DBG(g.getClipBounds().getWidth());
		g.drawImageTransformed(bgImage, af, false);
				//g.drawImage(image, 0, 0, 100, 100, 0, 0, 128, 128);

		//backgroundDrawn = true;
		//DBG("Draw image");
	} else if (!bgImage.isValid())
	{
		//DBG("Draw image: invalid");


	}

	if (!useBGImage || !bgImage.isValid())
	{
		TabbedComponent::paint(g);
	}

}
