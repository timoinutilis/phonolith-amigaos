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

#include "SysTimer.h"

#include <proto/timer.h>
#include <proto/exec.h>
#include <exec/exec.h>

//==================================================================================
// Constructor/Destructor
//==================================================================================


MsgPort *TimerMP = NULL;
timerequest *TimerIO = NULL;
Device *TimerBase = NULL;
TimerIFace *ITimer = NULL;

SysTimer::SysTimer()
{
	TimerMP = (MsgPort *) IExec->AllocSysObject(ASOT_PORT, NULL);
	TimerIO = (timerequest *) IExec->AllocSysObjectTags(ASOT_IOREQUEST,
			ASOIOR_Size, sizeof(timerequest),
			ASOIOR_ReplyPort, TimerMP,
			TAG_DONE);

	IExec->OpenDevice(TIMERNAME, UNIT_MICROHZ, (IORequest *) TimerIO, 0);
	TimerBase = (Device *) TimerIO->tr_node.io_Device;
	ITimer = (TimerIFace *) IExec->GetInterface((Library *) TimerBase, "main", 1, NULL);

	reset();
}

//----------------------------------------------------------------------------------

SysTimer::~SysTimer()
{
//	  if (!IExec->CheckIO((IORequest *) TimerIO))
//	  {
//		  IExec->AbortIO((IORequest *) TimerIO);
//	  }
//	  IExec->WaitIO((IORequest *) TimerIO);
	IExec->DropInterface((Interface *) ITimer);
	IExec->CloseDevice((IORequest *) TimerIO);
	IExec->FreeSysObject(ASOT_IOREQUEST, TimerIO);
	IExec->FreeSysObject(ASOT_PORT, TimerMP);
}

//==================================================================================
// Time
//==================================================================================

void SysTimer::reset()
{
	ITimer->GetUpTime(&lastTime);
}

//----------------------------------------------------------------------------------

unsigned long SysTimer::getMicros()
{
	timeval nowTime;

	ITimer->GetUpTime(&nowTime);
	ITimer->SubTime(&nowTime, &lastTime);
	
	return nowTime.tv_micro + (nowTime.tv_secs * 1000000);
}

