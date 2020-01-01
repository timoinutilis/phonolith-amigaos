#ifndef _IOIMPORTEREXS24_
#define _IOIMPORTEREXS24_

#include <fstream>
#include <string>

#include "CoreInstrument.h"

class IOImporterEXS24
{
	public:
		static void import(char *fileName, CoreInstrument *samplerUnit);

	protected:
		const static long ID_EOF  = 0;
		const static long ID_INSTRUMENT = 0x00010000;
		const static long ID_ZONE       = 0x00010001;
		const static long ID_GROUP      = 0x00010002;
		const static long ID_SAMPLE     = 0x00010003;
		const static long ID_PARAM      = 0x00010004;

		const static long AIFF      = 0x41494646;
		const static long WAVE      = 0x57415645;

		static bool mIntelMode;

		static long getNextChunkID(ifstream &ifile);
		static void skipChunk(ifstream &ifile);
		static unsigned long *readChunk(ifstream &ifile);
		static string convertPath(char *path);

		static short getShort(short value);
		static long getLong(long value);

};

#endif


