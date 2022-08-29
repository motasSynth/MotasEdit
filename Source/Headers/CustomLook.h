
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

    CustomLook.h


  ==============================================================================
*/

#pragma once

#include "ApplicationCommon.h"
//==============================================================================
/*
*/
class CustomLook    : public LookAndFeel_V4
{
public:
    CustomLook(int type);


    int type;

    void drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
                                  bool isMouseOverButton, bool isButtonDown) override;

     void drawDrawableButton	(	Graphics & 	,    DrawableButton & 	,    bool 	isMouseOverButton,    bool 	isButtonDown   )	override;


     void drawTabButton (TabBarButton& button, Graphics& g, bool isMouseOver, bool isMouseDown) override;
     void 	drawButtonText (Graphics &, TextButton &, bool isMouseOverButton, bool isButtonDown) override;


     void drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                               float rotaryStartAngle, float rotaryEndAngle, Slider& slider) override;


     void drawLinearSlider	(	Graphics & 	, 		 int 	x,
    		 int 	y,    		 int 	width,    		 int 	height,    		 float 	sliderPos,
    		 float 	minSliderPos,    		 float 	maxSliderPos,    		 const Slider::SliderStyle 	,
    		 Slider &
    		 )	override;

     void drawComboBox (Graphics& g, int width, int height, bool,
                                        int, int, int, int, ComboBox& box) override;


     void drawTextEditorOutline	(	Graphics & g,     int 	width,     int 	height,     TextEditor & 	t   )	override;

     void fillTextEditorBackground (Graphics& g, int width, int height, TextEditor& textEditor) override;


     void positionComboBoxText (ComboBox& box, Label& label) override;

     //Typeface::Ptr getTypefaceForFont() override;



private:
     Image knobImageRed ;
     Image* knobImage ;

     Image knobImageYellow;
     Image knobImageBlur ;

     float cornerSizeBase = 0.01f;
     float cornerMax = 4.0f;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomLook)
};
