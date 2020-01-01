#ifndef _COREPROJECT_
#define _COREPROJECT_

#include <string>

#include "CoreInstrument.h"

class CoreProject
{
	public:
		CoreProject();
		~CoreProject();

		void reinit();
		void saveProject(const char *name);
		void loadProject(const char *name);

		void setMasterVolume(int volume);
		void setChannelVolume(int channelIndex, int volume);
		void setChannelPanorama(int channelIndex, int panning);

		const static int CHANNELS = 16;

		CoreInstrument *mInstruments[CHANNELS];

		int	mMasterVolumeInt;
		float mMasterVolume;

		int mChannelVolumeInt[CHANNELS];
		float mChannelVolume[CHANNELS];

		int mChannelPanoramaInt[CHANNELS];
		float mChannelPanoramaAmpL[CHANNELS];
		float mChannelPanoramaAmpR[CHANNELS];

		int mChannelVoices[CHANNELS];

		bool mChanged;

		bool mRecordingRequested;
		bool mRecordingActive;
		string mRecordFileName;

	protected:

};

#endif

