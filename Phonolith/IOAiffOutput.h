#ifndef _IOAIFFOUTPUT_
#define _IOAIFFOUTPUT_

#include <dos/dos.h>

class IOAiffOutput
{
	public:
		static void openLib();
		static void closeLib();

		IOAiffOutput();
		~IOAiffOutput();

		bool openFile(const char *fileName, int channels, int bytesPerSample, unsigned long frequency);
		bool closeFile();
		bool isOpen();

		bool writeData(void *buffer, unsigned long frames);

	protected:
		const static int I_FORM = 0x464F524D;
		const static int I_AIFF = 0x41494646;
		const static int I_COMM = 0x434F4D4D;
		const static int I_SSND = 0x53534E44;

		BPTR mFile;

		unsigned long mFrames;

		int mChannels;
		int mBytesPerSample;
		unsigned long mFrequency;

		void floatToExtended(float value, char *extended);

};

#endif

