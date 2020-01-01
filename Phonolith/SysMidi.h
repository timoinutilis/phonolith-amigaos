#ifndef _SYSMIDI_
#define _SYSMIDI_

#include <midi/mididefs.h>
#include <midi/camd.h>

#include "Vector.h"

class SysMidi
{
	public:
		SysMidi();
		~SysMidi();

		bool openMidi(const char *portName, int signal);
		void closeMidi();

		static void openLib();
		static void closeLib();

		bool getEvent(char *status, char *channel, char *data1, char *data2);

		static Vector *getPorts();
		static void freePortsVector(Vector *ports);


	protected:
		MidiNode    *mMidi;
		MidiLink    *mMidiIn;
};

#endif


