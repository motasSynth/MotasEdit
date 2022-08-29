
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

    PanelGraphic.cpp

  ==============================================================================
*/

#include "./Headers/PanelGraphic.h"

#include "./Headers/images.h"

#define SCALE_FACTOR 20
PanelGraphic::PanelGraphic(const char* resourceDataImage, const int resourceDataSize, float x, float y)
: x(x), y(y)
{


	//image = Drawable::createFromImageFile(File(IMAGE_FILE_LOCATION + fileName +  ".svg"));

	image = Drawable::createFromImageData(resourceDataImage, (size_t) resourceDataSize);

	if (image == nullptr)
	{
		DrawableText* t = new DrawableText();
		t->setText("Image not found!");
		image.reset(t);
	}
	w = image->getWidth() / 2.0f;
	//h = image->getHeight() / 2.0f;

	h = 7.5; // fix the height

	//DBG("Icon height: " + String(h));
}

void PanelGraphic::drawGraphic(Graphics& g, float scaleX, float scaleY, Colour &c)
{
	//image->drawWithin(g, x * scaleX - w, y * scaleY - h, 1);

	//g.setColour(Colour(0xFF, 0xFF, 0xFF));
	g.setColour(c);
	float xPos = x * scaleX;
	float yPos = y * scaleY;

	float width = SCALE_FACTOR*w*scaleX *0.75f;
	float height = SCALE_FACTOR*h*scaleY *1.1f;

	Rectangle<float> r;
	r.setBounds(0, 0 ,width, height);
	r.setCentre(xPos, yPos);


	height/=1.1f;
	g.fillRect(r);


	r.setBounds(xPos, yPos ,width * 100 , height); // increase width so height is the restricting dimension
	r.setCentre(xPos, yPos);
	image->drawWithin(g, r, RectanglePlacement::centred, 1);

}


