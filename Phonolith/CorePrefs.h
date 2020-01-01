#ifndef _COREPREFS_
#define _COREPREFS_

#include <string>

#include "FrameApp.h"

class CorePrefs
{
	public:
		CorePrefs();
		~CorePrefs();

		void getFrom(CorePrefs *other);
		void load(FrameApp *app);
		void save(FrameApp *app);

		//MIDI
		const static char *DEFAULT_CLUSTER;
		string mMidiClusterName;

		//AUDIO
		unsigned long mAudioModeID;
		string mAudioModeName;
		unsigned long mAudioFrequency;
		short mPlayerBufferMs;

		//SAMPLER
		short mSamplerMaxVoices;

		//APPLICATION
		string mDirProjects;
		string mDirInstruments;
		string mDirSamples;
		string mDirImports;
		bool   mActivatePlayer;
//		  bool mShowDockIcon;

	protected:

};

#endif

