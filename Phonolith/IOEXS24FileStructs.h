#pragma pack(1)

struct EXS24Instrument
{
	long unknown1;
	long unknown2;
	long sobt;
	char name[64]; //3
	long unknown3;
	long numZones; //20
	long unknown4;
	long unknown5;
	long unknown6;
	long unknown7;
	long unknown8;
	long unknown9;
	long unknown10;
	long unknown11;
};

struct EXS24Zone
{
	long unknown1;
	long unknown2;
	long sobt;
	char name[64]; //3

	char unknown3; //19
	char keyBase;
	char fineTune;
	char unknown4;

	short unknown5; //20
	char keyLow;
	char keyHigh;

	long unknown6;
	long startPosition; //22 check
	long endPosition; //23 check
	long unknown9;
	long unknown10; //25
	long unknown11;
	long unknown12;
	long unknown13;
	long unknown14;
	long unknown15;
	long unknown16;
	long unknown17;
	long unknown18;
	long unknown19;
	long unknown20;
	long unknown21;
	long unknown22;
	long unknown23;

	char roughTune; //39
	char unknown24;
	short unknown25;

	long unknown26;
	long unknown27;
	long sampleNumber; //42
	long unknown29;
	long unknown30;
};

struct EXS24Sample
{
	long unknown1;
	long unknown2;
	long sobt;
	char name[64]; //3
	long unknown3; //19
	long unknown4;
	long unknown5;
	long unknown6;
	long unknown7;
	long unknown8;
	long unknown9;
	long fileFormat; //26
	long unknown10;
	long unknown11;
	long unknown12;
	long unknown13;
	long unknown14;
	long unknown15;
	long unknown16;
	long unknown17;
	long unknown18;
	long unknown19;
	long unknown20;
	long unknown21;
	char path[256]; //39
};

#pragma pack()

