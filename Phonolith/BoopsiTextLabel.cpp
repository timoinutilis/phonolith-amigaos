/*
Phonolith Software Sampler
Copyright (C) 2006-2008 Timo Kloss

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include <iostream>
#include <stdexcept>

using namespace std;

#include <gadgets/space.h>
#include <graphics/gfx.h>
#include <intuition/intuition.h>
#include <intuition/gadgetclass.h>
#include <utility/hooks.h>

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/space.h>
#include <proto/utility.h>

#include "BoopsiTextLabel.h"

//=====Boopsi Object Data=====
struct TextLabelData
{
	uint32 left;
	uint32 top;
	uint32 width;
	uint32 height;
	char *text;
	int textLength;
};

//=====Prototypes=====
uint32 TEXTLABEL_Dispatcher(Class *cl, Object *o, Msg msg);
void TEXTLABEL_Render(RastPort *rp, int type, TextLabelData *data);
void TEXTLABEL_SetText(const char *text, TextLabelData *data);

//==================================================================================
// BOOPSI
//==================================================================================

IClass *textlabel_BoopsiClass = NULL;

//----------------------------------------------------------------------------------

void TEXTLABEL_MakeBoopsiClass()
{
	if (!textlabel_BoopsiClass)
	{
		textlabel_BoopsiClass = IIntuition->MakeClass(NULL, NULL, ISpace->SPACE_GetClass(), sizeof( TextLabelData ), 0);
		if (!textlabel_BoopsiClass)
		{
			throw runtime_error("Could not make TextLabel class");
		}

		textlabel_BoopsiClass->cl_Dispatcher.h_SubEntry = (HOOKFUNC) TEXTLABEL_Dispatcher;
	}
}

//----------------------------------------------------------------------------------

void TEXTLABEL_FreeBoopsiClass()
{
	if (textlabel_BoopsiClass)
	{
		IIntuition->FreeClass(textlabel_BoopsiClass);
		textlabel_BoopsiClass = NULL;
	}
}

//----------------------------------------------------------------------------------

Class *TEXTLABEL_GetClass()
{
	return (Class *) textlabel_BoopsiClass;
}

//----------------------------------------------------------------------------------

uint32 TEXTLABEL_Dispatcher(Class *cl, Object *o, Msg msg)
{
	uint32 retval = GMR_MEACTIVE;
	TextLabelData *textLabelData = (TextLabelData *) INST_DATA(cl, o);

	switch (msg->MethodID)
	{
		case OM_NEW: {
			Object *newObject = NULL;
			if (newObject = (Object *) IIntuition->IDoSuperMethodA(cl, o, msg))
			{

				opSet *set = (opSet *) msg;
				textLabelData = (TextLabelData *) INST_DATA(cl, newObject);

				textLabelData->text = NULL;
				textLabelData->left = 0;
				textLabelData->top = 0;
				textLabelData->width = 0;
				textLabelData->height = 0;
				textLabelData->textLength = 0;

				TEXTLABEL_SetText((const char *) IUtility->GetTagData(TEXTLABEL_Text, NULL, set->ops_AttrList), textLabelData);

			}
			retval = (uint32) newObject;

		} break;

		case OM_DISPOSE: {
			if (textLabelData->text)
			{
				delete[] textLabelData->text;
			}
			retval = IIntuition->IDoSuperMethodA(cl, o, msg);
		} break;

		case GM_DOMAIN: {
            retval = IIntuition->IDoSuperMethodA(cl, o, msg);
			if (textLabelData)
			{
				gpDomain *domain = (gpDomain *) msg;

				if (domain->gpd_Which == GDOMAIN_MINIMUM)
				{
					domain->gpd_Domain.Width = 1;
					domain->gpd_Domain.Height = domain->gpd_RPort->TxHeight;
				}
			}
		} break;

	    case GM_LAYOUT: {
		    retval = IIntuition->IDoSuperMethodA(cl, o, msg);

			IIntuition->GetAttrs(o,
					GA_Top, &textLabelData->top,
					GA_Left, &textLabelData->left,
					GA_Width, &textLabelData->width,
					GA_Height, &textLabelData->height,
					TAG_DONE);
		} break;

		case GM_RENDER: {
			TEXTLABEL_Render(((gpRender *) msg)->gpr_RPort, ((gpRender *) msg)->gpr_Redraw, textLabelData);

			retval = 0;
		} break;

		case OM_SET:
		case OM_UPDATE: {
			opUpdate *update = (opUpdate *) msg;

            retval = IIntuition->IDoSuperMethodA(cl, o, msg);

			const char *newtext = (const char *) IUtility->GetTagData(TEXTLABEL_Text, NULL, update->opu_AttrList);
			if (newtext)
			{
				TEXTLABEL_SetText(newtext, textLabelData);
			}

			IIntuition->DoRender(o, update->opu_GInfo, GREDRAW_UPDATE);
			retval = 1;
		} break;

		default:
			retval = IIntuition->IDoSuperMethodA(cl, o, msg);
	}

	return retval;
}

void TEXTLABEL_Render(RastPort *rp, int type, TextLabelData *data)
{
	IGraphics->SetAPen(rp, 2);
	IGraphics->RectFill(rp, data->left, data->top, data->left + data->width - 1, data->top + data->height - 1);
	if (data->textLength)
	{
		IGraphics->SetAPen(rp, 1);
		IGraphics->SetDrMd(rp, JAM1);
		IGraphics->Move(rp, data->left, data->top + rp->TxBaseline);

		struct TextExtent textExtent;
		int maxChars = IGraphics->TextFit(rp, data->text, data->textLength, &textExtent, NULL, 1, data->width, 100);
		IGraphics->Text(rp, data->text, maxChars);
	}
}

void TEXTLABEL_SetText(const char *text, TextLabelData *data)
{
	if (data->text)
	{
		delete[] data->text;
		data->text = NULL;
		data->textLength = 0;
	}
	if (text)
	{
		data->textLength = strlen(text);
		data->text = new char[data->textLength + 1];
		strcpy(data->text, text);
	}
}

