

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

    CustomSlider.cpp


  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "Headers/CustomSlider.h"

#include "Headers/PanelControl.h"
/*
CustomSlider::CustomSlider(SliderStyle style, TextEntryBoxPosition textBoxPos)
: Slider (style, textBoxPos), button(nullptr), isSliderClicked(false)
{

	setWantsKeyboardFocus(true);
	panelControl = nullptr;
}*/

CustomSlider::CustomSlider(DrawableButton* b, PanelControl* owner)
: Slider(Slider::RotaryHorizontalVerticalDrag, Slider::NoTextBox), button(b), isSliderClicked(false)
{

	setWantsKeyboardFocus(true);
	panelControl = owner;

}


void CustomSlider::mouseDown(const MouseEvent&m )
{

	//DBG("mouse down on slider");
	isSliderClicked = true;
	if (button != nullptr)
	{
		button->setToggleState(true, NotificationType::sendNotification);
	}
	Slider::mouseDown(m);

}

void CustomSlider::mouseUp(const MouseEvent& m)
{
	//DBG("mouse up on slider");
	isSliderClicked = false;

	Slider::mouseUp(m);
}

bool CustomSlider::keyPressed(const KeyPress& key)
{
	bool status = false;
	int32_t increment = 1;
	int sign = 1;
	double sliderValue = this->getValue();

	if (KeyPress::isKeyCurrentlyDown(KeyPress::leftKey))
	{
		sign = -1;
		if (key.getModifiers().isShiftDown())
			increment *= 10;
		if (key.getModifiers().isCtrlDown())
			increment *= 10;

		status = true;
	} else if (KeyPress::isKeyCurrentlyDown(KeyPress::rightKey))
	{

		if (key.getModifiers().isShiftDown())
			increment *= 10;
		if (key.getModifiers().isCtrlDown())
			increment *= 10;

		this->setValue(sliderValue + increment, NotificationType::sendNotification);
		status = true;
	}
	else if (KeyPress::isKeyCurrentlyDown(KeyPress::upKey))
	{
		increment *= 10;
		if (key.getModifiers().isShiftDown())
			increment *= 10;
		if (key.getModifiers().isCtrlDown())
			increment *= 10;

		this->setValue(sliderValue + increment, NotificationType::sendNotification);
		status = true;
	} else if (KeyPress::isKeyCurrentlyDown(KeyPress::downKey))
	{
		increment *= 10;
		sign = -1;
		if (key.getModifiers().isShiftDown())
			increment *= 10;
		if (key.getModifiers().isCtrlDown())
			increment *= 10;
		status = true;
	}
	if (status)
	{
		this->setValue(sliderValue + increment * sign, NotificationType::sendNotification);
	}

	return status;
}

void CustomSlider::setActive()
{
	//this->toFront(true);
	this->grabKeyboardFocus();
	//double sliderValue = this->getValue();
	//this->setValue(sliderValue + 1000);
}

void CustomSlider::mouseDoubleClick(const MouseEvent&m)
{
	//DBG("mouse double click on slider");
	panelControl->dClick(this);
	Slider::mouseDoubleClick(m);


}
