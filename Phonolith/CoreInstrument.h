#ifndef _COREINSTRUMENT_
#define _COREINSTRUMENT_

#include <string>

#include "CoreSound.h"
#include "Vector.h"

#include "CoreEnvelope.h"

struct Zone
{
	string name;
	CoreSound *sound;
	char lowKey;
	char highKey;
	char baseKey;
	char lowVelocity;
	char highVelocity;
	signed char mainTune;
	signed char fineTune;
	signed char panning;
	unsigned long startPosition; //includes frame
	unsigned long endPosition; //excludes frame
};

class CoreInstrument
{
	public:
		CoreInstrument();
		~CoreInstrument();

		void reinit();

		void saveInstrument(char *fileName);
		void loadInstrument(char *fileName);

		void addZone(const char *name, const char *soundFileName, char lowKey, char highKey, char baseKey, char lowVelocity, char highVelocity, signed char mainTune, signed char fineTune, signed char panning);
		void setZonePositions(int index, unsigned long start, unsigned long end);
		void deleteZone(int index);
		void deleteAllZones();
		Zone *getZone(int index);
		int getNumOfZones();
		bool moveZoneUp(int zoneIndex);
		bool moveZoneDown(int zoneIndex);

		void changeZoneSound(int zoneIndex, const char *name, const char *soundFileName);

		void zoneSortByBaseKey();
		void zoneReadBaseKeysFromNames();
		void zoneSpreadBaseKeys(bool onlyOnWhiteKeys);
		void zoneSetKeyRangesFromBaseKeys();
		void zoneShiftKeys(int keys);
		bool zoneCheckBaseKeySorting();

		CoreEnvelope mAmpEnvelope;

		int mNumVoices;

		string mName;

	protected:
		int readKeyFromName(const char *name);

		Vector *mZones;

};

#endif

