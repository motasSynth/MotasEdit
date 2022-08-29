
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
 * MotasPluginParameter.h
 *
 *
 */

#ifndef SOURCE_MOTASPLUGINPARAMETER_H_
#define SOURCE_MOTASPLUGINPARAMETER_H_
#include "ApplicationCommon.h"
class MotasEditPluginAudioProcessor;
class ParameterData;

class MotasPluginParameter : public AudioParameterInt
{
public:
	MotasPluginParameter(MotasEditPluginAudioProcessor* processor,
			const String& parameterID, const String& name,
			int parameterPage, int subParam, int min = 0, int max = 4095, int32_t defaultValue = 2048,
			std::function< String(int value, int maximumStringLength)>  lambdaFunction = nullptr);
			//std::function<void()>  lambdaFunction = nullptr);
	virtual ~MotasPluginParameter();



	enum
	{
		SOURCE_FROM_DAW,
		SOURCE_INTERNAL,
	};

	using AudioProcessorParameter::isDiscrete;
	bool isDiscrete();

	using AudioProcessorParameter::isOrientationInverted;

	bool isOrientationInverted();
	void getNRPNBasic(uint8_t* buffer, int newValue);

	void valueChanged(int newValue) override;


	//String getReadableValue(int value, int maxLength);

	void setValueInternal(uint8_t msb, uint8_t lsb);

	//void setValue(float newValue) override;
private:
	bool isModulation();
	bool isLFOWaveform();

	bool isEGRate();

	bool isLFOFreq();
	bool isPitch();


	MotasEditPluginAudioProcessor* processor;
	int paramPage;
	int subParam;

	uint8_t source;

};

#endif /* SOURCE_MOTASPLUGINPARAMETER_H_ */
