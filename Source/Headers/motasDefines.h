

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


#ifndef SOURCE_MOTAS_DEFINES_H_
#define SOURCE_MOTAS_DEFINES_H_

#define NAME_LENGTH 16

#ifdef _MSC_VER
#  define PACKED_STRUCT() \
    __pragma(pack(push, 1)) struct  __pragma(pack(pop))
#elif defined(__GNUC__)
#  define PACKED_STRUCT() struct __attribute__((packed))
#endif


#define MIN_VALUE_GLOBAL_LFO 0//-33
#define MIN_VALUE_LOCAL_LFO 0

#define LFO2_EDIT_OUTPUT_MODE_MAX_VALUE  2
#define LFO2_EDIT_OUTPUT_MODE_MIN_VALUE  0
#define LFO2_EDIT_TRIGGER_MAX_VALUE  2
#define LFO2_EDIT_TRIGGER_MIN_VALUE  0
#define LFO2_EDIT_PITCH_TRACK_MAX_VALUE  3
#define LFO2_EDIT_PITCH_TRACK_MIN_VALUE  0
#define LFO2_EDIT_SINGLE_SHOT_MAX_VALUE  1
#define LFO2_EDIT_SINGLE_SHOT_MIN_VALUE  0

#define LFO2_EDIT_LFOX_FREQ_MODE_MAX_VALUE  4095
#define LFO2_EDIT_LFOX_FREQ_MODE_MIN_VALUE  -4095
#define LFO2_EDIT_EG_FREQ_MODE_MAX_VALUE  4095
#define LFO2_EDIT_EG_FREQ_MODE_MIN_VALUE  -4095




#define EG_EDIT_SOURCE_MAX_VALUE  4
#define EG_EDIT_SOURCE_MIN_VALUE  0

#define EG_EDIT_LOOP_MAX_VALUE  1
#define EG_EDIT_LOOP_MIN_VALUE  0

#define EG_EDIT_RESTART_MAX_VALUE  1
#define EG_EDIT_RESTART_MIN_VALUE  0

#define EG_EDIT_SHAPE_MAX_VALUE  64
#define EG_EDIT_SHAPE_MIN_VALUE  -64

#define EG_EDIT_UNIPOLAR_MAX_VALUE  1
#define EG_EDIT_UNIPOLAR_MIN_VALUE  0

#define EG_EDIT_KEYTRACK_MAX_VALUE  7
#define EG_EDIT_KEYTRACK_MIN_VALUE  0



const uint8_t LENGTH_OF_PATCH_NAME = 16;
const uint8_t NUM_GLOBAL_LFOS = 4;
const uint8_t NUM_GLOBAL_EGS = 4;
const uint8_t PARAM_END = 33;//  was 36;

/*
 *
 *
 * Really handy typedef to check for sizes of structures during compile time!
 */
#define SIZEOF_STATIC_ASSERT(test) typedef char assertion_on_mystruct[( !!(test) )*2-1 ]



enum
{
	T_ALL_PARAMS_VERSION_1  = 1,
	T_ALL_PARAMS_VERSION_2, // version 2 added microtune number storage to patches
	T_ALL_PARAMS_VERSION_3, // version 3 EG shape now variable from -64 to +64 (WAS 0, 1, 2)
	T_ALL_PARAMS_VERSION_4, // added 4 custom LFOs
	T_ALL_PARAMS_VERSION_5, // added more modulation options
	T_ALL_PARAMS_VERSION_6, // added new EG trigger options
	T_ALL_PARAMS_VERSION_7, // added new advanced modulation options and changed EG attack, decay, release
	T_ALL_PARAMS_VERSION_8 // changed LFO freq when using clock sync
};


typedef PACKED_STRUCT()
{
	uint16_t  	delay;
	uint16_t 	sustain;
	int32_t 	attack;
	int32_t  	decay;
	int32_t  	release;
	uint8_t		egRetrig;
	int8_t 		egShape;
	uint8_t 	egRestartOnTrigger;
	//uint8_t 	egUnused; // for future expansion
	uint8_t 	egUnipolar; // set to zero for bipolar operation
} tEGCore;

/**
 * LFO1's which are shared across all params
 */
typedef  PACKED_STRUCT()
{
	uint8_t     trigger;
	uint8_t	    waveformType;
	int16_t 	freq;
	uint8_t 	timeSync;
	//uint8_t 	singleShot;
	struct
		{
			uint8_t 	singleShot : 1;
			uint8_t 	pitchTrack : 2;
			uint8_t 	unused : 5;
		} extras;
	uint16_t 	unusedCommonLFO1; // for future expansion
} tCommonLFO; // one of the 4 or so 'common ' LFOs for each patch

typedef  PACKED_STRUCT()
{
	//tCommonLFO* pLFO; // point to a common LFO
	uint32_t pLFO; // need to set this as a uint32 on PC platform as size could be 64bit

	uint8_t	    source; // choose commonLFO1, 2 etc
	uint8_t 	outputMode; // for future expansion
	int16_t 	modDepth;
} tLFOAmount; // refers to a common LFO - controls how much effect it has, and which one is routed



typedef  PACKED_STRUCT()
{
	//tEGCore* 	pEG; // point to actual EG
	uint32_t pEG; // need to set this as a uint32 on PC platform as size could be 64bit

	uint8_t	    source; // choose common EG1, 2 etc
	uint8_t 	unusedEGAmount; // for future expansion
	int16_t 	modDepth;
} tEGAmount;


/**
 * LFO2's which are fixed to each parameter page
 */
typedef  PACKED_STRUCT()
{
	uint8_t     trigger;
	uint8_t	    waveformType ;
	int16_t 	freq;
	int16_t 	modDepth;
	uint8_t     outputMode;
	struct
	{
		uint8_t 	singleShotLFO2 : 1;
		uint8_t 	pitchTrackLFO2 : 2;
		uint8_t 	unused : 5;
	} extras;
} tStorageLFO;

typedef  PACKED_STRUCT()
{
	uint8_t destination; // specify what this controller controls (when using the 'alternate' destination mod amount. Always destination is offset.
	//uint8_t unusedStorageMIDI; // for future expansion
	uint8_t unipolar;
	int16_t modDepth;
	int16_t modDepthAlternate;
} tStorageMIDI;

typedef PACKED_STRUCT()
{
	struct
	{
		uint8_t array[11];
	} harmonics;
	struct
	{
		uint8_t harmonicControl: 1;
		uint8_t unused: 7;
	} control;

} tStorageDigitalOsc;




typedef union
{
	uint8_t value;
	struct
	{
		uint8_t async:1;
		uint8_t antialias:1;
		uint8_t invert:1;
		uint8_t modulation:2;
		uint8_t unused:3;
	} digitalOptions;
} option2DigitalOptions_t;

typedef union
{
	uint16_t	value;
	struct
	{
		int16_t coarseTune: 6;
		int16_t fineTune : 10;
	} noName;

} t_option3;



typedef PACKED_STRUCT()
{
	/*
	 *
	 *
	 * For Visual Studio it seems cannot have a uin16_t on a boundary across uint8_t
	 * so, we cannot have:
	 * uint8_t
	 * uint16_t
	 * uint8_t
	 * uint8_t
	 * uint8_t
	 *
	 * But we CAN have:
	 * uint8_t
	 * uint8_t
	 * uint16_t
	 * uint8_t
	 * uint8_t
	 */
	/*
	 *
	 * This structure it stored in the patch, which defines the advanced modulation settings
	 *
	 */
	// 8 bytes max
	uint8_t source1Page:4;
	uint8_t source2Page:4;

	uint8_t source1Param;
	uint8_t source2Param;


	uint8_t unipolar :1;
	uint8_t algorithm:5; // the function are we going to employ
	uint8_t unusedBit:2;


	int16_t gain; // positive or negative. up to magnitude GAIN_MAX_VALUE


	uint8_t destinationPage;
	uint8_t destinationParam;


} t_modOfMod;



typedef  PACKED_STRUCT()
{
	int32_t 		basicOffset;
	tEGCore 		EG0;
	tEGAmount 		EG1;

	//tEGAmount 		EG2Unused;
	int32_t 		offsetIncludingPotUNUSED;

	int16_t 		potentiometerValueSaved; // for Motas 6, value of pot as saved to FLASH
	t_option3 		option3;


	tLFOAmount		LFO1;
	tStorageLFO 	LFO2;

	uint8_t			option1; //  e.g to set the VCF output node, or whether osc sync is on etc.
	option2DigitalOptions_t			option2;

	int16_t 		potentiometerValue; // for Motas 6 - current value of pot on panel
//	uint16_t 		unusedParam1; // for future expansion
//	uint16_t 		unusedParam2; // for future expansion

	// allow control of parameter via MIDI note, velocity and up to 2 other controllers
	tStorageMIDI 	MIDINote;
	tStorageMIDI	MIDIVelocity;
	tStorageMIDI 	MIDICon1; //   this is also the transmitted controller code -// n.b. can make MOD1, MOD2 button/dial generate these signals internally (and output data on MIDI)
	tStorageMIDI 	MIDICon2; //
	tStorageMIDI 	MIDICon3; //  this is also the transmitted controller code -// n.b. can make MOD1, MOD2 button/dial generate these signals internally (and output data on MIDI)
	tStorageMIDI 	MIDICon4; //





	union
		{
		 	 PACKED_STRUCT()
			{
				int16_t LFO2_freqModFromLFO1; // amount of modulation from active global LFO;
				int16_t LFO2_freqModFromEG; // amount of modulation from active EG

				// 8 bytes remaining here

				t_modOfMod modOfmodulation;


			} lfo2Mod_And_ModofMod;
			tStorageDigitalOsc digitalOsc; // when we did use this it was only used on the triangle wave page
		} more;

	//uint16_t unusedAgain;
	//uint16_t unusedAgain2;
} singleParam;

#define CUSTOM_LFO_VERTICES 32
#define CUSTOM_LFO_MAX_NUMBER 4

typedef struct // 68 bytes
{
	uint8_t version;
	uint8_t startTime; // allows to set the point where LFO start in single-shot mode, or when reset


	// do this later if we need to save space....
//	uint8_t modeGlide[CUSTOM_LFO_VERTICES / 8]; // 1 bit per vertex
//	uint8_t active[CUSTOM_LFO_VERTICES / 8];
	uint8_t unused2;
	uint8_t unused3;
	struct
	{
		uint8_t mode:1; // stepped or glide
		uint8_t active:1; // editable or not
		uint8_t unused:6;
	} meta[CUSTOM_LFO_VERTICES];
	int8_t values[CUSTOM_LFO_VERTICES];

} t_customLFOWave;



#define NUMBER_OF_CC_MAPPINGS 24
#define NUMBER_OF_CC_MAPPINGS_BYTES ((NUMBER_OF_CC_MAPPINGS * 3) / 2)
#define FIRST_MIDI_CC_FOR_CONTROL 1
typedef struct // 36 bytes in size
{
	// holds the mapping from CC controllers to control page/parameter

	uint8_t mapByte[NUMBER_OF_CC_MAPPINGS_BYTES];


} t_CCControlStruct;




typedef  PACKED_STRUCT()
{
	// MUST be 8 bytes in total

	// interesting - on VS use of 'unsigned int : 'caused the structure to be 20 bytes long, 'uint8 : ' fixed that.
	uint16_t tempo;

	uint8_t  mode : 4;
	uint8_t  unused : 4;
	uint8_t pattern;
	uint8_t  range : 4; // 1 to 10 octaves
	uint8_t  unused2 : 4;
	uint8_t  active : 3;// on, off or hold, internal
	uint8_t  delay : 5; // +/- ARPEG_DELAY_OFFSET clock ticks
	uint8_t swing;
	uint8_t noteLength;

} tArpegSettings;

typedef  PACKED_STRUCT()
{
	uint8_t chord;
	uint8_t notes;
	uint8_t noteVals[8];

	uint8_t velocityVals[8];
}
tArpegChordSettings;





typedef union
{
	uint32_t value;
	struct
	{
		uint32_t writeProtected: 1;
	} fields;
} tBitField;

#define NUMBER_OF_EG_TRIGGER_SOURCES 8

typedef  PACKED_STRUCT()
{
	// this is now v1 of the structure - remember if definition changes then copy structure and name as above '_vX'
	PACKED_STRUCT()
	{
		uint32_t identifier;
		uint32_t crc32;
		char name[LENGTH_OF_PATCH_NAME];
		uint32_t versionNumber; // used to denote latest version of this patch

		tBitField bitfield;
		uint8_t MIDIChannel;
		uint8_t noteLow;
		uint8_t noteHigh;
		uint8_t paddingB;

		uint8_t edited; // set to 1 once value lock is off or data is changed.
		uint8_t microTuneOption;
		uint8_t unusedB1;
		uint8_t unusedB2;
		//uint32_t unused0; // for future expansion
		uint32_t unused1; // for future expansion

	} header;


	tCommonLFO commonLFOs[NUM_GLOBAL_LFOS];
	tEGCore commonEGs[NUM_GLOBAL_EGS];


	uint8_t triggerMode; // 0 is lastnote, 1 is highest note
	uint8_t portamentoMode;
	uint16_t portamentoTime;
	uint16_t pitchWheelSensitivity;
	uint8_t portamentoTimeOrRate;
	uint8_t lfoSync;
	uint32_t unusedData1; // for future expansion

	uint8_t MOD1Source;
	uint8_t MOD2Source;
	uint8_t MOD3Source;
	uint8_t MOD4Source;



	uint8_t MOD1unipolar;
	uint8_t MOD2unipolar;
	uint8_t MOD3unipolar;
	uint8_t MOD4unipolar;

	/*
	uint8_t MOD5SourceUnused; // for future expansion
	uint8_t MOD6SourceUnused; // for future expansion
	uint8_t MOD7SourceUnused; // for future expansion
	uint8_t MOD8SourceUnused; // for future expansion
*/


	tArpegSettings arpeg;
	//char	extraName[16];
	uint8_t noteOffTracking;

	uint8_t egTriggerSource[NUMBER_OF_EG_TRIGGER_SOURCES];


	uint8_t advancedModulationMode;
	uint8_t unusedBuffer[14];


	//uint8_t unusedBuffer[23];

	union
	{
		// this holds all the data necessary (and no more!) to store a patch to disk
		// extra data such as the instantaneous state of EG or LFO signal is stored elsewhere
		singleParam singleParams[PARAM_END];
		PACKED_STRUCT()
		{
			singleParam MasterPitch;
			singleParam Vco1_Level;
			singleParam Vco1Pitch;
			singleParam Vco1Tri;
			singleParam Vco1Saw;
			singleParam Vco1PWMMod;
			singleParam Vco1PWMLevel;

#if MOTAS_VERSION == 6
			singleParam Vco2PM;
#else
			singleParam Vco1_2XMOD; // 8
#endif


			singleParam Vco2_Level;
			singleParam Vco2Pitch;
			singleParam Vco2Tri;
			singleParam Vco2Saw;
			singleParam Vco2Square;



#if MOTAS_VERSION == 6
			singleParam Vco2Sub;
			singleParam Vco3PM; // 15
#else
			singleParam Vco2XOR;
			singleParam Vco3Sub; // 15
#endif
			singleParam Vco3_Level;
			singleParam Vco3Pitch;
			singleParam Vco3Tri;
			singleParam Vco3Saw;
			singleParam Vco3PWMMod;
			singleParam Vco3PWMLevel;


			singleParam Noise; //22
			singleParam Mix;
			singleParam Vcf1Freq;
			singleParam Vcf1Res;
			singleParam VcaVcf1;
			singleParam HpfFreq;
			singleParam VcaHpf;


			singleParam ExternIn; // 29
			singleParam Vcf2Freq;
			singleParam Vcf2Res;
			singleParam VcaVcf2;
			singleParam VcaMaster;
/*
			singleParam paramUnused0; // for future expansion
			singleParam paramUnused1; // for future expansion
			singleParam paramUnused2; // for future expansion. REMOVED to make way for custom LFO shapes
*/

			t_customLFOWave customLFOs[CUSTOM_LFO_MAX_NUMBER];

			t_CCControlStruct patchCCControl; // 36 bytes

			uint8_t unusedBytes[16];



		} paramNames;
	}params;

	//customLFOShapes_t customLFOshapes;




} tAllParams;


const uint16_t  MINIMUM_FIRMWARE_VERSION = 0x0104;




const uint32_t NEW_FIRMWARE_START_ADDRESS = 768;
const uint32_t MASS_FLASH_TRANSFER_LENGTH_BYTES  = NEW_FIRMWARE_START_ADDRESS * 4096L;
const uint32_t SETUP_DATA_LENGTH_BYTES = 256;



const uint8_t SYSEX_HEADER_BYTES   = 12;
const uint8_t BANK_OF_PATCHES_LENGTH  = 50;
const uint8_t BANK_OF_PRESETS_LENGTH = 50;
const uint8_t BANK_OF_PATTERNS_LENGTH = 8;
const uint8_t BANK_OF_SEQUENCES_LENGTH = 8;

const uint32_t PATCH_LENGTH_BYTES_UNPACKED  = 4096UL;
const uint32_t  PATTERN_LENGTH_BYTES_UNPACKED = 2048UL;  //192 // using sizeof
const uint32_t  SEQUENCE_LENGTH_BYTES_UNPACKED  = (4096UL * (5 + 8));

const uint32_t LARGEST_BUFFER_LENGTH = 1024*1024L;




const uint8_t OFFSET_FOR_NON_MIDI_CONTROLLERS = (128 -32 - 4 - 8); // enter number of controlling things at start of list other than the midi controllers

#define MOTAS_VERSION  6
#define MINIMUM_FIRMWARE_VERSION 0x011C



enum
{
    MIDI_SYSEX_NOT_SET  = 0,
    MIDI_SYSEX_REQUEST_BITMAP = 10,
    MIDI_SYSEX_REQUEST_OSCILLOSCOPE_TRACE = 11,
    MIDI_SYSEX_REQUEST_BULK_FLASH_DATA_SEND = 12,
    MIDI_SYSEX_RECEIVE_BULK_FLASH_DATA = 13,
	MIDI_SYSEX_REQUEST_INFO = 14,

    MIDI_SYSEX_REQUEST_GLOBALS = 15,

	MIDI_SYSEX_REQUEST_BITMAP_COMPRESSED = 16,
	MIDI_SYSEX_REQUEST_STATUS = 17,
    MIDI_SYSEX_REQUEST_PATCH = 20,
    MIDI_SYSEX_REQUEST_PATCHES_BANK,

    MIDI_SYSEX_REQUEST_PATTERN = 25,
    MIDI_SYSEX_REQUEST_PATTERN_BANK,

    MIDI_SYSEX_REQUEST_SEQUENCE = 30,
    MIDI_SYSEX_REQUEST_SEQUENCE_BANK,


    MIDI_SYSEX_BITMAP_TRANSFER = 40,
    MIDI_SYSEX_OSCILLOSCOPE_TRANSFER = 41,
	MIDI_SYSEX_INFO_TRANSFER,
	MIDI_SYSEX_BITMAP_COMPRESSED_TRANSFER,
    MIDI_SYSEX_GLOBALS_TRANSFER = 45,

    MIDI_SYSEX_PATCH_TRANSFER = 50,
    MIDI_SYSEX_PATCH_BANK_TRANSFER,
    MIDI_SYSEX_PATTERN_TRANSFER = 55,
    MIDI_SYSEX_PATTERN_BANK_TRANSFER,
    MIDI_SYSEX_SEQUENCE_TRANSFER = 60,
    MIDI_SYSEX_SEQUENCE_BANK_TRANSFER,
	MIDI_SYSEX_STATUS_TRANSFER = 62,

    MIDI_SYSEX_FIRMWARE = 70, // 0x32
    MIDI_SYSEX_START_TRANSFER,
    MIDI_SYSEX_CONTINUE_TRANSFER,
    MIDI_SYSEX_END_TRANSFER, //0x35
    MIDI_SYSEX_START_AND_END_TRANSFER, // used for small transfers
    MIDI_SYSEX_ABORT_TRANSFER,
};









enum
	{
		MIDI_DATA_SEND_TYPE_UNKNOWN,

		MIDI_DATA_SEND_TYPE_NRPN_FROM_MOTAS_EDIT,
		MIDI_DATA_SEND_TYPE_NOTES_FROM_MOTAS_EDIT,
		MIDI_DATA_SEND_TYPE_CONTROLLER_FROM_MOTAS_EDIT,
		MIDI_DATA_SEND_TYPE_PROGRAM_CHANGE_MOTAS_EDIT,
		MIDI_DATA_SEND_TYPE_SYSEX_FROM_MOTAS_EDIT,
		MIDI_DATA_SEND_TYPE_SYSEX_FIRMWARE_FROM_MOTAS_EDIT,
		MIDI_DATA_SEND_TYPE_SYSEX_REQUEST_FROM_MOTAS_EDIT,


		MIDI_DATA_SEND_TYPE_NOTES_FROM_DAW,
		MIDI_DATA_SEND_TYPE_CONTROLLERS_FROM_DAW,
		MIDI_DATA_SEND_TYPE_PITCH_WHEEL_FROM_DAW,
		MIDI_DATA_SEND_TYPE_OTHER_FROM_DAW,
		MIDI_DATA_SEND_TYPE_NRPN_FROM_DAW,
		MIDI_DATA_SEND_TYPE_SYSEX_FROM_DAW,


		MIDI_DATA_SEND_TYPE_NRPN_FROM_MOTAS,
		MIDI_DATA_SEND_TYPE_SYSEX_FROM_MOTAS,
		MIDI_DATA_SEND_TYPE_OTHER_FROM_MOTAS,


	};



typedef union
{
	struct
	{
		unsigned int NRPN_paramMSBReceived: 1;
		unsigned int NRPN_paramLSBReceived: 1;
		unsigned int NRPN_dataMSBReceived: 1;
		unsigned int NRPN_dataLSBReceived: 1;
	} bits;
	unsigned int value;
} NRPNinit_t;





enum
{

	LFO_TYPE_SINE,
	LFO_TYPE_SINE_90DEG,
	LFO_TYPE_TRIANGLE,
	LFO_TYPE_TRIANGLE_90DEG,
	LFO_TYPE_SQUARE,
	LFO_TYPE_PULSE_1,
	LFO_TYPE_PULSE_2,
	LFO_TYPE_PULSE_3,

	LFO_TYPE_RAMP,
	LFO_TYPE_RAMP_90DEG,
	LFO_TYPE_RAMP_HOLD,
	LFO_TYPE_3_STEP,
	LFO_TYPE_4_STEP,
	LFO_TYPE_SAMPLE_AND_HOLD,
	LFO_TYPE_NOISE,


	LFO_TYPE_RANDOM_TRIANGLE,


	// NEW SHAPES added in firmware v0602-0102

	LFO_TYPE_PULSE_1_REVERSE,
	LFO_TYPE_PULSE_2_REVERSE,
	LFO_TYPE_PULSE_3_REVERSE,
	LFO_TYPE_RAMP_HOLD_INVERT,

	LFO_TYPE_BOUNCE,
	LFO_TYPE_BOING,
	LFO_TYPE_RETARD,
	LFO_TYPE_SIN_DECAY_DEC,

	LFO_TYPE_SIN_DECAY_INC,
	LFO_TYPE_BURST,

	LFO_TYPE_BURST2,


	LFO_TYPE_SIN_PLUS_SIN2,
	LFO_TYPE_SIN_PLUS_SIN3,

	LFO_TYPE_SIN_TRI,
	LFO_TYPE_TRI_HALF,
	LFO_TYPE_SIN_BEAT,
	LFO_TYPE_SIN_BEAT2,

	LFO_TYPE_CHROMATIC,

	LFO_TYPE_MAJOR_TRIAD,
	LFO_TYPE_MINOR_TRIAD,
	LFO_TYPE_AUG_TRIAD,
	LFO_TYPE_DIM_TRIAD,

	LFO_TYPE_DIM_7TH,
	LFO_TYPE_HALF_DIM_7TH,
	LFO_TYPE_MINOR_SEVENTH,
	LFO_TYPE_MINOR_MAJOR_SEVENTH,
	LFO_TYPE_DOMINANT_SEVENTH,
	LFO_TYPE_MAJOR_SEVENTH,
	LFO_TYPE_AUG_SEVENTH,
	LFO_TYPE_AUG_MAJOR_SEVENTH,



	LFO_TYPE_SEQ1,
	LFO_TYPE_SEQ2,
	LFO_TYPE_SEQ3,
	LFO_TYPE_SEQ4,



	LFO_TYPE_MAX_VALUE
};

enum
{
	MOD_DEST_LFO1_LEVEL,
	MOD_DEST_LFO2_FREQ,
	MOD_DEST_LFO2_LEVEL,
	MOD_DEST_EG_RATE,
	MOD_DEST_EG_ATTACK,
	MOD_DEST_EG_DECAY,
	MOD_DEST_EG_SUSTAIN,
	MOD_DEST_EG_RELEASE,
	MOD_DEST_EG_LEVEL,
	MOD_DEST_END
};

const uint8_t EG_RETRIG_MAX  = 7 + 8; // added trigger options
const uint8_t MAX_VALUE_EG_RESTART_ON_TRIGGER = 7 ;// a bitfield of 3 values
//const uint8_t POLY_TRACK_NUMBER = (EG_RETRIG_MAX - 1);

const uint32_t EG_RATE_MAX_VALUE = 312076; // this is the max value setting allowed for the interface (ADR parameters). Set to give 1ms rise time when at max value
const uint8_t EG_RATE_MIN_VALUE = 8;

enum
{
	LFO_MODE_NORMAL,

	LFO_MODE_NOTE_TRACK,
	LFO_MODE_PITCH_TRACK,
	LFO_MODE_CLK_SYNC,
	LFO_MODE_MAX,
};


const uint8_t LFO1_MAX_CHOICE_VALUE = (LFO_TYPE_MAX_VALUE - 1);
const uint8_t LFO2_MAX_CHOICE_VALUE = (LFO_TYPE_MAX_VALUE - 1);
enum
{
	LFO_TRIGGER_OFF,
	LFO_TRIGGER_FIRST_NOTE_ON,
	LFO_TRIGGER_ALL_NOTE_ON,
	LFO_TRIGGER_MAX_STATE
} ;


enum
{
	MOD_SOURCE_CHOICE_CH_AFTERTOUCH  = OFFSET_FOR_NON_MIDI_CONTROLLERS,
	MOD_SOURCE_CHOICE_PITCH_BEND,
#if MOTAS_VERSION == 6
	MOD_SOURCE_CHOICE_CV_GATE_CH1,
	MOD_SOURCE_CHOICE_CV_GATE_CH2, //MUST be 1 more than prev
	MOD_SOURCE_CHOICE_CV_GATE_CH3, //MUST be 1 more than prev
	MOD_SOURCE_CHOICE_CV_GATE_CH4 //MUST be 1 more than prev
#endif
};


#define NRPN_MSB_GLOBAL_ADVANCED_MODULATION  64
enum
{
	// put the continuous controllers at values < 64
	NRPN_VALUE_PAGE_OFFSET = 1,


	NRPN_VALUE_PAGE_EG1_MOD_DEPTH = 5,


	NRPN_VALUE_PAGE_EG1_DELAY = 10,
	NRPN_VALUE_PAGE_EG1_ATTACK,
	NRPN_VALUE_PAGE_EG1_DECAY,
	NRPN_VALUE_PAGE_EG1_SUSTAIN,
	NRPN_VALUE_PAGE_EG1_RELEASE,

	NRPN_VALUE_PAGE_LFO1_MOD_DEPTH = 20,


	NRPN_VALUE_PAGE_LFO2_FREQ = 25,
	NRPN_VALUE_PAGE_LFO2_MOD_DEPTH,


	NRPN_VALUE_PAGE_MOD1_MOD_DEPTH = 30,
	NRPN_VALUE_PAGE_MOD1_MOD_DEPTH_ALTERNATE,
	NRPN_VALUE_PAGE_MOD1_MOD_UNIPOLAR,


	NRPN_VALUE_PAGE_MOD2_MOD_DEPTH = 35,
	NRPN_VALUE_PAGE_MOD2_MOD_DEPTH_ALTERNATE,
	NRPN_VALUE_PAGE_MOD2_MOD_UNIPOLAR,

	NRPN_VALUE_PAGE_MOD3_MOD_DEPTH = 40,
	NRPN_VALUE_PAGE_MOD3_MOD_DEPTH_ALTERNATE,
	NRPN_VALUE_PAGE_MOD3_MOD_UNIPOLAR,

	NRPN_VALUE_PAGE_MOD4_MOD_DEPTH = 45,
	NRPN_VALUE_PAGE_MOD4_MOD_DEPTH_ALTERNATE,
	NRPN_VALUE_PAGE_MOD4_MOD_UNIPOLAR,


	NRPN_VALUE_PAGE_VELOCITY_MOD_DEPTH = 50,
	NRPN_VALUE_PAGE_VELOCITY_MOD_DEPTH_ALTERNATE,
	NRPN_VALUE_PAGE_VELOCITY_MOD_UNIPOLAR,



	NRPN_VALUE_PAGE_NOTE_MOD_DEPTH = 55,
	NRPN_VALUE_PAGE_NOTE_MOD_DEPTH_ALTERNATE,
	NRPN_VALUE_PAGE_NOTE_MOD_UNIPOLAR,



	NRPN_VALUE_PAGE_OPTION1 = 64,
	NRPN_VALUE_PAGE_OPTION2,

	NRPN_VALUE_PAGE_EG1_SOURCE = 70,
	NRPN_VALUE_PAGE_EG1_RETRIG,
	NRPN_VALUE_PAGE_EG1_RESTART_ON_TRIGGER,

	NRPN_VALUE_PAGE_EG1_SHAPE = 75,
	NRPN_VALUE_PAGE_EG1_SHAPE_MULTI = 76,
	NRPN_VALUE_PAGE_EG1_UNIPOLAR = 78,

	NRPN_VALUE_PAGE_LFO1_SOURCE = 80,

	NRPN_VALUE_PAGE_LFO2_WAVEFORM = 85,
	NRPN_VALUE_PAGE_LFO2_TRIGGER,
	NRPN_VALUE_PAGE_LFO2_SINGLE_SHOT,

	NRPN_VALUE_PAGE_LFO2_OUTPUT_MODE,
	NRPN_VALUE_PAGE_LFO2_PITCH_TRACK,

	NRPN_VALUE_PAGE_MOD1_DEST = 90,

	NRPN_VALUE_PAGE_LFO2_LFOX_FREQ_MOD = 92,
	NRPN_VALUE_PAGE_LFO2_EG_FREQ_MOD,

	NRPN_VALUE_PAGE_MOD2_DEST = 95,
	NRPN_VALUE_PAGE_MOD3_DEST = 100,
	NRPN_VALUE_PAGE_MOD4_DEST = 105,
	NRPN_VALUE_PAGE_VELOCITY_DEST = 110,
	NRPN_VALUE_PAGE_NOTE_DEST = 115,

} ;

enum
{
	// put the continuous controllers at values < 64

	NRPN_VALUE_GLOBAL_LFO1_FREQ = 4,
	NRPN_VALUE_GLOBAL_LFO2_FREQ,
	NRPN_VALUE_GLOBAL_LFO3_FREQ,
	NRPN_VALUE_GLOBAL_LFO4_FREQ,


	NRPN_VALUE_GLOBAL_EG1_DELAY = 12,
	NRPN_VALUE_GLOBAL_EG2_DELAY,
	NRPN_VALUE_GLOBAL_EG3_DELAY,
	NRPN_VALUE_GLOBAL_EG4_DELAY,


	NRPN_VALUE_GLOBAL_EG1_ATTACK = 20,
	NRPN_VALUE_GLOBAL_EG2_ATTACK,
	NRPN_VALUE_GLOBAL_EG3_ATTACK,
	NRPN_VALUE_GLOBAL_EG4_ATTACK,

	NRPN_VALUE_GLOBAL_EG1_DECAY = 28,
	NRPN_VALUE_GLOBAL_EG2_DECAY,
	NRPN_VALUE_GLOBAL_EG3_DECAY,
	NRPN_VALUE_GLOBAL_EG4_DECAY,

	NRPN_VALUE_GLOBAL_EG1_SUSTAIN = 36,
	NRPN_VALUE_GLOBAL_EG2_SUSTAIN,
	NRPN_VALUE_GLOBAL_EG3_SUSTAIN,
	NRPN_VALUE_GLOBAL_EG4_SUSTAIN,


	NRPN_VALUE_GLOBAL_EG1_RELEASE = 44,
	NRPN_VALUE_GLOBAL_EG2_RELEASE,
	NRPN_VALUE_GLOBAL_EG3_RELEASE,
	NRPN_VALUE_GLOBAL_EG4_RELEASE,


	NRPN_VALUE_GLOBAL_PORTAMENTO_MODE = 52,
	NRPN_VALUE_GLOBAL_PORTAMENTO_TIME,
	NRPN_VALUE_GLOBAL_PORTAMENTO_TIME_OR_RATE,
	NRPN_VALUE_GLOBAL_LFO_SYNC_MIDI_START,




	NRPN_VALUE_GLOBAL_MOD1_SOURCE = 64,
	NRPN_VALUE_GLOBAL_MOD2_SOURCE,
	NRPN_VALUE_GLOBAL_MOD3_SOURCE,
	NRPN_VALUE_GLOBAL_MOD4_SOURCE,


	NRPN_VALUE_GLOBAL_LFO1_TRIGGER = 72,
	NRPN_VALUE_GLOBAL_LFO2_TRIGGER,
	NRPN_VALUE_GLOBAL_LFO3_TRIGGER,
	NRPN_VALUE_GLOBAL_LFO4_TRIGGER,

	NRPN_VALUE_GLOBAL_LFO1_SINGLE_SHOT = 76,
	NRPN_VALUE_GLOBAL_LFO2_SINGLE_SHOT,
	NRPN_VALUE_GLOBAL_LFO3_SINGLE_SHOT,
	NRPN_VALUE_GLOBAL_LFO4_SINGLE_SHOT,



	NRPN_VALUE_GLOBAL_LFO1_WAVEFORM = 80,
	NRPN_VALUE_GLOBAL_LFO2_WAVEFORM,
	NRPN_VALUE_GLOBAL_LFO3_WAVEFORM,
	NRPN_VALUE_GLOBAL_LFO4_WAVEFORM,

	NRPN_VALUE_GLOBAL_LFO1_PITCH_TRACK = 84,
	NRPN_VALUE_GLOBAL_LFO2_PITCH_TRACK ,
	NRPN_VALUE_GLOBAL_LFO3_PITCH_TRACK ,
	NRPN_VALUE_GLOBAL_LFO4_PITCH_TRACK ,


	NRPN_VALUE_GLOBAL_EG1_TRIGGER = 88,
	NRPN_VALUE_GLOBAL_EG2_TRIGGER,
	NRPN_VALUE_GLOBAL_EG3_TRIGGER,
	NRPN_VALUE_GLOBAL_EG4_TRIGGER,

	NRPN_VALUE_GLOBAL_EG1_RESTART_ON_TRIGGER = 96,
	NRPN_VALUE_GLOBAL_EG2_RESTART_ON_TRIGGER,
	NRPN_VALUE_GLOBAL_EG3_RESTART_ON_TRIGGER,
	NRPN_VALUE_GLOBAL_EG4_RESTART_ON_TRIGGER,


	NRPN_VALUE_GLOBAL_EG1_SHAPE = 104,
	NRPN_VALUE_GLOBAL_EG2_SHAPE,
	NRPN_VALUE_GLOBAL_EG3_SHAPE,
	NRPN_VALUE_GLOBAL_EG4_SHAPE,

	NRPN_VALUE_GLOBAL_EG1_UNIPOLAR = 110,
	NRPN_VALUE_GLOBAL_EG2_UNIPOLAR,
	NRPN_VALUE_GLOBAL_EG3_UNIPOLAR,
	NRPN_VALUE_GLOBAL_EG4_UNIPOLAR,

	NRPN_VALUE_GLOBAL_EG1_SHAPE_MULTI = 114,  // new version with only 128 shapes
	NRPN_VALUE_GLOBAL_EG2_SHAPE_MULTI,
	NRPN_VALUE_GLOBAL_EG3_SHAPE_MULTI,
	NRPN_VALUE_GLOBAL_EG4_SHAPE_MULTI,


	NRPN_VALUE_GLOBAL_ADVANCED_MODULATION = 118,


	NRPN_VALUE_GLOBAL_SET_NEW_NRPN_CHANGES_PAGE = 127// not sure this is used anymore, SysEx command instead


} ;
const uint8_t MAX_MIDI_CONTROL_VALUE = (MOD_SOURCE_CHOICE_PITCH_BEND + 4); // 4 is for CV/gate



#endif
