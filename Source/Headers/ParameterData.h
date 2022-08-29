
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
 * ParameterData.h
 *
 *
 */

#ifndef SOURCE_PARAMETERDATA_H_
#define SOURCE_PARAMETERDATA_H_

#include "ApplicationCommon.h"
#include "utilities.h"
#pragma once


class ParameterData
{



public:
	ParameterData(int type);
	void initialise(int panelCId, int page, int parameter, String controlName);
	void setPage(int page);
	void setParameter(int param);
	//void setMinMax(int32_t min, int32_t max);

	void setSlider(Slider* s);
	void setComboBox(ComboBox* c);
	void setButton(Button* b);



	int getPanelControlId() const;
	int getPage() const;
	int getParameter() const;

	NRPNmessage_t convertParameterDataToNRPN();


	static void populateComboBox(ComboBox* c, int type);
	static NRPNmessage_t convertBufferToNRPN(uint8_t* buf);


	uint8_t getMSB() const;
	uint8_t getLSB() const;
	void setValue(int32_t val);
	void setPanelValue(int32_t val);
	void setValueFromPatch(int32_t val);

	int32_t getPanelValue();

	String getName() const;
	int getMax();
	void setMax(int maxValue);
	int getMin();
//	void setInverted(bool inverted);

	int getSubId() const;

	//int32_t getValueForPatch() const;


	int32_t defaultValue;
	void setToDefault();
	Slider* getSlider();

	String getComboText();
	String getDisplayHumanText();

	void setIncrementStep(int32_t stepValue);
	int32_t getIncrementStep(bool bigStep);
//	bool isInverted();
private:
	int32_t incrementStep;
	String name;
	Button* button;
	ComboBox* comboBox;


	Slider* slider;
	//int32_t valueForPatch;
	int subId;
	int panelControlId;

	int32_t panelValue;

	bool isPitch;
	int32_t max;
	int32_t min;
	uint8_t msb;
	uint8_t lsb;

	//uint32_t msblsb;
	int parameterPage;

	int parameterType;

public:
	bool isParameterPageFixed;
private:
//	bool inverted;












};

#endif /* SOURCE_PARAMETERDATA_H_ */
