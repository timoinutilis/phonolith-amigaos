#define PROJECTID 0x5050524A //PPRJ
#define VERSION 0x0001

#define AREA_MASTER          0x4D415354 //MAST
#define AREA_CHANNEL         0x4348414E //CHAN


// TAGS
// xxx0: WORD value
// xxx1: LONG value
// xxx2: WORD len string
// xxx3: LONG len data

// AREA_MASTER
#define TAG_MASTER_VOLUME 0x0000

// AREA_CHANNEL
#define TAG_CHANNEL_INDEX 0x0000
#define TAG_CHANNEL_VOLUME 0x0010
#define TAG_CHANNEL_PANORAMA 0x0020

