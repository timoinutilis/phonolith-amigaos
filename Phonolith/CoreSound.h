#ifndef _CORESOUND_
#define _CORESOUND_

#include <string>
#include <fstream>


class CoreSound
{
	public:
		CoreSound(const char *filename);
		~CoreSound();

		char *getFileName();

		int mNumChannels;
		int mNumBytesPerSample;
		unsigned long mNumSampleFrames;

		long mSampleRate;

		bool mLoopActive;
		unsigned long mLoopBeginFrame;
		unsigned long mLoopEndFrame;

		char *mData;

	protected:
		string mFileName;

		void loadAIFF(ifstream &ifile);
		void loadWAVE(ifstream &ifile);

		const static int END_OF_FILE = 0;

		const static int FORM = 0x464F524D;
		const static int AIFF = 0x41494646;
		const static int AIFF_COMM = 0x434F4D4D;
		const static int AIFF_MARK = 0x4D41524B;
		const static int AIFF_INST = 0x494E5354;
		const static int AIFF_SSND = 0x53534E44;

		bool checkIFF(ifstream &ifile, long formatID);
		long getNextChunkID(ifstream &ifile);
		void skipChunk(ifstream &ifile);
		char *readChunk(ifstream &ifile);

		float extendedToFloat(char *extended);

		const static int RIFF = 0x52494646;
		const static int WAVE = 0x57415645;
		const static int WAVE_FMT  = 0x666D7420;
		const static int WAVE_DATA = 0x64617461;

		bool checkRIFF(ifstream &ifile, long formatID);
		long getNextRChunkID(ifstream &ifile);
		void skipRChunk(ifstream &ifile);
		char *readRChunk(ifstream &ifile);
		short intelShort(short value);
		long intelLong(long value);

};

#endif

