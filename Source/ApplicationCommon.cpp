

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

    ApplicationCommon.cpp

  ==============================================================================
*/

/*
****************
****************
****************

TO COMPILE ON WINDOWS:

****************
****************
****************

C:\Program Files (x86)\Microsoft Visual Studio\2017\Community>


#On windows 10 to build plugin. First clean the paths.
msbuild.exe -m:2 /target:clean /p:Configuration=Release E:\Motas\Juce\MotasEditPlugin\ProducerFiles\Plugin\Builds\VisualStudio2017\MotasEditPlugin.sln

# then do the build!
msbuild.exe -m:2 /p:Configuration=Release E:\Motas\Juce\MotasEditPlugin\ProducerFiles\Plugin\Builds\VisualStudio2017\MotasEditPlugin.sln


****************
****************
****************



****************
****************

*/

#include "Headers/ApplicationCommon.h"
#include "Headers/MIDIProcessor.h"
#include "../binaryResources/MotasEditResources.h"
#include "Headers/crc.h"
#include "Headers/PatchPatternSequence.h"

ApplicationCommon::ApplicationCommon()
: midiProcessor(nullptr)
{
	prevMatchState = false;
	activePatchNumber = 0;
	firstTimeStartup = true;
	originalMatchesEditedPatch = false;
	aPatchHasChanged = true;

	jassert(MotasEditResources::FACTORY_BANK0_FINAL_rev1_bpatchSize == (PATCH_LENGTH_BYTES_UNPACKED * BANK_OF_PATCHES_LENGTH));

#if BANK_OF_PRESETS_LENGTH > BANK_OF_PATCHES_LENGTH
	#error
#endif


	for (int i = 0; i < BANK_OF_PRESETS_LENGTH; i++)
	{
		patchPresetsArray.add(new uint8[PATCH_LENGTH_BYTES_UNPACKED]);
		patchPresetsEditedArray.add(new uint8[PATCH_LENGTH_BYTES_UNPACKED]);

		memcpy(patchPresetsArray[i],
				MotasEditResources::FACTORY_BANK0_FINAL_rev1_bpatch + ((uint32_t) i * PATCH_LENGTH_BYTES_UNPACKED),
				PATCH_LENGTH_BYTES_UNPACKED);
		PatchPatternSequence::convertPatchToLatestVersion((tAllParams*) patchPresetsArray[i]);


		// now copy into the edits array too
		memcpy(patchPresetsEditedArray[i], patchPresetsArray[i], PATCH_LENGTH_BYTES_UNPACKED);


	}

	activePatch = patchPresetsEditedArray[0];
	setGUIPatch();
	// fill the patches with data from within the resource



#if BUILD_STANDALONE_EXECUTABLE == 1
	options.commonToAllUsers = false;
	options.filenameSuffix = ".settings";
	options.storageFormat = PropertiesFile::StorageFormat::storeAsXML;
	options.applicationName = "MotasEdit";
	//options.folderName = "";
	options.osxLibrarySubFolder = "Application Support";
	options.ignoreCaseOfKeyNames = true;
	options.doNotSave = false;
	options.millisecondsBeforeSaving = 1000;
	appProperties.setStorageParameters(options);

	DBG("Properties file: " + options.getDefaultFile().getFullPathName());


	properties = appProperties.getUserSettings();

	fileNameAndPath = options.getDefaultFile().getParentDirectory().getFullPathName()
		+ String(File::getSeparatorString())
			+ String("presetPatches.bpatch");
	DBG("fileNameAndPath: " + fileNameAndPath);
	loadPresets();
#else
	properties = &appProperties;
#endif


	constexpr bool sz = sizeof(tAllParams) == 4096;
	if (!sz)
	{
		DBG("ERROR! - Size of tAllParam: " + String( sizeof(tAllParams)));
	}


}

void ApplicationCommon::saveAppPropertyString(int propertyEnum, String value)
{
	switch (propertyEnum)
	{
		default:
			break;
		case APP_PROPERTY_DEFAULT_FILE_LOCATION:
			properties->setValue("defaultPath", value);
			break;
		case APP_PROPERTY_BG_COLOUR:
			properties->setValue("bgColour", value);
			break;

	}
}

void ApplicationCommon::saveAppPropertyInteger(int propertyEnum, int value)
{

	bool notifyMIDI = false;
	switch (propertyEnum)
	{
		default:
			break;
		case APP_PROPERTY_GUI_SIZE_X:
			properties->setValue("windowWidth", value);
			break;
		case APP_PROPERTY_GUI_SIZE_Y:
			properties->setValue("windowHeight", value);
			break;
		case APP_PROPERTY_PLUGIN_GUI_SIZE:
			properties->setValue("pluginGUIsize", value);
			break;
		case APP_PROPERTY_MIDI_SEND_RATE:
			properties->setValue("MIDIsendRate", value);
			notifyMIDI = true;
			break;
//		case APP_PROPERTY_MIDI_UPDATE_INTERVAL:
//			properties->setValue("MIDIupdateInterval", value);
//			notifyMIDI = true;
//			break;
		case APP_PROPERTY_SCREENSHOT_COLOUR:
			properties->setValue("screenShotColour", value);
			break;
		case APP_PROPERTY_PATCH_PRESET_NUMBER:
			properties->setValue("presetPatchNumber", value);
			break;
		case APP_PROPERTY_MIDI_BASIC_CHANNEL:
			properties->setValue("midiBasicChannel", value);
			break;
		case APP_PROPERTY_MIDI_ROUTING_BITFIELD:
			properties->setValue("midiRoutingBitfield", value);
			notifyMIDI = true;
			break;


	}

	if (notifyMIDI)
	{
		midiProcessor->updateMIDISettings();
	}

}

void ApplicationCommon::saveAppPropertyBool(int propertyEnum, bool value)
{
	bool notifyMIDI = false;
	switch (propertyEnum)
	{
		default:
			break;
		case APP_PROPERTY_HIDE_NON_MOTAS_DEVICES:
		    properties->setValue("hideNonMotas", value);
		    notifyMIDI = true;
			break;
		case APP_PROPERTY_SHOW_TOOLTIPS:
			properties->setValue("toolTips", value);
			break;
		case APP_PROPERTY_PASS_SYSEX:
			properties->setValue("forwardSysExtoDAW", value);
			notifyMIDI = true;
			break;
		case APP_PROPERTY_MIDI_THRU_ENABLED:
			properties->setValue("thruEnabled", value);
			notifyMIDI = true;
			break;
		case APP_PROPERTY_AUTO_CONNECT_MOTAS:
			properties->setValue("autoConnectMotas", value);
			notifyMIDI = true;
			break;
		case APP_PROPERTY_LIVE_SCREENSHOT:
			properties->setValue("liveScreenshot", value);
			notifyMIDI = true;
			break;
		case APP_PROPERTY_ENABLE_AUTO_PAGE_CHANGE:
			properties->setValue("autoPageChange", value);
			notifyMIDI = true;
			break;
		case APP_PROPERTY_ENABLE_SHOW_MONITOR_ONLY:
			properties->setValue("showOnlyMonitor", value);
			notifyMIDI = true;
			break;


	}

	if (notifyMIDI)
	{
		midiProcessor->updateMIDISettings();
	}

}


String ApplicationCommon::loadAppPropertyString(int propertyEnum)
{
	String retVal = "";
	switch (propertyEnum)
	{
		default:
			break;
		case APP_PROPERTY_DEFAULT_FILE_LOCATION:
			retVal = properties->getValue("defaultPath", "");
			break;
		case APP_PROPERTY_BG_COLOUR:
			retVal = properties->getValue("bgColour", "dddddd");
			break;

	}
	return retVal;

}

int ApplicationCommon::loadAppPropertyInteger(int propertyEnum)
{
	int retVal = 0;
	switch (propertyEnum)
	{
		default:
			break;
		case APP_PROPERTY_GUI_SIZE_X:
			retVal = properties->getIntValue("windowWidth", 800);
			break;
		case APP_PROPERTY_GUI_SIZE_Y:
			retVal = properties->getIntValue("windowHeight", 600);
			break;
		case APP_PROPERTY_MIDI_SEND_RATE:
			retVal = properties->getIntValue("MIDIsendRate", 3);
			break;
	//	case APP_PROPERTY_MIDI_UPDATE_INTERVAL:
	//		retVal = properties->getIntValue("MIDIupdateInterval", 1);
			break;
		case APP_PROPERTY_PLUGIN_GUI_SIZE:
			retVal = properties->getIntValue("pluginGUIsize", 3);
			break;
		case APP_PROPERTY_SCREENSHOT_COLOUR:
			retVal = properties->getIntValue("screenShotColour", 0);
			break;
		case APP_PROPERTY_PATCH_PRESET_NUMBER:
			retVal = properties->getIntValue("presetPatchNumber", 0);
			break;
		case APP_PROPERTY_MIDI_BASIC_CHANNEL:
			retVal = properties->getIntValue("midiBasicChannel", 1);
			break;
		case APP_PROPERTY_MIDI_ROUTING_BITFIELD:
			retVal = properties->getIntValue("midiRoutingBitfield", 0x07); // default turn on only MotasEdit data to Motas
			break;

	}
	return retVal;

}



bool ApplicationCommon::loadAppPropertyBool(int propertyEnum)
{
	bool retVal = false;
	switch (propertyEnum)
	{
		default:
			break;
		case APP_PROPERTY_HIDE_NON_MOTAS_DEVICES:
			retVal = properties->getBoolValue("hideNonMotas", true);
			break;
		case APP_PROPERTY_SHOW_TOOLTIPS:
			retVal = properties->getBoolValue("toolTips", true);
			break;
		case APP_PROPERTY_PASS_SYSEX:
			retVal = properties->getBoolValue("forwardSysExtoDAW", false);
			break;
		case APP_PROPERTY_MIDI_THRU_ENABLED:
			retVal = properties->getBoolValue("thruEnabled", false);
			break;
		case APP_PROPERTY_AUTO_CONNECT_MOTAS:
			retVal = properties->getBoolValue("autoConnectMotas", true);
			break;
		case APP_PROPERTY_LIVE_SCREENSHOT:
			retVal = properties->getBoolValue("liveScreenshot", false);
			break;
		case APP_PROPERTY_ENABLE_AUTO_PAGE_CHANGE:
			retVal = properties->getBoolValue("autoPageChange", false);
			break;
		case APP_PROPERTY_ENABLE_SHOW_MONITOR_ONLY:
			retVal = properties->getBoolValue("showOnlyMonitor", false);
			break;



	}
	return retVal;
}

void ApplicationCommon::saveSettings()
{

#if BUILD_STANDALONE_EXECUTABLE == 1
	appProperties.saveIfNeeded();
#endif

}


#if BUILD_STANDALONE_EXECUTABLE == 0

MemoryBlock& ApplicationCommon::getPluginStateBinary()
{

	memoryBlock.replaceWith((void*) &this->activePatch, sizeof(activePatch));

	return memoryBlock;
}

std::unique_ptr<XmlElement> ApplicationCommon::getPluginSettingsXML()
{

	// the name given will wrap the entire xml data.
	std::unique_ptr<XmlElement> xml =  properties->createXml("MotasEditPluginSettings");


	// write the binary patch data into a memory block

	for (int i = 0; i < BANK_OF_PRESETS_LENGTH; i++)
	{

		DBG("xml create..");

		XmlElement* e = xml->createNewChildElement ("PATCH_" + String(i));

		e->setAttribute("name", "patch_" + String(i));


		MemoryBlock m;
		m.replaceWith( patchPresetsArray[i], PATCH_LENGTH_BYTES_UNPACKED);

		// 64-bit encode the binary data to a string
		e->setAttribute("binaryDataEncoded",  m.toBase64Encoding());



		//->addTextElement (m.toBase64Encoding());
	}
	//xml->addChildElement(e);

	return xml;
}

void ApplicationCommon::restorePluginState(XmlElement* xml)
{
	properties->restoreFromXml(*xml);

	if (xml->hasTagName ("MotasEditPluginSettings"))
	{
		for (int i = 0; i < BANK_OF_PRESETS_LENGTH; i++)
		{
			// now we'll iterate its sub-elements looking for elements..
			forEachXmlChildElement (*xml, e)
			{
		//		DBG("tag name: " + e->getTagName());
				if (e->hasTagName ("PATCH_" + String(i)))
				{
					String patch =  e->getStringAttribute("binaryDataEncoded");
					if (!patch.isEmpty())
					{
						MemoryBlock m;
						m.fromBase64Encoding(patch);

						if (m.getSize() ==  PATCH_LENGTH_BYTES_UNPACKED)
						{
							DBG("Copy patch binary to active patch #" + String(i));
							memcpy(patchPresetsArray[i], m.getData(), PATCH_LENGTH_BYTES_UNPACKED);

							// this is in case old version of MotasPlugin has saved patches in old format
							PatchPatternSequence::convertPatchToLatestVersion((tAllParams*) patchPresetsArray[i]);


							// update the Edit patch array too
							memcpy(patchPresetsEditedArray[i],patchPresetsArray[i], PATCH_LENGTH_BYTES_UNPACKED);
							aPatchHasChanged = true;
							originalMatchesEditedPatch = true;
						}
				//		DBG("Found activePatch data");
						break;
					}
				}
			}
		}
	}
	setPatchNumber(loadAppPropertyInteger(APP_PROPERTY_PATCH_PRESET_NUMBER));
}



#endif

void ApplicationCommon::setMIDIProcessor(MIDIProcessor* m)
{
	this->midiProcessor = m;
}

ApplicationCommon::~ApplicationCommon()
{

	//jassert(1 == 0);


	for (int i = 0; i < patchPresetsArray.size(); i++)
	{
		delete[] patchPresetsArray[i];
	}
	for (int i = 0; i < patchPresetsEditedArray.size(); i++)
	{
		delete[] patchPresetsEditedArray[i];
	}
	this->patchPresetsEditedArray.clear();
	this->patchPresetsArray.clear();
}



int ApplicationCommon::getPatchNumber()
{
	return activePatchNumber;
}

void ApplicationCommon::setPatchNumber(int newPatchNum)
{
	if (newPatchNum >= 0 && newPatchNum < BANK_OF_PRESETS_LENGTH)
	{
		activePatchNumber = newPatchNum;
		// point to the EDITED array at all times
		activePatch = patchPresetsEditedArray[activePatchNumber];
		setGUIPatch();

		saveAppPropertyInteger(APP_PROPERTY_PATCH_PRESET_NUMBER, activePatchNumber);
	}
}




void ApplicationCommon::changePatchName(int patchNum, const uint8_t* newNameBuffer)
{
	if (patchNum >= 0 && patchNum < BANK_OF_PRESETS_LENGTH)
	{

		// change the name of the edited patch and the original

		Utilities::setName(newNameBuffer, patchPresetsArray[patchNum]);
		Utilities::setName(newNameBuffer, patchPresetsEditedArray[patchNum]);


		crc.updatePatchCRC((tAllParams* ) patchPresetsArray[patchNum]);
		crc.updatePatchCRC((tAllParams* ) patchPresetsEditedArray[patchNum]);
		aPatchHasChanged = true;

	}

}

void ApplicationCommon::changePatchName(int patchNum, const String& newName)
{
	if (patchNum >= 0 && patchNum < BANK_OF_PRESETS_LENGTH)
	{

		// change the name of the edited patch and the original

		Utilities::setName(newName, patchPresetsArray[patchNum]);
		Utilities::setName(newName, patchPresetsEditedArray[patchNum]);


		crc.updatePatchCRC((tAllParams* ) patchPresetsArray[patchNum]);
		crc.updatePatchCRC((tAllParams* ) patchPresetsEditedArray[patchNum]);
		aPatchHasChanged = true;

	}
}





bool ApplicationCommon::isPatchValid(uint8_t* patch)
{
	uint32_t CRC32  = Utilities::read32(4, patch);
	uint32_t crcVal = crcCalculator::crcFastSTMF4(patch + 8, PATCH_LENGTH_BYTES_UNPACKED - 8); // calculate CRC of all data (excluding first 8 bytes, and final 4)

	uint32_t identifierType = PATCH_IDENTIFIER;
	uint32_t identifier  = Utilities::read32(0, patch);

	if ((identifier == identifierType) && (crcVal == CRC32))
	{
		return true;
	} else
	{
		return false;
	}
}

void ApplicationCommon::updatePatch(int index, uint8_t* buf)
{
	if (index < BANK_OF_PATCHES_LENGTH && index >= 0 )
	{
		if (isPatchValid(buf))
		{
			memcpy(patchPresetsEditedArray[index], buf,  PATCH_LENGTH_BYTES_UNPACKED);
			memcpy(patchPresetsArray[index], buf,  PATCH_LENGTH_BYTES_UNPACKED);
			aPatchHasChanged = true;
			originalMatchesEditedPatch = true;
			DBG("Copied patch");
		} else
		{
			DBG("Copying patch FAILED");
		}
	}

}

void ApplicationCommon::storeEditedPatch(int index)
{
	if (index < BANK_OF_PATCHES_LENGTH && index >= 0 )
	{
		memcpy(patchPresetsArray[index], patchPresetsEditedArray[index],  PATCH_LENGTH_BYTES_UNPACKED);
		aPatchHasChanged = true;
		originalMatchesEditedPatch = true;
	}
}

void ApplicationCommon::restoreEditedPatch(int index)
{
	if (index < BANK_OF_PATCHES_LENGTH && index >= 0 )
	{
		memcpy(patchPresetsEditedArray[index], patchPresetsArray[index],  PATCH_LENGTH_BYTES_UNPACKED);
		aPatchHasChanged = true;
		originalMatchesEditedPatch = true;
	}

}

bool ApplicationCommon::isFirstTimeStartedGui()
{
	return firstTimeStartup;
}

uint8_t* ApplicationCommon::getActivePatch()
{
	return this->activePatch;
}

tAllParams* ApplicationCommon::getGUIPatch()
{
	return &this->guiPatch;
}

tAllParams* ApplicationCommon::getOriginalPatch()
{
	return  (tAllParams*) patchPresetsArray[activePatchNumber];
}

void ApplicationCommon::setGUIPatch()
{
	memcpy(&guiPatch, getActivePatch(), PATCH_LENGTH_BYTES_UNPACKED);
	originalMatchesEditedPatch = (0 != memcmp(getOriginalPatch(), getGUIPatch(), PATCH_LENGTH_BYTES_UNPACKED));
}

void ApplicationCommon::updateCurrentPatch(uint8_t* buffer)
{
	updatePatch(getPatchNumber(), buffer);
}

void ApplicationCommon::changeActivePatchName(uint8_t* newNameBuf)
{

	this->changePatchName(activePatchNumber, newNameBuf);
}


String ApplicationCommon::getCurrentPatchName()
{
	return getPatchName(activePatchNumber);
}

void ApplicationCommon::changeCurrentPatchName(const String& newName)
{
	changePatchName(activePatchNumber, newName);
}


#if BUILD_STANDALONE_EXECUTABLE == 1
bool ApplicationCommon::savePresets()
{

	File f(fileNameAndPath);

	DBG("Save, filename: " + f.getFileName());
	f.deleteFile();
	FileOutputStream fileStream(f);

	if (!fileStream.openedOk())
	{
		AlertWindow::showMessageBox	(AlertWindow::WarningIcon , "Error", "Problem opening file", "OK", nullptr);
		return 0;
	}

	for (int i = 0; i < BANK_OF_PATCHES_LENGTH; i++)
	{
		fileStream.write(patchPresetsArray[i], PATCH_LENGTH_BYTES_UNPACKED);
	}
	return 1;
}

bool ApplicationCommon::loadPresets()
{

	File f(fileNameAndPath);
	int size = (int) f.getSize();
	DBG("File size " + String(size));


	if (f.exists())
	{
		if (size != PATCH_LENGTH_BYTES_UNPACKED * BANK_OF_PATCHES_LENGTH)
		{
			AlertWindow::showMessageBox	(AlertWindow::WarningIcon , "Error", "Patch bank file size error", "OK", nullptr);
			return 0;
		} else
		{
			FileInputStream fileStream(f);
			if (fileStream.openedOk())
			{
				for (int i = 0; i < BANK_OF_PATCHES_LENGTH; i++)
				{
					fileStream.read(patchPresetsArray[i], PATCH_LENGTH_BYTES_UNPACKED);
					PatchPatternSequence::convertPatchToLatestVersion((tAllParams*) patchPresetsArray[i]);
					restoreEditedPatch(i);
				}
			} else
			{
				return 0;
			}
		}
		return 1;
	} else
	{
		return 0;
	}


}

#endif



String ApplicationCommon::getPatchName(uint8_t* patch)
{
	if (isPatchValid(patch))
	{
		return Utilities::getName(patch);
	} else
	{
		return String (" ");
	}
}

String ApplicationCommon::getPatchName(int index)
{
	if (index >= 0 && index < BANK_OF_PATCHES_LENGTH)
	{
		return getPatchName(patchPresetsArray[index]);
	} else
		return "?";
}

bool ApplicationCommon::isActivePatchEdited()
{


	if (aPatchHasChanged || (prevMatchState != originalMatchesEditedPatch))
	{
		prevMatchState = originalMatchesEditedPatch;
		aPatchHasChanged = false;
		return true;
	}
	else
		return false;
}
