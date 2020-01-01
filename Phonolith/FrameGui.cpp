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
#include <cstring>
#include <stdexcept>
#include <new>

using namespace std;

#include "FrameGui.h"
#include "main.h"

#include <proto/intuition.h>
#include <proto/exec.h>
#include <proto/window.h>
#include <proto/gadtools.h>

#include <libraries/gadtools.h>
#include <reaction/reaction_macros.h>

//==================================================================================
// Library Handling
//==================================================================================

Library       *GadToolsBase = NULL;
GadToolsIFace *IGadTools    = NULL;

//----------------------------------------------------------------------------------

void FrameGui::openLib()
{
	#ifdef DEBUGFRAME
	cout << "GuiMenu openLib()\n";
	#endif
	GadToolsBase = IExec->OpenLibrary("gadtools.library", 50);
	if (GadToolsBase)
	{
		IGadTools = (GadToolsIFace *) IExec->GetInterface(GadToolsBase, "main", 1, NULL);
	}
}

//----------------------------------------------------------------------------------

void FrameGui::closeLib()
{
	#ifdef DEBUGFRAME
	cout << "GuiMenu closeLib()\n";
	#endif
	IExec->DropInterface((Interface *) IGadTools);
	IGadTools = NULL;
	IExec->CloseLibrary(GadToolsBase);
	GadToolsBase = NULL;
}

//==================================================================================
// Constructor/Destructor
//==================================================================================

FrameGui::FrameGui(FrameApp *fApp)
{
	mFApp = fApp;
	mAppPort = NULL;
	mWindowObject = NULL;
	mSignalMask = 0;
	mWindow = NULL;
	mMenu = NULL;
	mVisualInfo = NULL;
	mNewMenuItems = NULL;
	mNumOfNewMenuItems = 0;
	mHints = NULL;
}

//----------------------------------------------------------------------------------

FrameGui::~FrameGui()
{
}

//==================================================================================
// GUI Commands
//==================================================================================


void FrameGui::activate()
{
	if (!mWindow) // iconified?
	{
		reopenWindow();
	}

	IIntuition->WindowToFront(mWindow);
	IIntuition->ActivateWindow(mWindow);
}

//----------------------------------------------------------------------------------

void FrameGui::setTitle(char *title)
{
	IIntuition->SetAttrs(mWindowObject, WA_Title, "", TAG_DONE);
	mTitle = title;
	IIntuition->SetAttrs(mWindowObject, WA_Title, mTitle.c_str(), TAG_DONE);
}

//----------------------------------------------------------------------------------

void FrameGui::iconifyWindow()
{
	IIntuition->IDoMethod(mWindowObject, WM_ICONIFY);
	mWindow = NULL;
}
//----------------------------------------------------------------------------------

void FrameGui::closeWindow()
{
	IIntuition->IDoMethod(mWindowObject, WM_CLOSE);
	mWindow = NULL;
}

//----------------------------------------------------------------------------------

void FrameGui::reopenWindow()
{
	IIntuition->IDoMethod(mWindowObject, WM_OPEN);
	IIntuition->GetAttr(WINDOW_Window, mWindowObject, (ULONG *) &mWindow);

}

//==================================================================================
// Menu
//==================================================================================

void FrameGui::addMenu(NewMenu *newMenu)
{
	int numOfItems = 0;
	NewMenu *item = newMenu;
	while (item->nm_Type != NM_END)
	{
		numOfItems++;
		item++;
	}

	NewMenu *fullMenu = new NewMenu[mNumOfNewMenuItems + numOfItems + 1];

	//copy old menu items
	for (int i = 0; i < mNumOfNewMenuItems; i++)
	{
		memcpy(&fullMenu[i], &mNewMenuItems[i], sizeof(NewMenu));
	}

	//add new menu items and end-item
	for (int i = 0; i < numOfItems + 1; i++)
	{
		memcpy(&fullMenu[i + mNumOfNewMenuItems], &newMenu[i], sizeof(NewMenu));
	}

	if (mNewMenuItems) delete[] mNewMenuItems;
	mNewMenuItems = fullMenu;
	mNumOfNewMenuItems += numOfItems;

}

//----------------------------------------------------------------------------------

int FrameGui::getMenuItemID(unsigned long code)
{
	MenuItem *item = IIntuition->ItemAddress(mMenu, code);
	if (item)
	{
		return ((int) GTMENUITEM_USERDATA(item));
	}
	else
	{
		return -1;
	}
}

//==================================================================================
// Window
//==================================================================================

void FrameGui::createWindow(const char *idName, const char *title, Object *layout, int flags, FrameWindow *fWin)
{
	mAppPort = IExec->CreateMsgPort();
	if (!mAppPort)
	{
		throw runtime_error("Could not open window");
	}

	mTitle = title;
	mIdName = idName;

	mWindowObject = (Object *) WindowObject,
		WA_CloseGadget, TRUE,
		WA_DepthGadget, TRUE,
		WA_SizeGadget, TRUE,
		WA_DragBar, TRUE,
		WA_Activate, TRUE,
		WA_ScreenTitle, mFApp->mName,
		WA_Title, mTitle.c_str(),
//		  WA_CustomScreen, screen,
	    WINDOW_AppPort, mAppPort,
		WINDOW_Layout, layout,
		WINDOW_LockWidth, (flags & FLAG_LOCKWIDTH) != 0,
		WINDOW_LockHeight, (flags & FLAG_LOCKHEIGHT) != 0,
		WINDOW_IDCMPHook, &FrameApp::mIDCMPHook,
		WINDOW_IDCMPHookBits, IDCMP_IDCMPUPDATE,
	    WINDOW_UserData, fWin,
		WINDOW_IconifyGadget, TRUE,
		WINDOW_IconTitle, mFApp->mName,
	    WINDOW_Icon, main_applicationIcon,
		WINDOW_IconNoDispose, TRUE,
	End;

	if (!mWindowObject)
	{
		deleteWindow();
		throw runtime_error("Could not open window");
	}

	if (mHints)
	{
		IIntuition->SetAttrs(mWindowObject,
				WINDOW_GadgetHelp, TRUE,
				WINDOW_HintInfo, mHints,
				TAG_DONE);
	}

	WindowBorders *borders = mFApp->getWindowBorders(mIdName.c_str());
	if (borders)
	{
		IIntuition->SetAttrs(mWindowObject,
				WA_Top, borders->top,
				WA_Left, borders->left,
				WA_InnerWidth, borders->width,
				WA_InnerHeight, borders->height,
				TAG_DONE);
	}
	else
	{
		IIntuition->SetAttrs(mWindowObject,
				WINDOW_Position, WPOS_CENTERSCREEN,
				TAG_DONE);
	}

	if (mNumOfNewMenuItems > 0)
	{
		Screen *screen = IIntuition->LockPubScreen(NULL);
		mVisualInfo = IGadTools->GetVisualInfoA(screen, NULL);
		mMenu = IGadTools->CreateMenus(mNewMenuItems, GTMN_FullMenu, TAG_DONE);
		IGadTools->LayoutMenus(mMenu, mVisualInfo, GTMN_NewLookMenus, TRUE, TAG_DONE);
		IIntuition->UnlockPubScreen(NULL, screen);
		IIntuition->SetAttrs(mWindowObject, WINDOW_MenuStrip, mMenu, TAG_DONE);
	}

	mWindow = RA_OpenWindow(mWindowObject);
	if (!mWindow)
	{
		deleteWindow();
		throw runtime_error("Could not open window");
	}
	
	IIntuition->GetAttr(WINDOW_SigMask, mWindowObject, &mSignalMask);

}

//----------------------------------------------------------------------------------

void FrameGui::deleteWindow()
{
	rememberWindowBorders();

	IIntuition->DisposeObject(mWindowObject);
	IExec->DeleteMsgPort(mAppPort);
	if (mMenu) IGadTools->FreeMenus(mMenu);
    if (mVisualInfo) IGadTools->FreeVisualInfo(mVisualInfo);
}

//----------------------------------------------------------------------------------

void FrameGui::setGadgetHints(HintInfo *hints)
{
	mHints = hints;
}

//----------------------------------------------------------------------------------

void FrameGui::rememberWindowBorders()
{
	if (mWindow)
	{
		//get window position/size and remember it
		long top, left, width, height;
		IIntuition->GetAttrs(mWindowObject,
				WA_Top, &top,
				WA_Left, &left,
				WA_InnerWidth, &width,
				WA_InnerHeight, &height,
				TAG_DONE);
		mFApp->setWindowBorders(mIdName.c_str(), left, top, width, height);
	}
}

