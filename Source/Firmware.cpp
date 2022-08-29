

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

    Firmware.cpp

  ==============================================================================
*/
#include "../binaryResources/MotasEditResources.h"
#include "./Headers/Firmware.h"
#include "./Headers/HoldingWindowComponent.h"
#include "./Headers/MIDIComms.h"
#include "Headers/MIDIProcessor.h"


Firmware::Firmware(HoldingWindowComponent* hw, LookAndFeel* l)
: holdingWindow(hw), laf(l)
{


	firmwareVersionReported = 0;

	version = ProjectInfo::versionString;
	softwareInfoText = "MotasEdit - software for controlling Motas synthesizers\n\n" +

#if BUILD_STANDALONE_EXECUTABLE == 0
			String("THIS PLUGIN DOES NO AUDIO PROCESSING\n") +
			String("It normally uses a direct USB MIDI connection to the Motas-6 synthesizer\n") +
			String("It can also sends MIDI data (including NRPNs and SysEx) direct to the DAW\n\n") +
#endif

			String("This version designed for Motas-6 firmware version v0602011C.\nIf you are using an older version please update your Motas-6.\n") +
			String("\nSoftware version " + version + "\n") +
			String("Copyright Motas Electronics Limited 2022\n") +
			String("www.motas-synth.uk\n");


	infoEditLabel.setReadOnly(true);
	infoEditLabel.setMultiLine(true, true);
	infoEditLabel.setLookAndFeel(laf);



	updateFirmwareButton.setLookAndFeel(laf);
	requestBulkButton.setLookAndFeel(laf);
	requestSetupButton.setLookAndFeel(laf);
	sendBulkButton.setLookAndFeel(laf);
	sendSetupButton.setLookAndFeel(laf);


	updateFirmwareButton.setButtonText("Update firmware");
	requestBulkButton.setButtonText("Request bulk data");
	requestSetupButton.setButtonText("Request setup data");
	sendBulkButton.setButtonText("Send bulk data");
	sendSetupButton.setButtonText("Send setup data");


	updateFirmwareButton.setTooltip("Press to load a new firmware binary file and send to Motas.");
	requestBulkButton.setTooltip("Press to request entire bulk data from Motas to save to file. \n VERY large transfer as contains "
			"all patches, pattern, sequences etc. \nUse to back-up all Motas data in one go.");
	sendBulkButton.setTooltip("Press to load bulk entire memory file and send to Motas.\n!!WARNING will overwrite ALL stored data on Motas!!");
	requestSetupButton.setTooltip("Press to request global settings data from Motas to save to file.");
	sendSetupButton.setTooltip("Press to load  global settings data and send to Motas");



	updateFirmwareButton.addListener(this);
	requestBulkButton.addListener(this);
	requestSetupButton.addListener(this);
	sendBulkButton.addListener(this);
	sendSetupButton.addListener(this);

	addAndMakeVisible(updateFirmwareButton);
	addAndMakeVisible(requestBulkButton);
	addAndMakeVisible(requestSetupButton);
	addAndMakeVisible(sendBulkButton);
	addAndMakeVisible(sendSetupButton);

/*
	std::unique_ptr<Drawable>  image =  Drawable::createFromImageData(MotasEditResources::MotasElectronicsLogo_svg,MotasEditResources::MotasElectronicsLogo_svgSize);

		if (image == nullptr)
		{
			DrawableText* t = new DrawableText();
			t->setText("Image N/A!");
			image = t;
		}
		motasElectronicsLogoButton = new DrawableButton("motasElectronicsLogo", DrawableButton::ImageFitted);
		motasElectronicsLogoButton->setImages(image, nullptr, nullptr, nullptr, nullptr, nullptr , nullptr, nullptr);
		motasElectronicsLogoButton->toBack();
		addAndMakeVisible(motasElectronicsLogoButton);
*/

	updateInfo(nullptr);
	//infoLabel.setJustificationType(Justification::left);

	getMotasInfoButton.setButtonText("Get Motas info");
	getMotasInfoButton.setTooltip("Press to request info from Motas");
	addAndMakeVisible(getMotasInfoButton);
	getMotasInfoButton.setLookAndFeel(laf);
	addAndMakeVisible(infoEditLabel);

	//infoLabel.setEditable(true, false, true);
	//infoLabel.setEnabled(false);

	getMotasInfoButton.addListener(this);

}

void Firmware::resized()
{

	auto r = getLocalBounds().reduced(10);

	float h = (float) r.getHeight();
	float w = (float) r.getWidth();



	const int buttonWidth = (int)  (w/4);
	const int buttonHeight = (int) (h/12);

	//const int areaLeft = 0;
	//const int areaRight = buttonWidth*3;
//	const int listBoxWidth = buttonWidth * 3;


	updateFirmwareButton.setSize(buttonWidth, buttonHeight);
	requestBulkButton.setSize(buttonWidth, buttonHeight);
	requestSetupButton.setSize(buttonWidth, buttonHeight);
	sendBulkButton.setSize(buttonWidth, buttonHeight);
	sendSetupButton.setSize(buttonWidth, buttonHeight);

	updateFirmwareButton.setTopLeftPosition(10, 10);
	requestBulkButton.setTopLeftPosition(10, 10 + buttonHeight*2);
	requestSetupButton.setTopLeftPosition(10 + buttonWidth, 10 + buttonHeight*2);
	sendBulkButton.setTopLeftPosition(10, 10 + buttonHeight*3);
	sendSetupButton.setTopLeftPosition(10 + buttonWidth, 10 + buttonHeight*3);



	r.removeFromTop(buttonHeight*4);


		//motasElectronicsLogoButton->setBounds(r);
		//auto r2 = r.removeFromTop(50);
//	motasElectronicsLogoButton->setSize(250, 70);
//	motasElectronicsLogoButton->setTopRightPosition(r.getTopRight().x, r.getTopRight().y);
	//r.removeFromTop(50);
	getMotasInfoButton.setSize(buttonWidth, buttonHeight);
	r.removeFromTop(buttonHeight);
	getMotasInfoButton.setTopLeftPosition(r.getTopLeft());
	//getMotasInfoButton.setSize(300, 30);
	r.removeFromTop(buttonHeight*2);
	infoEditLabel.setBounds(r);



}

Firmware::~Firmware() {
	//this->setLookAndFeel(nullptr);
	DBG("Firmware destructor");
}


void Firmware::updateInfo(uint8_t* buffer)
{

	String t = softwareInfoText;
	if (buffer != nullptr)
	{
		t += "\nSystem identifier (3 words):\n";
		tSystemInfo systemInfo;
		memcpy(&systemInfo, buffer, 256);

		String ser = String::toHexString(systemInfo.data.ID1);

		while (ser.length() < 8)
			ser = "0" + ser;
		ser += "-" + String::toHexString(systemInfo.data.ID2);
		ser += "-" +String::toHexString(systemInfo.data.ID3);

		t += ser.toUpperCase();

		if (systemInfo.data.versionValueExists != SYSTEM_INFO_VERSION_EXISTS_IDENTIFIER)
		{
			t += String("\nFirmware version : unknown");
		} else
		{

			String firmwareVersion = String::toHexString(systemInfo.data.firmwareVersion).toUpperCase();


			while (firmwareVersion.length() < 8)
				firmwareVersion = "0"  + firmwareVersion;
			t += "\nFirmware version : " + firmwareVersion;


			t += "\nFirmware build date : " + String::fromUTF8(systemInfo.data.firmwareBuildDate,11);
			version = String::toHexString(systemInfo.data.UIPCBVersion).toUpperCase();
			while (version.length() < 4)
					version = "0"  + version;
			t += "\nUI PCB version : " + version;


			if (systemInfo.data.versionNumber >= 2)
			{
				String name = String::fromUTF8(systemInfo.data.currentPatchName, LENGTH_OF_PATCH_NAME);
				t += "\nActive patch name: " + name;

			}

		}





		t += "\nPower cycles : " +String(systemInfo.data.powerCycles);
		if ((systemInfo.data.firmwareVersion & 0xFFFF) < MINIMUM_FIRMWARE_VERSION
						|| systemInfo.data.versionValueExists != SYSTEM_INFO_VERSION_EXISTS_IDENTIFIER) // or hardware version minor  is zero
		{
			t +=  "\n\nThe connected Motas synthesizer has an older firmware version.\nFor the best experience please update your Motas now by downloading"
					" new firmware from www.motas-synth.uk\nand applying with the 'Update firmware' button above. \n";
		}


/*
		t+= "\nCalibration results:";
		for (int j = 0; j < NUMBER_OF_OSCILLATORS; j++)
		{
			t += "\nosc" + String(j) + ": ";
			for (int i = 0; i < NUMBER_OF_COMPUTUNE_POINTS; i++)
			{
				t += " " + String(systemInfo.data.calibValuesOscillator[j][i]);
			}
		}
		for (int j = 0; j < NUMBER_OF_FILTERS; j++)
		{
			t += "\nfilter" + String(j) + ": ";
			for (int i = 0; i < NUMBER_OF_COMPUTUNE_POINTS; i++)
			{
				t += " " + String(systemInfo.data.calibValuesFilter[j][i]);
			}
		}


		t += "\nNoise: " + String(systemInfo.data.calibNoiseLevel);
		t += "\nPWM1: " + String(systemInfo.data.VCO1PWMCentre);
		t += "\nPWM3: " + String(systemInfo.data.VCO3PWMCentre);

		 t += "\nFilterRes1: " + String(systemInfo.data.calibLpf1Resonance24Level);
		 t += "\nFilterRes2: " + String(systemInfo.data.calibLpf2ResonanceLevel);
		 */
	} else
	{
		//DBG("motas info buffer null ptr");
	}



	 infoEditLabel.setText(t, NotificationType::dontSendNotification);



	//ui->labelSystemInfo->setText(t);



}

void Firmware::buttonClicked(Button* b)
{
#if BUILD_STANDALONE_EXECUTABLE == 0
	MIDIProcessor* mp = holdingWindow->midiProcessor;
#else
	MIDIProcessor* mp = holdingWindow->midiProcessor.get();
#endif

	if (mp->isBusySendingGuiCommand)
	{
#if JUCE_MODAL_LOOPS_PERMITTED
		 AlertWindow::showMessageBox	(AlertWindow::WarningIcon , "Sorry", "Already busy with another MIDI request", "OK", nullptr);
		 DBG("Sorry, busy sending previous data");
#endif
	}
	else
	{
		int type = -1;
		if (b == &updateFirmwareButton)
			type = 0;
		else if (b == &sendBulkButton)
			type = 1;
		else if (b == &sendSetupButton)
			type = 2;
		else if (b == &requestBulkButton)
		{
			mp->commandRequest(MIDIProcessor::COMMAND_REQUEST_GET_BULK, nullptr, 0);
		} else if (b == &requestSetupButton)
		{
			mp->commandRequest(MIDIProcessor::COMMAND_REQUEST_GET_GLOBALS, nullptr, 0);
		} else if (b == &getMotasInfoButton)
		{
			infoEditLabel.clear();
			infoEditLabel.setText(softwareInfoText, NotificationType::dontSendNotification);
			mp->commandRequest(MIDIProcessor::COMMAND_REQUEST_GET_INFO, 0);
		}

		if (type >= 0)
		{

			std::unique_ptr<WildcardFileFilter> wildcardFilter;// ("*.*", String(), "Foo files");

			String summaryText;
			String text;
			int mode;
			String ext;

			int32_t expectedFileLength;
			switch (type)
			{
				default:
				case 0:
					expectedFileLength = 0;
					summaryText = "Load firmware";
					text = "Please choose the firmware file... (*.mbin)";
					wildcardFilter = std::make_unique<WildcardFileFilter>("*.mbin", String(), "Motas firmware file");
					ext = "/*.mbin";
					break;
				case 1:
					expectedFileLength = MASS_FLASH_TRANSFER_LENGTH_BYTES;
					summaryText = "Load bulk data";
					text = "Please choose the bulk data file... (*.dat)";
					wildcardFilter = std::make_unique<WildcardFileFilter>("*.dat", String(), "Motas bulk data file");
					ext = "/*.dat";
					break;
				case 2:
					expectedFileLength = SETUP_DATA_LENGTH_BYTES;
					summaryText = "Load setup data";
					text = "Please choose the setup data file... (*.setup)";
					wildcardFilter = std::make_unique<WildcardFileFilter>("*.setup", String(), "Motas setup data file");
					ext = "/*.setup";
					break;
			}

			mode = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;
			FileBrowserComponent browser (mode,	File(holdingWindow->getDefaultPath() + ext),	  wildcardFilter.get(),  nullptr);
			FileChooserDialogBox dialogBox (summaryText,	text,browser,	true,Colours::darkgrey);
			if (dialogBox.show())
			{
				// if file chosen
				File f = browser.getSelectedFile (0);
				int32_t fileLength =  (int32_t) f.getSize();
				bool fileLengthOk = true;

				if (expectedFileLength != 0)
				{
					if (fileLength != expectedFileLength)
						fileLengthOk = false;
				}
				else if (fileLength > (1024*1024) || (fileLength < 200))
					fileLengthOk = false;
				if (!fileLengthOk)
				{
	#if JUCE_MODAL_LOOPS_PERMITTED
					 AlertWindow::showMessageBox	(AlertWindow::WarningIcon , "Error", "File size error", "OK", nullptr);
					 DBG("Filesize: " + String(fileLength) + " expected: " + String(expectedFileLength));
	#endif
					 return;
				}


				//holdingWindow->updatePath(f.getFullPathName());
				holdingWindow->updatePath(f.getParentDirectory().getFullPathName());

				uint8_t* buffer = new uint8_t[fileLength]; // CANNOT use scopedPointer here!
				FileInputStream fileStream(f);
				if (fileStream.openedOk())
				{
					//DBG("Sending new firmware, filesize: " + String(fileLength));
					fileStream.read(buffer, fileLength);
					bool ok = true;
					switch (type)
					{
						case 0:
						{
							// decode the firmware version from the file
							String msg = "Current Motas firmware version is newer.\nFile version: ";
							uint32_t firmwareVersionFromFile;
							memcpy(&firmwareVersionFromFile, &buffer[10], sizeof (firmwareVersionFromFile));
							uint16_t fw = firmwareVersionReported & 0xFFFF;
							uint16_t fwFile = firmwareVersionFromFile & 0xFFFF;
							if (fw == 0 || fw > fwFile)
							{
								msg += String::toHexString(fwFile).toUpperCase();

								if (fw > 0)
									msg += "\nMotas version: " + String::toHexString(fw).toUpperCase();
								else
									msg += "\nMotas version: UNKNOWN";

								msg += "\n***** WARNING *****\nUpdating to older firmware may cause loss of Motas functionality, or "
										"loss of data.";
								msg += "\nFor example, buttons may not work properly anymore or saved patches may corrupt!";
								bool accepted = false;
								#if JUCE_MODAL_LOOPS_PERMITTED
								accepted = AlertWindow::showOkCancelBox	(AlertWindow::WarningIcon ,
												 "Warning", msg, "Continue","Cancel",  nullptr);
								#endif
								ok = false;
								if (accepted) // if ok selected
								{
									ok = true;
								}
							}



							if (ok && !mp->commandRequest(MIDIProcessor::COMMAND_REQUEST_SEND_FIRMARE, buffer, fileLength))
							{
								#if JUCE_MODAL_LOOPS_PERMITTED
									AlertWindow::showMessageBox	(AlertWindow::WarningIcon , "Error", "Error in firmware data, it appears to be corrupted or incorrect", "OK", nullptr);
								#endif
							}
							break;
						}
						case 1:
						{
							 mp->commandRequest(MIDIProcessor::COMMAND_REQUEST_SEND_BULK, buffer, fileLength);
						}
						break;
						case 2:
						{
							mp->commandRequest(MIDIProcessor::COMMAND_REQUEST_SEND_GLOBALS, buffer, fileLength);
							break;
						}
					}
				}
				delete [] buffer;
			}
		}
	}
}

void Firmware::updateFirmwareVersionReported(uint8_t* buffer)
{



	tSysexStatus* status = (tSysexStatus*) buffer;
/*
 * String t = infoEditLabel.getText();
	String firmwareVersion = String::toHexString(status->data.firmwareVersion).toUpperCase();
	t += String("\nFirmware version : " + firmwareVersion);


	infoEditLabel.setText(t, NotificationType::dontSendNotification);
*/
	firmwareVersionReported = status->data.firmwareVersion;


}
