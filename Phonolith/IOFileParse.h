#ifndef _IOFILEPARSE_
#define _IOFILEPARSE_

#include <fstream>

class IOFileParse
{
	public:
		//modes
		const static int LOAD = 0;
		const static int SAVE = 1;

		//open/close file
		IOFileParse(const char *filename, int mode, unsigned long formatID, unsigned short version);
		~IOFileParse();

		//save mode
		void addTagShort(unsigned short tagID, short value);
		void addTagLong(unsigned short tagID, long value);
		void addTagString(unsigned short tagID, const char *value);
		void addTagData(unsigned short tagID, unsigned long length, char *data);
		void writeArea(unsigned long areaID);

		//load mode
		const static unsigned long END_OF_FILE = 0xFFFFFFFF;
		const static unsigned short END_OF_AREA = 0xFFFF;

		bool isNewerVersion();
		unsigned long readNextArea();
		unsigned short readNextTag(long *value, unsigned long *length);

	protected:
		const static int BUFFER_SIZE_STEP = 8 * 1024;
		int mMode;
		ofstream *mOutStream;
		ifstream *mInStream;
		char *mBuffer;
		unsigned long mBufferSize;
		unsigned long mPosition;
		unsigned short mVersion;
		unsigned short mFileVersion;

};

#endif


