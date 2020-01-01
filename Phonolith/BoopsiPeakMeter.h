#ifndef _BOOPSIPEAKMETER_
#define _BOOPSIPEAKMETER_

#include <utility/tagitem.h>
#include <intuition/classes.h>

//Attributes
#define PEAKMETER_PeakLeft     TAG_USER+1
#define PEAKMETER_PeakRight    TAG_USER+2
#define PEAKMETER_Process      TAG_USER+3
#define PEAKMETER_Screen       TAG_USER+4

void PEAKMETER_MakeBoopsiClass();
void PEAKMETER_FreeBoopsiClass();
Class *PEAKMETER_GetClass();


#endif


