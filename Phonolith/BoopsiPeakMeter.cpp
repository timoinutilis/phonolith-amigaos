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
#include <images/bitmap.h>

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/space.h>
#include <proto/utility.h>
#include <proto/bitmap.h>

#include "BoopsiPeakMeter.h"

//=====Boopsi Object Data=====
struct PeakMeterData
{
	short peakL;
	short peakR;
	bool clippedL;
	bool clippedR;
	uint32 left;
	uint32 top;
	int lastHeightL;
	int lastHeightR;
	bool lastClippingL;
	bool lastClippingR;
};

const static int MAX_VALUE = 32767;

//=====Prototypes=====
uint32 PEAKMETER_Dispatcher(Class *cl, Object *o, Msg msg);
void PEAKMETER_Render(RastPort *rp, int type, PeakMeterData *data);

//==================================================================================
// BOOPSI
//==================================================================================

IClass *peakmeter_BoopsiClass = NULL;
Screen *peakmeter_Screen = NULL;
Object *peakmeter_ImageMeterOn = NULL;
Object *peakmeter_ImageMeterOff = NULL;
Object *peakmeter_ImageClippingOn = NULL;
Object *peakmeter_ImageClippingOff = NULL;
BitMap *peakmeter_BitMapMeterOn = NULL;
BitMap *peakmeter_BitMapMeterOff = NULL;

uint32 peakmeter_imagesWidth = 0;
uint32 peakmeter_clippingHeight = 0;
uint32 peakmeter_meterHeight = 0;

//----------------------------------------------------------------------------------

void disposeImages()
{
	if (peakmeter_ImageMeterOn) IIntuition->DisposeObject(peakmeter_ImageMeterOn);
	if (peakmeter_ImageMeterOff) IIntuition->DisposeObject(peakmeter_ImageMeterOff);
	if (peakmeter_ImageClippingOn) IIntuition->DisposeObject(peakmeter_ImageClippingOn);
	if (peakmeter_ImageClippingOff) IIntuition->DisposeObject(peakmeter_ImageClippingOff);

	peakmeter_ImageMeterOn = NULL;
	peakmeter_ImageMeterOff = NULL;
	peakmeter_ImageClippingOn = NULL;
	peakmeter_ImageClippingOff = NULL;
	peakmeter_BitMapMeterOn = NULL;
	peakmeter_BitMapMeterOff = NULL;
}

//----------------------------------------------------------------------------------

void createImages()
{
	peakmeter_ImageMeterOn = (Object *) IIntuition->NewObject(IBitMap->BITMAP_GetClass(), NULL,
		BITMAP_Screen, peakmeter_Screen,
		BITMAP_SourceFile, "PROGDIR:Images/meter_on",
		TAG_DONE);

	peakmeter_ImageMeterOff = (Object *) IIntuition->NewObject(IBitMap->BITMAP_GetClass(), NULL,
		BITMAP_Screen, peakmeter_Screen,
		BITMAP_SourceFile, "PROGDIR:Images/meter_off",
		TAG_DONE);

	peakmeter_ImageClippingOn = (Object *) IIntuition->NewObject(IBitMap->BITMAP_GetClass(), NULL,
		BITMAP_Screen, peakmeter_Screen,
		BITMAP_SourceFile, "PROGDIR:Images/clipping_on",
		TAG_DONE);

	peakmeter_ImageClippingOff = (Object *) IIntuition->NewObject(IBitMap->BITMAP_GetClass(), NULL,
		BITMAP_Screen, peakmeter_Screen,
		BITMAP_SourceFile, "PROGDIR:Images/clipping_off",
		TAG_DONE);

	if (!peakmeter_ImageMeterOn || !peakmeter_ImageMeterOff || !peakmeter_ImageClippingOn || !peakmeter_ImageClippingOff)
	{
		throw runtime_error("Error while opening PeakMeter images");
	}

	IIntuition->GetAttr(BITMAP_Width, peakmeter_ImageClippingOn, (ULONG *) &peakmeter_imagesWidth);
	IIntuition->GetAttr(BITMAP_Height, peakmeter_ImageClippingOn, (ULONG *) &peakmeter_clippingHeight);
	IIntuition->GetAttr(BITMAP_Height, peakmeter_ImageMeterOn, (ULONG *) &peakmeter_meterHeight);

	IIntuition->GetAttr(BITMAP_BitMap, peakmeter_ImageMeterOn, (ULONG *) &peakmeter_BitMapMeterOn);
	IIntuition->GetAttr(BITMAP_BitMap, peakmeter_ImageMeterOff, (ULONG *) &peakmeter_BitMapMeterOff);

}

//----------------------------------------------------------------------------------

void PEAKMETER_MakeBoopsiClass()
{
	if (!peakmeter_BoopsiClass)
	{
		peakmeter_BoopsiClass = IIntuition->MakeClass(NULL, NULL, ISpace->SPACE_GetClass(), sizeof( PeakMeterData ), 0);
		if (!peakmeter_BoopsiClass)
		{
			throw runtime_error("Could not make PeakMeter class");
		}

		peakmeter_BoopsiClass->cl_Dispatcher.h_SubEntry = (HOOKFUNC) PEAKMETER_Dispatcher;
	}
}

//----------------------------------------------------------------------------------

void PEAKMETER_FreeBoopsiClass()
{
	if (peakmeter_BoopsiClass)
	{
		IIntuition->FreeClass(peakmeter_BoopsiClass);
		peakmeter_BoopsiClass = NULL;
		peakmeter_Screen = NULL;
		disposeImages();
	}
}

//----------------------------------------------------------------------------------

Class *PEAKMETER_GetClass()
{
	return (Class *) peakmeter_BoopsiClass;
}

//----------------------------------------------------------------------------------

uint32 PEAKMETER_Dispatcher(Class *cl, Object *o, Msg msg)
{
	uint32 retval = GMR_MEACTIVE;
	PeakMeterData *peakMeterData = (PeakMeterData *) INST_DATA(cl, o);

	switch (msg->MethodID)
	{
		case OM_NEW: {
			Object *newObject = NULL;
			if (newObject = (Object *) IIntuition->IDoSuperMethodA(cl, o, msg))
			{

				opSet *set = (opSet *) msg;
				peakMeterData = (PeakMeterData *) INST_DATA(cl, newObject);

				peakMeterData->peakL = 0;
				peakMeterData->peakR = 0;
				peakMeterData->clippedL = false;
				peakMeterData->clippedR = false;
				peakMeterData->lastHeightL = 0;
				peakMeterData->lastHeightR = 0;
				peakMeterData->lastClippingL = false;
				peakMeterData->lastClippingR = false;
				
				Screen *screen = (Screen *) IUtility->GetTagData(PEAKMETER_Screen, NULL, set->ops_AttrList);
				if (screen != peakmeter_Screen)
				{
					disposeImages();
					peakmeter_Screen = screen;
					createImages();
				}


			}
			retval = (uint32) newObject;

		} break;

		case OM_DISPOSE: {
			retval = IIntuition->IDoSuperMethodA(cl, o, msg);
		} break;

		case GM_DOMAIN: {
            retval = IIntuition->IDoSuperMethodA(cl, o, msg);
			if (peakMeterData)
			{
				gpDomain *domain = (gpDomain *) msg;

				if (domain->gpd_Which == GDOMAIN_MINIMUM)
				{
					domain->gpd_Domain.Width = peakmeter_imagesWidth * 2 + 1;
					domain->gpd_Domain.Height = peakmeter_clippingHeight + 1 + peakmeter_meterHeight;
				}
			}
		} break;

	    case GM_LAYOUT: {
		    retval = IIntuition->IDoSuperMethodA(cl, o, msg);

			IIntuition->GetAttrs(o,
					GA_Top, &peakMeterData->top,
					GA_Left, &peakMeterData->left,
					TAG_DONE);

		} break;

		case GM_RENDER: {
			PEAKMETER_Render(((gpRender *) msg)->gpr_RPort, ((gpRender *) msg)->gpr_Redraw, peakMeterData);

			retval = 0;
		} break;

		case OM_GET: {
			opGet *get = (opGet *) msg;

			retval = 1;
			switch (get->opg_AttrID)
			{
				case PEAKMETER_PeakLeft:
					*get->opg_Storage = (ULONG) peakMeterData->peakL;
				break;
				case PEAKMETER_PeakRight:
					*get->opg_Storage = (ULONG) peakMeterData->peakR;
				break;
				default:
					retval = IIntuition->IDoSuperMethodA(cl, o, msg);
			}
		} break;

		case OM_SET:
		case OM_UPDATE: {
			opUpdate *update = (opUpdate *) msg;
			TagItem *attr = update->opu_AttrList;

            retval = IIntuition->IDoSuperMethodA(cl, o, msg);

			while (attr->ti_Tag != TAG_DONE)
			{
				switch (attr->ti_Tag)
				{
					case PEAKMETER_PeakLeft: {
						if (peakMeterData->peakL < attr->ti_Data)
						{
							peakMeterData->peakL = (unsigned short) attr->ti_Data;
							if (peakMeterData->peakL == MAX_VALUE)
							{
								peakMeterData->clippedL = true;
							}
						}
					} break;

					case PEAKMETER_PeakRight: {
						if (peakMeterData->peakR < attr->ti_Data)
						{
							peakMeterData->peakR = (unsigned short) attr->ti_Data;
							if (peakMeterData->peakR == MAX_VALUE)
							{
								peakMeterData->clippedR = true;
							}
						}
					} break;

					case PEAKMETER_Process: {
						peakMeterData->peakL -= attr->ti_Data;
						if (peakMeterData->peakL < 0)
						{
							peakMeterData->peakL = 0;
						}

						peakMeterData->peakR -= attr->ti_Data;
						if (peakMeterData->peakR < 0)
						{
							peakMeterData->peakR = 0;
						}
					} break;
				}
				attr++;
			}

			retval = 1;
			IIntuition->DoRender(o, update->opu_GInfo, GREDRAW_UPDATE);
		} break;

		case GM_GOACTIVE:
		case GM_HANDLEINPUT: {
		    retval = IIntuition->IDoSuperMethodA(cl, o, msg);

			gpInput *input = (gpInput *) msg;
			InputEvent *event = input->gpi_IEvent;

			if (input->gpi_IEvent->ie_Class == IECLASS_RAWMOUSE)
			{
				if (input->gpi_IEvent->ie_Code == SELECTDOWN)
				{
					peakMeterData->clippedL = false;
					peakMeterData->clippedR = false;
					IIntuition->DoRender(o, input->gpi_GInfo, GREDRAW_UPDATE);
				}
			}
		} break;

		default:
			retval = IIntuition->IDoSuperMethodA(cl, o, msg);
	}

	return retval;
}

void renderMeter(RastPort *rp, int x, int y, int peak, int &lastHeight, int type)
{
	int height = (peak * peakmeter_meterHeight / MAX_VALUE + 3) >> 2 << 2;

	if (lastHeight > height || type == GREDRAW_REDRAW)
	{
		IGraphics->BltBitMapRastPort(peakmeter_BitMapMeterOff, 0, 0,
				rp, x, y,
				peakmeter_imagesWidth, peakmeter_meterHeight - height, 0xC0);
	}

	if (lastHeight < height || type == GREDRAW_REDRAW)
	{
		IGraphics->BltBitMapRastPort(peakmeter_BitMapMeterOn, 0, peakmeter_meterHeight - height,
				rp, x, y + peakmeter_meterHeight - height,
				peakmeter_imagesWidth, height, 0xC0);
	}

	lastHeight = height;
}

void PEAKMETER_Render(RastPort *rp, int type, PeakMeterData *data)
{
	if (data->lastClippingL != data->clippedL || type == GREDRAW_REDRAW)
	{
		IIntuition->DrawImage(rp, (Image *) (data->clippedL ? peakmeter_ImageClippingOn : peakmeter_ImageClippingOff), data->left, data->top);
		data->lastClippingL = data->clippedL;
	}

	renderMeter(rp, data->left, data->top + peakmeter_clippingHeight + 1, data->peakL, data->lastHeightL, type);

	if (data->lastClippingR != data->clippedR || type == GREDRAW_REDRAW)
	{
		IIntuition->DrawImage(rp, (Image *) (data->clippedR ? peakmeter_ImageClippingOn : peakmeter_ImageClippingOff),
				data->left + peakmeter_imagesWidth + 1, data->top);
		data->lastClippingR = data->clippedR;
	}

	renderMeter(rp, data->left + peakmeter_imagesWidth + 1, data->top + peakmeter_clippingHeight + 1, data->peakR, data->lastHeightR, type);
}

