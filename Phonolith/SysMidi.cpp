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

using namespace std;

#include "SysMidi.h"

#include <proto/camd.h>
#include <proto/exec.h>

//==================================================================================
// Library Handling
//==================================================================================

Library     *CamdBase       = NULL;
CamdIFace   *ICamd          = NULL;

//----------------------------------------------------------------------------------

void SysMidi::openLib()
{
	CamdBase = IExec->OpenLibrary("camd.library", 50);
	if (CamdBase)
	{
		ICamd = (CamdIFace *) IExec->GetInterface(CamdBase, "main", 1, NULL);
	}

	if (!CamdBase || !ICamd)
	{
		throw runtime_error("Could not open camd.library (version 50 or higher)");
	}
}

//----------------------------------------------------------------------------------

void SysMidi::closeLib()
{
	IExec->DropInterface((Interface *) ICamd);
	ICamd = NULL;
	IExec->CloseLibrary(CamdBase);
	CamdBase = NULL;
}

//==================================================================================
// Constructor/Destructor
//==================================================================================

SysMidi::SysMidi()
{
	mMidi = NULL;
	mMidiIn = NULL;
}

//----------------------------------------------------------------------------------

SysMidi::~SysMidi()
{
}

//==================================================================================
// Connecting
//==================================================================================

bool SysMidi::openMidi(const char *portName, int signal)
{
	if (ICamd)
	{
		mMidi = ICamd->CreateMidi(
				MIDI_MsgQueue, 2048,
				MIDI_RecvSignal, signal,
				TAG_DONE);

		if (mMidi)
		{
			mMidiIn = ICamd->AddMidiLink(mMidi, MLTYPE_Receiver,
					MLINK_Name, "SysMidi.in",
					MLINK_Location, portName,
					TAG_END);

			if (mMidiIn)
			{
				return true;
			}

		}

		if (!mMidiIn)
		{
			closeMidi();
			return false;
		}
	}
	return false;
}

//----------------------------------------------------------------------------------

void SysMidi::closeMidi()
{
	if (ICamd)
	{
		if (mMidiIn)
		{
			ICamd->RemoveMidiLink(mMidiIn);
			mMidiIn = NULL;
		}
		ICamd->DeleteMidi(mMidi);
		mMidi = NULL;
	}
}

//==================================================================================
// Midi Events
//==================================================================================

bool SysMidi::getEvent(char *status, char *channel, char *data1, char *data2)
{
	MidiMsg   msg;
	if (ICamd->GetMidi(mMidi, &msg))
	{
		if (!realtime(&msg))
		{
			*status = msg.mm_Status & MS_StatBits;
			*channel = msg.mm_Status & MS_ChanBits;
			*data1 = msg.mm_Data1;
			*data2 = msg.mm_Data2;
			if (*status == MS_NoteOn && *data2 == 0)
			{
				//convert pseudo NoteOff to real NoteOff
				*status = MS_NoteOff;
			}
			return true;
		}
	}
	return false;
}

//==================================================================================
// Info
//==================================================================================

Vector *SysMidi::getPorts()
{
	if (!ICamd)
	{
		throw runtime_error("MIDI library (CAMD) is not open.");
	}

	Vector *ports = new Vector();

	APTR camdLock = ICamd->LockCAMD(CD_Linkages);

	struct MidiCluster *cluster = NULL;
	while (cluster = ICamd->NextCluster(cluster)) {
		STRPTR name = cluster->mcl_Node.ln_Name;

		int len = strlen(name);
		char *nameCopy = new char[len + 1];
		strcpy(nameCopy, name);

		ports->addElement(nameCopy);
	}

	ICamd->UnlockCAMD(camdLock);

	return ports;
}

//----------------------------------------------------------------------------------

void SysMidi::freePortsVector(Vector *ports)
{
	for (int i = ports->getSize() - 1; i >= 0; i--)
	{
		delete[] (char *)ports->getElement(i);
	}
	delete ports;
}

