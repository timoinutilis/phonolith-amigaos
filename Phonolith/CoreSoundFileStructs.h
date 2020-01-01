
#pragma pack(1)

struct AIFFCommon
{
	short numChannels;
	unsigned long numSampleFrames;
	short sampleSize;
	char sampleRate[10];
};

struct Loop
{
	short PlayMode;
	short beginLoop;
	short endLoop;
};

struct AIFFInstrument
{
	char baseNote;
	char detune;
	char lowNote;
	char highNote;
	char lowvelocity;
	char highvelocity;
	short gain;
	Loop sustainLoop;
	Loop releaseLoop;
};

struct WAVEFormat
{
	short compressionCode;
	short numberOfChannels;
	long sampleRate;
	long averageBytesPerSecond;
	short blockAlign;
	short significantBitsPerSample;
};

#pragma pack()

