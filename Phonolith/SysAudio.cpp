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

#include "SysAudio.h"

#include <devices/ahi.h>

#include <proto/exec.h>
#include <proto/ahi.h>
#include <proto/dos.h>


MsgPort *SysAudio::mAHImp = NULL;
AHIRequest *SysAudio::mAHIio = NULL;

Hook SysAudio::mSoundHook;

//==================================================================================
// Library Handling
//==================================================================================

Library *AHIBase = NULL;
AHIIFace *IAHI = NULL;
int mSignal = -1;

void SysAudio::openLib()
{
	mAHImp = NULL;
	mAHIio = NULL;

	mAHImp = (MsgPort *) IExec->CreateMsgPort();
	if (!mAHImp)
	{
		throw runtime_error("Could not create message port for audio system.");
	}

	mAHIio = (AHIRequest *) IExec->CreateIORequest(mAHImp, sizeof(AHIRequest));
	if (!mAHIio)
	{
		throw runtime_error("Could not create IO request for audio system.");
	}

	mAHIio->ahir_Version = 4;
	int error = IExec->OpenDevice(AHINAME, AHI_NO_UNIT, (IORequest *) mAHIio, 0L);
	if (error == 0)
	{
		AHIBase = (Library *) mAHIio->ahir_Std.io_Device;
		IAHI = (AHIIFace *) IExec->GetInterface(AHIBase, "main", 1, NULL);
	}
	if (error || !IAHI)
	{
		throw runtime_error("Could not open ahi.device (version 4 or higher)");
	}

	mSoundHook.h_MinNode.mln_Succ = NULL;
	mSoundHook.h_MinNode.mln_Pred = NULL;
	mSoundHook.h_SubEntry = NULL;
	mSoundHook.h_Data = NULL;
	mSoundHook.h_Entry = (HOOKFUNC)SoundFunc;
}

//----------------------------------------------------------------------------------

void SysAudio::closeLib()
{
	IExec->DropInterface((Interface *) IAHI);
	IExec->CloseDevice((IORequest *) mAHIio);
	IExec->DeleteIORequest((IORequest *) mAHIio);
	IExec->DeleteMsgPort(mAHImp);
}

//==================================================================================
// System
//==================================================================================

bool SysAudio::requestMode(unsigned long *modeID, unsigned long *frequency, string *modeName)
{
	AHIAudioModeRequester *req = IAHI->AHI_AllocAudioRequest(
			AHIR_TitleText, "Select Audio Output",
			AHIR_DoMixFreq, TRUE,
			AHIR_DoDefaultMode, TRUE,
			AHIR_InitialAudioID, *modeID,
			AHIR_InitialMixFreq, *frequency,
			TAG_DONE);

	if (IAHI->AHI_AudioRequest(req, TAG_DONE))
	{
		*modeID = req->ahiam_AudioID;
		*frequency = req->ahiam_MixFreq;

		if (*modeID == 0)
		{
			*modeName = "default";
			*frequency = 44100;
		}
		else
		{
			char name[256];
			if (IAHI->AHI_GetAudioAttrs(*modeID, NULL,
					AHIDB_BufferLen, 256,
					AHIDB_Name, &name,
					TAG_DONE))
			{
				*modeName = name;
			}
		}

		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------

uint32 SysAudio::SoundFunc(Hook *hook, AHIAudioCtrl *audioCtrl, AHISoundMessage *msg)
{
	IExec->Signal((Task *) audioCtrl->ahiac_UserData, (1L << mSignal));
	return 0;
}

//==================================================================================
// Constructor/Destructor
//==================================================================================

SysAudio::SysAudio()
{
	mAudioCtrl = NULL;
	mNumOutputs = 0;
	mOutputActive = NULL;
	mBufferSize = 0;
	mSrcFrequency = 0;
	mSrcBytesPerSample = 0;
	mBufferNumber = 0;
	mSignal = -1;

	for (int i = 0; i < NUM_OF_BUFFERS; i++)
	{
		mInfo[i].ahisi_Address = NULL;
	}

}

//----------------------------------------------------------------------------------

SysAudio::~SysAudio()
{
}

//==================================================================================
// Creator
//==================================================================================

bool SysAudio::openAudio(unsigned long audioModeID, long hwFrequency, long srcFrequency, int srcBytesPerSample, long bufferSize)
{
	mBufferSize = bufferSize;
	mSrcFrequency = srcFrequency;
	mSrcBytesPerSample = srcBytesPerSample;

	mNumOutputs = 2;
	mOutputActive = new bool[mNumOutputs];
	for (int i = 0; i < mNumOutputs; i++)
	{
		mOutputActive[i] = false;
	}

	mAudioCtrl = (AHIAudioCtrl *) IAHI->AHI_AllocAudio(
		AHIA_AudioID, audioModeID,
		AHIA_MixFreq, hwFrequency,
		AHIA_Channels, 1,
		AHIA_Sounds, NUM_OF_BUFFERS,
		AHIA_SoundFunc, &mSoundHook,
		AHIA_UserData, IExec->FindTask(NULL),
		TAG_DONE);

	if (!mAudioCtrl)
	{
		closeAudio();
		return false;
	}

	for (int i = 0; i < NUM_OF_BUFFERS; i++)
	{
		APTR dataBuffer = (APTR) IExec->AllocVec(bufferSize * mNumOutputs * mSrcBytesPerSample, MEMF_PUBLIC | MEMF_CLEAR);
		if (!dataBuffer)
		{
			closeAudio();
			return false;
		}
		mInfo[i].ahisi_Address = dataBuffer;
		mInfo[i].ahisi_Type = AHIST_S16S;
		mInfo[i].ahisi_Length = bufferSize;
		int error = IAHI->AHI_LoadSound(i, AHIST_DYNAMICSAMPLE, &mInfo[i], mAudioCtrl);
		if (error)
		{
			closeAudio();
			return false;
		}
	}

	mSignal = IExec->AllocSignal(-1);
	if (mSignal == -1)
	{
		closeAudio();
		return false;
	}

	return true;
}

//----------------------------------------------------------------------------------

void SysAudio::closeAudio()
{
	if (mAudioCtrl)
	{
		deactivateOutputs(0, mNumOutputs - 1);
	}

	for (int i = 0; i < NUM_OF_BUFFERS; i++)
	{
		APTR dataBuffer = mInfo[i].ahisi_Address;
		if (dataBuffer)
		{
			IAHI->AHI_UnloadSound(i, mAudioCtrl);
			IExec->FreeVec(dataBuffer);
            mInfo[i].ahisi_Address = NULL;
		}
	}

	if (mAudioCtrl)
	{
		IAHI->AHI_FreeAudio(mAudioCtrl);
		mAudioCtrl = NULL;
	}
	IExec->FreeSignal(mSignal);
	mSignal = -1;

	mNumOutputs = 0;
	if (mOutputActive)
	{
		delete[] mOutputActive;
		mOutputActive = NULL;
	}

}

//==================================================================================
// Outputs
//==================================================================================


int SysAudio::getNumOutputs()
{
	return mNumOutputs;
}

//----------------------------------------------------------------------------------

void SysAudio::activateOutputs(int firstOut, int lastOut)
{
	for (int i = firstOut; i <= lastOut; i++)
	{
		mOutputActive[i] = true;
	}
	IAHI->AHI_ControlAudio(mAudioCtrl, AHIC_Play, TRUE, TAG_DONE);
	IAHI->AHI_SetFreq(0, mSrcFrequency, mAudioCtrl, AHISF_IMM);
	IAHI->AHI_SetVol(0, 0x10000, 0x00008000, mAudioCtrl, AHISF_IMM);
	IAHI->AHI_SetSound(0, 1, 0, 0, mAudioCtrl, AHISF_IMM);
}

//----------------------------------------------------------------------------------

void SysAudio::deactivateOutputs(int firstOut, int lastOut)
{
	for (int i = firstOut; i <= lastOut; i++)
	{
		mOutputActive[i] = false;
	}
	IAHI->AHI_ControlAudio(mAudioCtrl, AHIC_Play, FALSE, TAG_DONE);
}

//==================================================================================
// Player
//==================================================================================


void SysAudio::copyBufferToOutputs(int firstOut, int channels, void *buffer)
{
	short *outBuffer = (short *) mInfo[mBufferNumber].ahisi_Address;
	short *srcBuffer = (short *) buffer;

	for (long i = 0; i < mBufferSize; i++)
	{
		for (int channel = 0; channel < channels; channel++)
		{
			outBuffer[i * mNumOutputs + firstOut + channel] = srcBuffer[i * channels + channel];
		}
	}

}

//----------------------------------------------------------------------------------

void SysAudio::process()
{
	IAHI->AHI_SetSound(0, mBufferNumber, 0, 0, mAudioCtrl, 0);

	//wait for playback start
	IExec->Wait(1L << mSignal);

	if (++mBufferNumber == NUM_OF_BUFFERS)
	{
		mBufferNumber = 0;
	}

}

