
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

    CustomSlider.h

  ==============================================================================
*/

#pragma once


#include "../JuceLibraryCode/JuceHeader.h"
class PanelControl;
class CustomSlider : public Slider
{
public:
	//CustomSlider(Slider::SliderStyle style, TextEntryBoxPosition textBoxPos);
	CustomSlider(DrawableButton* button, PanelControl* owner);


	bool keyPressed (const KeyPress& key) override;


	void setActive();


	void mouseDown(const MouseEvent&) override;
	void mouseUp(const MouseEvent&) override;

	void mouseDoubleClick (const MouseEvent&) override;

private:

	PanelControl* panelControl;
	DrawableButton* button;
public:
	bool isSliderClicked;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomSlider)
};
