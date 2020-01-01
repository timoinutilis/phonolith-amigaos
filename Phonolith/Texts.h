#ifndef _TEXTS_
#define _TEXTS_

#include <libraries/locale.h>

class Texts
{
	public:
		static void init();
		static void dispose();

		// APPLICATION
		const static char *REQ_UNREGISTERED;
		const static char *YES_NO;
		const static char *DUMMY;
		const static char *PHONOLITH_MENU;
		const static char *PHONOLITH_SETTINGS_MENU;
		const static char *PHONOLITH_ABOUT_MENU;
		const static char *PHONOLITH_QUIT_MENU;
		const static char *PROJECT_MENU;
		const static char *PROJECT_NEW_MENU;
		const static char *PROJECT_LOAD_MENU;
		const static char *PROJECT_SAVE_MENU;
		const static char *PROJECT_SAVE_AS_MENU;
		const static char *REQ_ABOUT_1;
		const static char *REQ_ABOUT_2;
		const static char *REQ_ABOUT_3;
		const static char *REQ_NEW_PROJECT_NOT_SAVED;
		const static char *REQ_NEW_PROJECT;
		const static char *ASL_LOAD_PROJECT;
		const static char *ASL_SAVE_PROJECT;
		const static char *REQ_QUIT_NOT_SAVED;
		const static char *REQ_QUIT;
		const static char *STATUS_RECORDING;
		const static char *STATUS_ACTIVE_WAITING;
		const static char *STATUS_ACTIVE;
		const static char *STATUS_NOT_ACTIVE_RECORDING;
		const static char *STATUS_NOT_ACTIVE;
		const static char *STATUS_SHUTTING_DOWN;

		// MIXER WINDOW
		const static char *MASTER_GAD;
		const static char *STATUS_GAD;
		const static char *PLAYER_GAD;
		const static char *MASTER_VOLUME_HINT;
		const static char *MASTER_METER_HINT;
		const static char *ACTIVATE_HINT;
		const static char *DEACTIVATE_HINT;
		const static char *MIXDOWN_HINT;
		const static char *CHANNEL_METER_HINT;
		const static char *CHANNEL_VOLUME_HINT;
		const static char *CHANNEL_PANORAMA_HINT;
		const static char *CHANNEL_EDIT_HINT;
		const static char *REGISTERED_FOR;
		const static char *ASL_MIXDOWN;
	
		// INSTRUMENT WINDOW
		const static char *SAMPLE_NAME_GAD;
		const static char *SORT_BY_BASE_KEY_GAD;
		const static char *READ_BASE_KEYS_FROM_NAMES_GAD;
		const static char *SPREAD_BASE_KEYS_GAD;
		const static char *SET_KEY_RANGES_AROUND_BASE_KEYS_GAD;
		const static char *SHIFT_KEYS_GAD;
		const static char *INSTRUMENT_NAME_GAD;
		const static char *SETTINGS_GAD;
		const static char *VOICES_GAD;
		const static char *AMPLIFIER_ENVELOPE_GAD;
		const static char *ATTACK_TIME_GAD;
		const static char *DECAY_TIME_GAD;
		const static char *SUSTAIN_LEVEL_GAD;
		const static char *RELEASE_TIME_GAD;
		const static char *ZONES_GAD;
		const static char *TOOLS_GAD;
		const static char *ZONE_SETTINGS_GAD;
		const static char *ASL_CHOOSE_SAMPLE_FILE;
		const static char *SAMPLE_FILE_GAD;
		const static char *START_END_GAD;
		const static char *BASE_KEY_GAD;
		const static char *KEY_RANGE_GAD;
		const static char *VELO_RANGE_GAD;
		const static char *TUNE_GAD;
		const static char *FINE_TUNE_GAD;
		const static char *PANORAMA_GAD;
		const static char *ADD_HINT;
		const static char *REMOVE_HINT;
		const static char *MOVE_UP_HINT;
		const static char *MOVE_DOWN_HINT;
		const static char *PLAY_SAMPLE_HINT;
		const static char *PLAY_TONE_HINT;
		const static char *INSTRUMENT_MENU;
		const static char *INSTRUMENT_NEW_MENU;
		const static char *INSTRUMENT_LOAD_MENU;
		const static char *INSTRUMENT_IMPORT_EXS24_MENU;
		const static char *INSTRUMENT_SAVE_MENU;
		const static char *INSTRUMENT_SAVE_AS_MENU;
		const static char *INSTRUMENT;
		const static char *NO_SAMPLE_GAD;
		const static char *ASL_LOAD_SAMPLER_INSTRUMENT;
		const static char *ASL_SAVE_SAMPLER_INSTRUMENT;
		const static char *ASL_CHOOSE_EXS24_INSTRUMENT;
		const static char *ASL_CHOOSE_SAMPLES;
		const static char *REQ_WHICH_KEYS_FOR_BASE_KEYS;
		const static char *REQ_WHITE_ONLY_WHITE_AND_BLACK;
		const static char *REQ_ZONES_MUST_BE_SORTED;
		const static char *REQ_SORT_BEFORE_CANCEL;
		const static char *REQ_HOW_MANY_OCTAVES;

		// SETTINGS WINDOW
		const static char *MIDI_INPUT_GAD;
		const static char *REFRESH_PORT_LIST_GAD;
		const static char *CHANGES_AFTER_PLAYER_RESTART_GAD;
		const static char *AUDIO_OUTPUT_GAD;
		const static char *SELECT_AUDIO_OUTPUT_GAD;
		const static char *AUDIO_MODE_GAD;
		const static char *MIX_FREQUENCY_GAD;
		const static char *PLAYER_TITLE_GAD;
		const static char *BUFFER_SIZE_GAD;
		const static char *MAX_VOICES_GAD;
		const static char *PATHS_TITLE_GAD;
		const static char *PATH_PROJECTS_GAD;
		const static char *PATH_INSTRUMENTS_GAD;
		const static char *PATH_SAMPLES_GAD;
		const static char *PATH_IMPORTS_GAD;
		const static char *MISCELLANEOUS_GAD;
		const static char *ACTIVATE_PLAYER_ON_START_GAD;
		const static char *MIDI_GAD;
		const static char *AUDIO_GAD;
		const static char *SAMPLER_GAD;
		const static char *APPLICATION_GAD;
		const static char *SAVES_WINDOW_POSITIONS_GAD;
		const static char *SAVE_GAD;
		const static char *USE_GAD;
		const static char *CANCEL_GAD;
		const static char *SETTINGS;

		// IO UTILS
		const static char *REQ_SELECT_MANUALLY_1;
		const static char *REQ_SELECT_MANUALLY_2;
		const static char *REQ_SELECT_MANUALLY_3;
		const static char *ASL_IO_SELECT_SAMPLE_FILE;

	protected:
		static Catalog *mCatalog;

};

#endif

