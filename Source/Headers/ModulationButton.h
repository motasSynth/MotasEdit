
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

    ModulationButton.h


  ==============================================================================
*/

#pragma once

#include "ApplicationCommon.h"
//==============================================================================
/*
 *
 * Contains 2 sliders (one for main, one for secondary modulation amount) and combobox for secondary destination
*/
class ModulationButton
{
public:
    ModulationButton(Component* boss);
    ~ModulationButton();


    void setPosition(int x, int y);


private:
    std::unique_ptr<Slider> mainSlider;
    std::unique_ptr<Slider> secondarySlider;
    std::unique_ptr<ComboBox> comboBoxSecondarySelection;

    Component* owner;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ModulationButton)
};
