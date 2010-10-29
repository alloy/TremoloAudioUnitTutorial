#include <AudioUnit/AudioUnit.r>

#include "TremoloVersion.h"

// Note that resource IDs must be spaced 2 apart for the 'STR ' name and description
#define kAudioUnitResID_Tremolo				1000

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Tremolo~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define RES_ID			kAudioUnitResID_Tremolo
#define COMP_TYPE		kAudioUnitType_Effect
#define COMP_SUBTYPE	Tremolo_COMP_SUBTYPE
#define COMP_MANUF		Tremolo_COMP_MANF	

#define VERSION			kTremoloVersion
#define NAME			"Demo: Tremolo"
#define DESCRIPTION		"Tremolo AU"
#define ENTRY_POINT		"TremoloEntry"

#include "AUResources.r"