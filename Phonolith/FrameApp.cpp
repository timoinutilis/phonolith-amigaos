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
#include <new>

using namespace std;

#include "FrameApp.h"
#include "FrameWindow.h"
#include "main.h"
#include "Version.h"

#include <proto/intuition.h>
#include <proto/exec.h>
#include <proto/window.h>
#include <proto/requester.h>
#include <proto/application.h>
#include <proto/utility.h>

#include <exec/exec.h>
#include <classes/requester.h>
#include <gadgets/layout.h>

#include <reaction/reaction_macros.h>


extern PrefsObjectsIFace *IPrefsObjects;

Hook FrameApp::mIDCMPHook;

//==================================================================================
// Constructor/Destructor
//==================================================================================

FrameApp::FrameApp(char *name)
{
	mName = name;
	mSigMask = 0;

	mIDCMPHook.h_MinNode.mln_Succ = NULL;
	mIDCMPHook.h_MinNode.mln_Pred = NULL;
	mIDCMPHook.h_SubEntry = NULL;
	mIDCMPHook.h_Entry = (HOOKFUNC)IDCMPFunc;
	mIDCMPHook.h_Data = NULL;

}

//----------------------------------------------------------------------------------

FrameApp::~FrameApp()
{
	//close all windows
	for (int i = 0; i < mFWindows.getSize(); i++)
	{
		FrameWindow *win = (FrameWindow *) mFWindows.getElement(i);
		delete win;
	}

	//delete window borders
	for (int i = 0; i < mWindowBorders.getSize(); i++)
	{
		WindowBorders *borders = (WindowBorders *) mWindowBorders.getElement(i);
		delete borders;
	}
}

//==================================================================================
// Window Manager
//==================================================================================

uint32 FrameApp::IDCMPFunc(Hook *hook, Object *window, IntuiMessage *msg)
{
	TagItem *tags = (TagItem *)msg->IAddress;

	long relVerify = IUtility->GetTagData(LAYOUT_RelVerify, 0, tags);
	long helpHit = IUtility->GetTagData(LAYOUT_HelpHit, 0, tags);

	if (!relVerify && !helpHit)
	{
		FrameWindow *win = NULL;
		IIntuition->GetAttr(WINDOW_UserData, window, (uint32 *) &win);
		if (win)
		{
			unsigned long gadgetID = IUtility->GetTagData(GA_ID, ~0, tags);
			win->gadgetPressed(gadgetID, (int)msg->Code, (short) msg->Qualifier, false);
		}
	}

	return 0;
}


//----------------------------------------------------------------------------------

void FrameApp::addWindow(FrameWindow *inWindow)
{
	mFWindows.addElement(inWindow);
	refreshSigMask();
}

//----------------------------------------------------------------------------------

void FrameApp::removeWindow(FrameWindow *inWindow)
{
	mFWindows.deleteElement(inWindow);
	refreshSigMask();
}

//----------------------------------------------------------------------------------

void FrameApp::refreshSigMask()
{
	mSigMask = 0;
	for (int i = 0; i < mFWindows.getSize(); i++)
	{
		FrameWindow *win = (FrameWindow *) mFWindows.getElement(i);
		mSigMask |= win->mFGui->mSignalMask;
	}
}

//----------------------------------------------------------------------------------

void FrameApp::checkWindows(unsigned long signals)
{
	unsigned short code;
	unsigned long result;
	long msgClass;

	FrameWindow *windowToClose = NULL;

	for (int i = 0; i < mFWindows.getSize(); i++)
	{
		FrameWindow *win = (FrameWindow *) mFWindows.getElement(i);

		if ((signals & win->mFGui->mSignalMask) != 0)
		{
			unsigned long qualifier = 0;
			IIntuition->GetAttr(WINDOW_Qualifier, win->mFGui->mWindowObject, &qualifier);

			while ((result = RA_HandleInput(win->mFGui->mWindowObject, &code)) != WMHI_LASTMSG)
			{
				msgClass = (result & WMHI_CLASSMASK);

				if (msgClass == WMHI_CLOSEWINDOW)
				{
					switch (win->closeRequested())
					{
						case FrameWindow::CLOSE_WINDOW: windowToClose = win; break;
						case FrameWindow::CLOSE_APP: quitRequested(); break;
					}
				}
				else if (msgClass == WMHI_MENUPICK)
				{
					int menuID = win->mFGui->getMenuItemID(code);
					if (menuID != -1)
					{
						menuPicked(menuID);
						win->menuPicked(menuID);
					}
				}
				else if (msgClass == WMHI_GADGETUP)
				{
					win->gadgetPressed(result & WMHI_GADGETMASK, code, (short) qualifier, true);
				}
				else if (msgClass == WMHI_ICONIFY)
				{
					hideApplication();
				}
				else if (msgClass == WMHI_UNICONIFY)
				{
					showApplication();
				}
			}
			if (win->mLeaveWindow)
			{
				windowToClose = win;
			}
		}
	}

	if (windowToClose)
	{
		removeWindow(windowToClose);
		delete windowToClose;
	}
}

//----------------------------------------------------------------------------------

void FrameApp::hideApplication()
{
	if (mFWindows.getSize() > 0)
	{
		FrameWindow *win = (FrameWindow *) mFWindows.getElement(0);
		if (main_hasDockIcon)
		{
            win->mFGui->closeWindow();
		}
		else
		{
			win->mFGui->iconifyWindow();
		}

		for (int i = 1; i < mFWindows.getSize(); i++)
		{
			win = (FrameWindow *) mFWindows.getElement(i);
			win->mFGui->closeWindow();
		}
	}
}

//----------------------------------------------------------------------------------

void FrameApp::showApplication()
{
	for (int i = 0; i < mFWindows.getSize(); i++)
	{
		FrameWindow *win = (FrameWindow *) mFWindows.getElement(i);
		win->mFGui->reopenWindow();
	}
}

//----------------------------------------------------------------------------------

void FrameApp::toFront()
{
	int num = mFWindows.getSize();
	for (int i = 0; i < num - 1; i++)
	{
		FrameWindow *win = (FrameWindow *) mFWindows.getElement(i);

		if (!win->mFGui->mWindow) // iconified?
		{
			win->mFGui->reopenWindow();
		}
		IIntuition->WindowToFront(win->mFGui->mWindow);
	}
		
	if (num > 0)
	{
		FrameWindow *win = (FrameWindow *) mFWindows.getElement(num - 1);
		win->mFGui->activate();
	}
}

//==================================================================================
// Window Size/Position Memory
//==================================================================================

void FrameApp::setWindowBorders(const char *idName, int left, int top, int width, int height)
{
	WindowBorders *borders = getWindowBorders(idName);
	if (!borders)
	{
		borders = new WindowBorders;
		borders->idName = idName;
		mWindowBorders.addElement(borders);
	}

	borders->left = left;
	borders->top = top;
	borders->width = width;
	borders->height = height;
}

//----------------------------------------------------------------------------------

WindowBorders *FrameApp::getWindowBorders(const char *idName)
{
	for (int i = 0; i < mWindowBorders.getSize(); i++)
	{
		WindowBorders *borders = (WindowBorders *) mWindowBorders.getElement(i);
		if (borders->idName == idName)
		{
			return borders;
		}
	}

	return NULL;
}

//----------------------------------------------------------------------------------

void FrameApp::rememberAllWindowBorders()
{
	for (int i = 0; i < mFWindows.getSize(); i++)
	{
		FrameWindow *win = (FrameWindow *) mFWindows.getElement(i);
		win->mFGui->rememberWindowBorders();
	}
}

//----------------------------------------------------------------------------------

void FrameApp::addPrefsWindowBorders(PrefsObject *dict)
{
	PrefsObject *winArray = IPrefsObjects->PrefsArray(NULL, NULL, ALPO_Alloc, 0, TAG_DONE);

	rememberAllWindowBorders();

	for (int i = 0; i < mWindowBorders.getSize(); i++)
	{
		WindowBorders *borders = (WindowBorders *) mWindowBorders.getElement(i);

		PrefsObject *name = IPrefsObjects->PrefsString(NULL, NULL, ALPOSTR_AllocSetString, borders->idName.c_str(), TAG_DONE);
		PrefsObject *left = IPrefsObjects->PrefsNumber(NULL, NULL, ALPONUM_AllocSetLong, borders->left, TAG_DONE);
		PrefsObject *top = IPrefsObjects->PrefsNumber(NULL, NULL, ALPONUM_AllocSetLong, borders->top, TAG_DONE);
		PrefsObject *width = IPrefsObjects->PrefsNumber(NULL, NULL, ALPONUM_AllocSetLong, borders->width, TAG_DONE);
		PrefsObject *height = IPrefsObjects->PrefsNumber(NULL, NULL, ALPONUM_AllocSetLong, borders->height, TAG_DONE);

		PrefsObject *winDict = IPrefsObjects->PrefsDictionary(NULL, NULL, ALPO_Alloc, 0, TAG_DONE);
		IPrefsObjects->DictSetObjectForKey(winDict, name, "Name");
		IPrefsObjects->DictSetObjectForKey(winDict, left, "Left");
		IPrefsObjects->DictSetObjectForKey(winDict, top, "Top");
		IPrefsObjects->DictSetObjectForKey(winDict, width, "Width");
		IPrefsObjects->DictSetObjectForKey(winDict, height, "Height");

		IPrefsObjects->PrefsArray(winArray, NULL, ALPOARR_AddObj, winDict, TAG_DONE);
	}

	IPrefsObjects->DictSetObjectForKey(dict, winArray, "WindowBorders");

}

//----------------------------------------------------------------------------------

void FrameApp::getPrefsWindowBorders(PrefsObject *dict)
{
	PrefsObject *winArray = IPrefsObjects->DictGetObjectForKey(dict, "WindowBorders");
	if (winArray)
	{
		uint32 numOfWins = 0;
		IPrefsObjects->PrefsArray(winArray, NULL, ALPOARR_GetCount, &numOfWins, TAG_DONE);
		for (int i = 0; i < numOfWins; i++)
		{
			ALPOObjIndex objIndex = {
				NULL, //PrefsObject *obj;
				i
			};
			IPrefsObjects->PrefsArray(winArray, NULL, ALPOARR_GetObjAtIndex, &objIndex, TAG_DONE);
			PrefsObject *winDict = objIndex.obj;
			if (winDict)
			{
				const char *name = IPrefsObjects->DictGetStringForKey(winDict, "Name", NULL);
				int top = IPrefsObjects->DictGetIntegerForKey(winDict, "Top", 0);
				int left = IPrefsObjects->DictGetIntegerForKey(winDict, "Left", 0);
				int width = IPrefsObjects->DictGetIntegerForKey(winDict, "Width", -1);
				int height = IPrefsObjects->DictGetIntegerForKey(winDict, "Height", -1);
				if (name != NULL && width != -1 && height != -1)
				{
					setWindowBorders(name, left, top, width, height);
				}
			}
		}
	}
}

//==================================================================================
// Requester
//==================================================================================

void FrameApp::systemMessage(int type, const char *text)
{
	int image;
	switch (type)
	{
		case REQ_INFORMATION:
			image = REQIMAGE_INFO;
		break;

		case REQ_WARNING:
			image = REQIMAGE_WARNING;
		break;

		case REQ_ERROR:
			image = REQIMAGE_ERROR;
		break;

		default:
			throw invalid_argument("Message type is wrong.");
	}

	Object *requester = (Object *) IIntuition->NewObject(IRequester->REQUESTER_GetClass(), NULL,
			REQ_Type, REQTYPE_INFO,
			REQ_TitleText, APPNAME,
			REQ_BodyText, text,
			REQ_GadgetText, "_Ok",
			REQ_Image, image,
			TAG_DONE);

	if (!requester)
	{
		throw runtime_error("Could not create requester.");
	}

	int result = IIntuition->IDoMethod(requester, RM_OPENREQ, NULL, NULL, NULL);

	IIntuition->DisposeObject(requester);
}

//----------------------------------------------------------------------------------

void FrameApp::message(int type, const char *text)
{
	int image;
	switch (type)
	{
		case REQ_INFORMATION:
			image = REQIMAGE_INFO;
		break;

		case REQ_WARNING:
			image = REQIMAGE_WARNING;
		break;

		case REQ_ERROR:
			image = REQIMAGE_ERROR;
		break;

		default:
			throw invalid_argument("Message type is wrong.");
	}

	Object *requester = (Object *) IIntuition->NewObject(IRequester->REQUESTER_GetClass(), NULL,
			REQ_Type, REQTYPE_INFO,
			REQ_TitleText, APPNAME,
			REQ_BodyText, text,
			REQ_GadgetText, "_Ok",
			REQ_Image, image,
			TAG_DONE);

	if (!requester)
	{
		throw runtime_error("Could not create requester.");
	}

	Window *win = mFWindows.getSize() > 0 ? ((FrameWindow *) mFWindows.getElement(0))->mFGui->mWindow : NULL;

	setBusyMouse(true);
	int result = IIntuition->IDoMethod(requester, RM_OPENREQ, NULL, win, NULL);
	setBusyMouse(false);

	IIntuition->DisposeObject(requester);
}

//----------------------------------------------------------------------------------

int FrameApp::question(int type, const char *text, const char *buttons)
{
	int answer = 1;
	int image;
	switch (type)
	{
		case REQ_QUESTION:
			image = REQIMAGE_QUESTION;
		break;

		case REQ_WARNING:
			image = REQIMAGE_WARNING;
		break;

		case REQ_ERROR:
			image = REQIMAGE_ERROR;
		break;

		default:
			throw invalid_argument("Question type is wrong.");
	}


	Object *requester = (Object *) IIntuition->NewObject(IRequester->REQUESTER_GetClass(), NULL,
			REQ_Type, REQTYPE_INFO,
			REQ_TitleText, APPNAME,
			REQ_BodyText, text,
			REQ_GadgetText, buttons,
			REQ_Image, image,
			TAG_DONE);

	if (!requester)
	{
		throw runtime_error("Could not create requester.");
	}

	Window *win = mFWindows.getSize() > 0 ? ((FrameWindow *) mFWindows.getElement(0))->mFGui->mWindow : NULL;

	setBusyMouse(true);
	answer = IIntuition->IDoMethod(requester, RM_OPENREQ, NULL, win, NULL);
	setBusyMouse(false);

	IIntuition->DisposeObject(requester);

	return answer;
}

//==================================================================================
// Mouse
//==================================================================================

void FrameApp::setBusyMouse(bool active)
{
	for (int i = 0; i < mFWindows.getSize(); i++)
	{
		FrameWindow *win = (FrameWindow *) mFWindows.getElement(i);
		IIntuition->SetAttrs(win->mFGui->mWindowObject,
			WA_BusyPointer, active,
			TAG_DONE);
	}
}

