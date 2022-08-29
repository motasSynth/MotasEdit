
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

    ModulationButton.cpp


  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "./Headers/ModulationButton.h"


//==============================================================================
ModulationButton::ModulationButton(Component* ow)
: owner(ow)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.


	mainSlider =  std::make_unique<Slider>( Slider::LinearVertical, Slider::NoTextBox);
	secondarySlider =  std::make_unique<Slider>( Slider::LinearVertical, Slider::NoTextBox);
	comboBoxSecondarySelection =  std::make_unique< ComboBox>();

	comboBoxSecondarySelection->addItem("LFOX AMOUNT", 1);
	comboBoxSecondarySelection->addItem("LFO FREQ", 2);
	comboBoxSecondarySelection->addItem("LFO AMOUNT", 3);
	comboBoxSecondarySelection->addItem("EG RATE", 4);
	comboBoxSecondarySelection->addItem("EG ATTACK", 5);
	comboBoxSecondarySelection->addItem("EG DECAY", 6);
	comboBoxSecondarySelection->addItem("EG SUSTAIN", 7);
	comboBoxSecondarySelection->addItem("EG RELEASE", 8);
	comboBoxSecondarySelection->addItem("EG AMOUNT", 9);

	owner->addAndMakeVisible(*mainSlider);
	owner->addAndMakeVisible(*secondarySlider);
	owner->addAndMakeVisible(*comboBoxSecondarySelection);

}

ModulationButton::~ModulationButton()
{
	DBG("ModulationButton destructor");
}

#define SLIDER_SEPARATION 20
void ModulationButton::setPosition(int x, int y)
{
	mainSlider->setBounds(x, y, SLIDER_SEPARATION, 100);
	secondarySlider->setBounds(x+SLIDER_SEPARATION, y, SLIDER_SEPARATION, 100);
	comboBoxSecondarySelection->setBounds(x, y-50, 100, 20);

}
