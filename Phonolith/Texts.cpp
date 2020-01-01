/*
Phonolith Software Sampler
Copyright (C) 2006-2008 Timo Kloss

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include <iostream>

using namespace std;

#include "Texts.h"

#include <proto/locale.h>

Catalog *Texts::mCatalog = NULL;

// APPLICATION
const char *Texts::REQ_UNREGISTERED;
const char *Texts::YES_NO;
const char *Texts::DUMMY;
const char *Texts::PHONOLITH_MENU;
const char *Texts::PHONOLITH_SETTINGS_MENU;
const char *Texts::PHONOLITH_ABOUT_MENU;
const char *Texts::PHONOLITH_QUIT_MENU;
const char *Texts::PROJECT_MENU;
const char *Texts::PROJECT_NEW_MENU;
const char *Texts::PROJECT_LOAD_MENU;
const char *Texts::PROJECT_SAVE_MENU;
const char *Texts::PROJECT_SAVE_AS_MENU;
const char *Texts::REQ_ABOUT_1;
const char *Texts::REQ_ABOUT_2;
const char *Texts::REQ_ABOUT_3;
const char *Texts::REQ_NEW_PROJECT_NOT_SAVED;
const char *Texts::REQ_NEW_PROJECT;
const char *Texts::ASL_LOAD_PROJECT;
const char *Texts::ASL_SAVE_PROJECT;
const char *Texts::REQ_QUIT_NOT_SAVED;
const char *Texts::REQ_QUIT;
const char *Texts::STATUS_RECORDING;
const char *Texts::STATUS_ACTIVE_WAITING;
const char *Texts::STATUS_ACTIVE;
const char *Texts::STATUS_NOT_ACTIVE_RECORDING;
const char *Texts::STATUS_NOT_ACTIVE;
const char *Texts::STATUS_SHUTTING_DOWN;

// MIXER WINDOW
const char *Texts::MASTER_GAD;
const char *Texts::STATUS_GAD;
const char *Texts::PLAYER_GAD;
const char *Texts::MASTER_VOLUME_HINT;
const char *Texts::MASTER_METER_HINT;
const char *Texts::ACTIVATE_HINT;
const char *Texts::DEACTIVATE_HINT;
const char *Texts::MIXDOWN_HINT;
const char *Texts::CHANNEL_METER_HINT;
const char *Texts::CHANNEL_VOLUME_HINT;
const char *Texts::CHANNEL_PANORAMA_HINT;
const char *Texts::CHANNEL_EDIT_HINT;
const char *Texts::REGISTERED_FOR;
const char *Texts::ASL_MIXDOWN;

// INSTRUMENT WINDOW
const char *Texts::SAMPLE_NAME_GAD;
const char *Texts::SORT_BY_BASE_KEY_GAD;
const char *Texts::READ_BASE_KEYS_FROM_NAMES_GAD;
const char *Texts::SPREAD_BASE_KEYS_GAD;
const char *Texts::SET_KEY_RANGES_AROUND_BASE_KEYS_GAD;
const char *Texts::SHIFT_KEYS_GAD;
const char *Texts::INSTRUMENT_NAME_GAD;
const char *Texts::SETTINGS_GAD;
const char *Texts::VOICES_GAD;
const char *Texts::AMPLIFIER_ENVELOPE_GAD;
const char *Texts::ATTACK_TIME_GAD;
const char *Texts::DECAY_TIME_GAD;
const char *Texts::SUSTAIN_LEVEL_GAD;
const char *Texts::RELEASE_TIME_GAD;
const char *Texts::ZONES_GAD;
const char *Texts::TOOLS_GAD;
const char *Texts::ZONE_SETTINGS_GAD;
const char *Texts::ASL_CHOOSE_SAMPLE_FILE;
const char *Texts::SAMPLE_FILE_GAD;
const char *Texts::START_END_GAD;
const char *Texts::BASE_KEY_GAD;
const char *Texts::KEY_RANGE_GAD;
const char *Texts::VELO_RANGE_GAD;
const char *Texts::TUNE_GAD;
const char *Texts::FINE_TUNE_GAD;
const char *Texts::PANORAMA_GAD;
const char *Texts::ADD_HINT;
const char *Texts::REMOVE_HINT;
const char *Texts::MOVE_UP_HINT;
const char *Texts::MOVE_DOWN_HINT;
const char *Texts::PLAY_SAMPLE_HINT;
const char *Texts::PLAY_TONE_HINT;
const char *Texts::INSTRUMENT_MENU;
const char *Texts::INSTRUMENT_NEW_MENU;
const char *Texts::INSTRUMENT_LOAD_MENU;
const char *Texts::INSTRUMENT_IMPORT_EXS24_MENU;
const char *Texts::INSTRUMENT_SAVE_MENU;
const char *Texts::INSTRUMENT_SAVE_AS_MENU;
const char *Texts::INSTRUMENT;
const char *Texts::NO_SAMPLE_GAD;
const char *Texts::ASL_LOAD_SAMPLER_INSTRUMENT;
const char *Texts::ASL_SAVE_SAMPLER_INSTRUMENT;
const char *Texts::ASL_CHOOSE_EXS24_INSTRUMENT;
const char *Texts::ASL_CHOOSE_SAMPLES;
const char *Texts::REQ_WHICH_KEYS_FOR_BASE_KEYS;
const char *Texts::REQ_WHITE_ONLY_WHITE_AND_BLACK;
const char *Texts::REQ_ZONES_MUST_BE_SORTED;
const char *Texts::REQ_SORT_BEFORE_CANCEL;
const char *Texts::REQ_HOW_MANY_OCTAVES;

// SETTINGS WINDOW
const char *Texts::MIDI_INPUT_GAD;
const char *Texts::REFRESH_PORT_LIST_GAD;
const char *Texts::CHANGES_AFTER_PLAYER_RESTART_GAD;
const char *Texts::AUDIO_OUTPUT_GAD;
const char *Texts::SELECT_AUDIO_OUTPUT_GAD;
const char *Texts::AUDIO_MODE_GAD;
const char *Texts::MIX_FREQUENCY_GAD;
const char *Texts::PLAYER_TITLE_GAD;
const char *Texts::BUFFER_SIZE_GAD;
const char *Texts::MAX_VOICES_GAD;
const char *Texts::PATHS_TITLE_GAD;
const char *Texts::PATH_PROJECTS_GAD;
const char *Texts::PATH_INSTRUMENTS_GAD;
const char *Texts::PATH_SAMPLES_GAD;
const char *Texts::PATH_IMPORTS_GAD;
const char *Texts::MISCELLANEOUS_GAD;
const char *Texts::ACTIVATE_PLAYER_ON_START_GAD;
const char *Texts::MIDI_GAD;
const char *Texts::AUDIO_GAD;
const char *Texts::SAMPLER_GAD;
const char *Texts::APPLICATION_GAD;
const char *Texts::SAVES_WINDOW_POSITIONS_GAD;
const char *Texts::SAVE_GAD;
const char *Texts::USE_GAD;
const char *Texts::CANCEL_GAD;
const char *Texts::SETTINGS;

// IO UTILS
const char *Texts::REQ_SELECT_MANUALLY_1;
const char *Texts::REQ_SELECT_MANUALLY_2;
const char *Texts::REQ_SELECT_MANUALLY_3;
const char *Texts::ASL_IO_SELECT_SAMPLE_FILE;


//==================================================================================
// Initialize
//==================================================================================

void Texts::init()
{
	mCatalog = ILocale->OpenCatalog(NULL, "Phonolith.catalog",
			OC_BuiltInLanguage, "english",
			TAG_DONE);

	// APPLICATION
	REQ_UNREGISTERED                   = ILocale->GetCatalogStr(mCatalog, 100, "This is the unregistered Lite version!\n\nAIFF mixdown is disabled.\n\nCheck www.inutilis.de/phonolith for registering!");
	YES_NO                             = ILocale->GetCatalogStr(mCatalog, 101, "Yes|No");
	DUMMY                              = ILocale->GetCatalogStr(mCatalog, 102, "dummy");
	PHONOLITH_MENU                     = ILocale->GetCatalogStr(mCatalog, 103, "Phonolith");
	PHONOLITH_SETTINGS_MENU            = ILocale->GetCatalogStr(mCatalog, 104, "Settings...");
	PHONOLITH_ABOUT_MENU               = ILocale->GetCatalogStr(mCatalog, 105, "About...");
	PHONOLITH_QUIT_MENU                = ILocale->GetCatalogStr(mCatalog, 106, "Quit");
	PROJECT_MENU                       = ILocale->GetCatalogStr(mCatalog, 107, "Project");
	PROJECT_NEW_MENU                   = ILocale->GetCatalogStr(mCatalog, 108, "New");
	PROJECT_LOAD_MENU                  = ILocale->GetCatalogStr(mCatalog, 109, "Load...");
	PROJECT_SAVE_MENU                  = ILocale->GetCatalogStr(mCatalog, 110, "Save");
	PROJECT_SAVE_AS_MENU               = ILocale->GetCatalogStr(mCatalog, 111, "Save as...");
	REQ_ABOUT_1                        = ILocale->GetCatalogStr(mCatalog, 112, "\33bPhonolith\33n\nSoftware Sampler\nVersion ");
	REQ_ABOUT_2                        = ILocale->GetCatalogStr(mCatalog, 113, " (compiled ");
	REQ_ABOUT_3                        = ILocale->GetCatalogStr(mCatalog, 114, ")\n\n© 2006-2008 Inutilis Software / TK\nDeveloped by Timo Kloss\nThis is free software (GPL)\n\nhttp://www.inutilis.de/phonolith\nE-Mail: Timo@inutilis.de");
	REQ_NEW_PROJECT_NOT_SAVED          = ILocale->GetCatalogStr(mCatalog, 115, "Project is not saved!\nDo you really want to create a new one?");
	REQ_NEW_PROJECT                    = ILocale->GetCatalogStr(mCatalog, 116, "Do you really want to create a new project?");
	ASL_LOAD_PROJECT                   = ILocale->GetCatalogStr(mCatalog, 117, "Load Sampler Project");
	ASL_SAVE_PROJECT                   = ILocale->GetCatalogStr(mCatalog, 118, "Save Sampler Project");
	REQ_QUIT_NOT_SAVED                 = ILocale->GetCatalogStr(mCatalog, 119, "Project is not saved!\nDo you really want to quit?");
	REQ_QUIT                           = ILocale->GetCatalogStr(mCatalog, 120, "Do you really want to quit?");
	STATUS_RECORDING                   = ILocale->GetCatalogStr(mCatalog, 121, "Player is recording to file '");
	STATUS_ACTIVE_WAITING              = ILocale->GetCatalogStr(mCatalog, 122, "Player is active (recording is waiting for MIDI...)");
	STATUS_ACTIVE                      = ILocale->GetCatalogStr(mCatalog, 123, "Player is active");
	STATUS_NOT_ACTIVE_RECORDING        = ILocale->GetCatalogStr(mCatalog, 124, "Player is not active (recording is requested)");
	STATUS_NOT_ACTIVE                  = ILocale->GetCatalogStr(mCatalog, 125, "Player is not active");
	STATUS_SHUTTING_DOWN               = ILocale->GetCatalogStr(mCatalog, 126, "Shutting down player...");

	// MIXER WINDOW
	MASTER_GAD                         = ILocale->GetCatalogStr(mCatalog, 200, "Master");
	STATUS_GAD                         = ILocale->GetCatalogStr(mCatalog, 201, "Status");
	PLAYER_GAD                         = ILocale->GetCatalogStr(mCatalog, 202, "Player");
	MASTER_VOLUME_HINT                 = ILocale->GetCatalogStr(mCatalog, 203, "Master Volume");
	MASTER_METER_HINT                  = ILocale->GetCatalogStr(mCatalog, 204, "Master Meter\nClick to reset clipping lamps.");
	ACTIVATE_HINT                      = ILocale->GetCatalogStr(mCatalog, 205, "Activates player");
	DEACTIVATE_HINT                    = ILocale->GetCatalogStr(mCatalog, 206, "Deactivates player");
	MIXDOWN_HINT                       = ILocale->GetCatalogStr(mCatalog, 207, "Mixdown to AIFF file");
	CHANNEL_METER_HINT                 = ILocale->GetCatalogStr(mCatalog, 208, "Channel Meter\nClick to reset clipping lamps.");
	CHANNEL_VOLUME_HINT                = ILocale->GetCatalogStr(mCatalog, 209, "Channel Volume");
	CHANNEL_PANORAMA_HINT              = ILocale->GetCatalogStr(mCatalog, 210, "Channel Panorama");
	CHANNEL_EDIT_HINT                  = ILocale->GetCatalogStr(mCatalog, 211, "Opens instrument editor\nClick with SHIFT to open a new window.");
	REGISTERED_FOR                     = ILocale->GetCatalogStr(mCatalog, 212, " registered for ");
	ASL_MIXDOWN                        = ILocale->GetCatalogStr(mCatalog, 213, "Select File Name for AIFF Mixdown");

	// INSTRUMENT WINDOW
	SAMPLE_NAME_GAD                    = ILocale->GetCatalogStr(mCatalog, 300, "Sample/Name");
	SORT_BY_BASE_KEY_GAD               = ILocale->GetCatalogStr(mCatalog, 301, "Sort by base key");
	READ_BASE_KEYS_FROM_NAMES_GAD      = ILocale->GetCatalogStr(mCatalog, 302, "Read base keys from names");
	SPREAD_BASE_KEYS_GAD               = ILocale->GetCatalogStr(mCatalog, 303, "Spread base keys");
	SET_KEY_RANGES_AROUND_BASE_KEYS_GAD = ILocale->GetCatalogStr(mCatalog, 304, "Set key ranges around base keys");
	SHIFT_KEYS_GAD                     = ILocale->GetCatalogStr(mCatalog, 305, "Shift keys");
	INSTRUMENT_NAME_GAD                = ILocale->GetCatalogStr(mCatalog, 306, "Instrument Name");
	SETTINGS_GAD                       = ILocale->GetCatalogStr(mCatalog, 307, "Settings");
	VOICES_GAD                         = ILocale->GetCatalogStr(mCatalog, 308, "Voices");
	AMPLIFIER_ENVELOPE_GAD             = ILocale->GetCatalogStr(mCatalog, 309, "Amplifier Envelope");
	ATTACK_TIME_GAD                    = ILocale->GetCatalogStr(mCatalog, 310, "Attack Time");
	DECAY_TIME_GAD                     = ILocale->GetCatalogStr(mCatalog, 311, "Decay Time");
	SUSTAIN_LEVEL_GAD                  = ILocale->GetCatalogStr(mCatalog, 312, "Sustain Level");
	RELEASE_TIME_GAD                   = ILocale->GetCatalogStr(mCatalog, 313, "Release Time");
	ZONES_GAD                          = ILocale->GetCatalogStr(mCatalog, 314, "Zones");
	TOOLS_GAD                          = ILocale->GetCatalogStr(mCatalog, 315, "Tools");
	ZONE_SETTINGS_GAD                  = ILocale->GetCatalogStr(mCatalog, 316, "Zone Settings");
	ASL_CHOOSE_SAMPLE_FILE             = ILocale->GetCatalogStr(mCatalog, 317, "Choose Sample File");
	SAMPLE_FILE_GAD                    = ILocale->GetCatalogStr(mCatalog, 318, "Sample File");
	START_END_GAD                      = ILocale->GetCatalogStr(mCatalog, 319, "Start/End");
	BASE_KEY_GAD                       = ILocale->GetCatalogStr(mCatalog, 320, "Base Key");
	KEY_RANGE_GAD                      = ILocale->GetCatalogStr(mCatalog, 321, "Key Range");
	VELO_RANGE_GAD                     = ILocale->GetCatalogStr(mCatalog, 322, "Velo Range");
	TUNE_GAD                           = ILocale->GetCatalogStr(mCatalog, 323, "Tune (Semitones)");
	FINE_TUNE_GAD                      = ILocale->GetCatalogStr(mCatalog, 324, "Fine Tune (Cents)");
	PANORAMA_GAD                       = ILocale->GetCatalogStr(mCatalog, 325, "Panorama");
	ADD_HINT                           = ILocale->GetCatalogStr(mCatalog, 326, "Adds one or more zones");
	REMOVE_HINT                        = ILocale->GetCatalogStr(mCatalog, 327, "Removes the selected zone");
	MOVE_UP_HINT                       = ILocale->GetCatalogStr(mCatalog, 328, "Moves selected zone in the list up");
	MOVE_DOWN_HINT                     = ILocale->GetCatalogStr(mCatalog, 329, "Moves selected zone in the list down");
	PLAY_SAMPLE_HINT                   = ILocale->GetCatalogStr(mCatalog, 330, "Plays the sample in its base frequency");
	PLAY_TONE_HINT                     = ILocale->GetCatalogStr(mCatalog, 331, "Plays a tuning tone in the frequency of the base key");
	INSTRUMENT_MENU                    = ILocale->GetCatalogStr(mCatalog, 332, "Instrument");
	INSTRUMENT_NEW_MENU                = ILocale->GetCatalogStr(mCatalog, 333, "New");
	INSTRUMENT_LOAD_MENU               = ILocale->GetCatalogStr(mCatalog, 334, "Load...");
	INSTRUMENT_IMPORT_EXS24_MENU       = ILocale->GetCatalogStr(mCatalog, 335, "Import EXS24...");
	INSTRUMENT_SAVE_MENU               = ILocale->GetCatalogStr(mCatalog, 336, "Save");
	INSTRUMENT_SAVE_AS_MENU            = ILocale->GetCatalogStr(mCatalog, 337, "Save as...");
	INSTRUMENT                         = ILocale->GetCatalogStr(mCatalog, 338, "Instrument");
	NO_SAMPLE_GAD                      = ILocale->GetCatalogStr(mCatalog, 339, "-no sample-");
	ASL_LOAD_SAMPLER_INSTRUMENT        = ILocale->GetCatalogStr(mCatalog, 340, "Load Sampler Instrument");
	ASL_SAVE_SAMPLER_INSTRUMENT        = ILocale->GetCatalogStr(mCatalog, 341, "Save Sampler Instrument");
	ASL_CHOOSE_EXS24_INSTRUMENT        = ILocale->GetCatalogStr(mCatalog, 342, "Choose EXS24 Instrument File");
	ASL_CHOOSE_SAMPLES                 = ILocale->GetCatalogStr(mCatalog, 343, "Choose Sample File(s)");
	REQ_WHICH_KEYS_FOR_BASE_KEYS       = ILocale->GetCatalogStr(mCatalog, 344, "Which keys shall be used for base keys?");
	REQ_WHITE_ONLY_WHITE_AND_BLACK     = ILocale->GetCatalogStr(mCatalog, 345, "White only|White and Black");
	REQ_ZONES_MUST_BE_SORTED           = ILocale->GetCatalogStr(mCatalog, 346, "Zones must be sorted by base key for this tool.");
	REQ_SORT_BEFORE_CANCEL             = ILocale->GetCatalogStr(mCatalog, 347, "Sort before|Cancel");
	REQ_HOW_MANY_OCTAVES               = ILocale->GetCatalogStr(mCatalog, 348, "How many octaves the keys shall be shifted?");

	// SETTINGS WINDOW
	MIDI_INPUT_GAD                     = ILocale->GetCatalogStr(mCatalog, 400, "MIDI Input");
	REFRESH_PORT_LIST_GAD              = ILocale->GetCatalogStr(mCatalog, 401, "Refresh Port List");
	CHANGES_AFTER_PLAYER_RESTART_GAD   = ILocale->GetCatalogStr(mCatalog, 402, "Changes take affect after Player restart.");
	AUDIO_OUTPUT_GAD                   = ILocale->GetCatalogStr(mCatalog, 403, "Audio Output");
	SELECT_AUDIO_OUTPUT_GAD            = ILocale->GetCatalogStr(mCatalog, 404, "Select Audio Output");
	AUDIO_MODE_GAD                     = ILocale->GetCatalogStr(mCatalog, 405, "Audio Mode");
	MIX_FREQUENCY_GAD                  = ILocale->GetCatalogStr(mCatalog, 406, "Mix Frequency");
	PLAYER_TITLE_GAD                   = ILocale->GetCatalogStr(mCatalog, 407, "Player");
	BUFFER_SIZE_GAD                    = ILocale->GetCatalogStr(mCatalog, 408, "Buffer Size (ms)");
	MAX_VOICES_GAD                     = ILocale->GetCatalogStr(mCatalog, 409, "Maximum Voices per Channel");
	PATHS_TITLE_GAD                    = ILocale->GetCatalogStr(mCatalog, 410, "Paths");
	PATH_PROJECTS_GAD                  = ILocale->GetCatalogStr(mCatalog, 411, "Projects");
	PATH_INSTRUMENTS_GAD               = ILocale->GetCatalogStr(mCatalog, 412, "Instruments");
	PATH_SAMPLES_GAD                   = ILocale->GetCatalogStr(mCatalog, 413, "Samples");
	PATH_IMPORTS_GAD                   = ILocale->GetCatalogStr(mCatalog, 414, "Imports");
	MISCELLANEOUS_GAD                  = ILocale->GetCatalogStr(mCatalog, 415, "Miscellaneous");
	ACTIVATE_PLAYER_ON_START_GAD       = ILocale->GetCatalogStr(mCatalog, 416, "Activate Player on Application Start");
	MIDI_GAD                           = ILocale->GetCatalogStr(mCatalog, 417, "MIDI");
	AUDIO_GAD                          = ILocale->GetCatalogStr(mCatalog, 418, "Audio");
	SAMPLER_GAD                        = ILocale->GetCatalogStr(mCatalog, 419, "Sampler");
	APPLICATION_GAD                    = ILocale->GetCatalogStr(mCatalog, 420, "Application");
	SAVES_WINDOW_POSITIONS_GAD         = ILocale->GetCatalogStr(mCatalog, 421, "'Save' also saves all window positions.");
	SAVE_GAD                           = ILocale->GetCatalogStr(mCatalog, 422, "Save");
	USE_GAD                            = ILocale->GetCatalogStr(mCatalog, 423, "Use");
	CANCEL_GAD                         = ILocale->GetCatalogStr(mCatalog, 424, "Cancel");
	SETTINGS                           = ILocale->GetCatalogStr(mCatalog, 425, "Settings");

	// IO UTILS
	REQ_SELECT_MANUALLY_1              = ILocale->GetCatalogStr(mCatalog, 500, "\33bInstrument: ");
	REQ_SELECT_MANUALLY_2              = ILocale->GetCatalogStr(mCatalog, 501, "\33n\n\nCould not find sample file:\n");
	REQ_SELECT_MANUALLY_3              = ILocale->GetCatalogStr(mCatalog, 502, "\n\nPlease select it manually!");
	ASL_IO_SELECT_SAMPLE_FILE          = ILocale->GetCatalogStr(mCatalog, 503, "Choose Sample File");

}

//----------------------------------------------------------------------------------

void Texts::dispose()
{
	ILocale->CloseCatalog(mCatalog);
	mCatalog = NULL;
}

