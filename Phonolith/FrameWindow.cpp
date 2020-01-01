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

using namespace std;

#include "FrameWindow.h"

//==================================================================================
// Constructor/Destructor
//==================================================================================

FrameWindow::FrameWindow(FrameApp *fApp)
{
	mFApp = fApp;
	mFGui = NULL;
	mLeaveWindow = false;
}

//----------------------------------------------------------------------------------

FrameWindow::~FrameWindow()
{
	delete mFGui;
}

//==================================================================================
// ...
//==================================================================================

int FrameWindow::closeRequested()
{
	return CLOSE_WINDOW;
}

//----------------------------------------------------------------------------------

void FrameWindow::leaveWindow()
{
	mLeaveWindow = true;
}
