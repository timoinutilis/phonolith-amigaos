#ifndef _BOOPSITEXTLABEL_
#define _BOOPSITEXTLABEL_

#include <utility/tagitem.h>
#include <intuition/classes.h>

//Attributes
#define TEXTLABEL_Text     TAG_USER+1

void TEXTLABEL_MakeBoopsiClass();
void TEXTLABEL_FreeBoopsiClass();
Class *TEXTLABEL_GetClass();


#endif


