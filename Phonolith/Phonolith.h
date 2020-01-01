#ifndef _PHONOLITH_
#define _PHONOLITH_

#include <string>

#include "FrameApp.h"
#include "FrameGui.h"
#include "WinInstrument.h"
#include "GuiSettings.h"
#include "GuiMixer.h"
#include "CoreProject.h"
#include "CoreSampler.h"
#include "CoreInstrument.h"
#include "FileRequest.h"
#include "CorePrefs.h"

#include <libraries/gadtools.h>

class CoreSampler;

class Phonolith : public FrameApp
{
	public:
		Phonolith();
		~Phonolith();

		enum menuid {
			MENU_ABOUT = 1,
			MENU_SETTINGS,
			MENU_QUIT,
			MENU_PROJECT_NEW,
			MENU_PROJECT_LOAD,
			MENU_PROJECT_SAVE,
			MENU_PROJECT_SAVEAS
		};

		static void addDefaultMenu(FrameGui *gui);

		void menuPicked(int menuID);
		void quitRequested();
		void openProject(const char *fileName);

		void instrumentWindowClosed(int unitIndex);
		void showInstrumentWindow(int unitIndex, bool forceNewWindow);

		void settingsWindowClosed();
		void showSettingsWindow();

		void updateMixer();
		void updateMixerLabels();
		void updateStatus();

		bool activatePlayer();
		void deactivatePlayer();

		static void loadImages();
		static void freeImages();

		CoreSampler *mSampler;
		FileRequest *mReqProject;
		CorePrefs *mPrefs;
		CoreProject *mProject;

		string mUserName;
		bool mRegistered;

		static Object *mImagePlayerOn;
		static Object *mImagePlayerOff;
		static Object *mImagePlayerMixdown;
		static Object *mImageAdd;
		static Object *mImageRemove;
		static Object *mImageUp;
		static Object *mImageDown;
		static Object *mImagePlay;
		static Object *mImageBanner;
		static Object *mImageAdsr;

	protected:
		WinInstrument *mInstrumentWins[CoreProject::CHANNELS];
		GuiSettings *mSettingsGui;
		GuiMixer *mMixerGui;

};

#endif

