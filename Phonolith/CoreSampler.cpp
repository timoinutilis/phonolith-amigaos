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
#include <cmath>

using namespace std;

#include "CoreSampler.h"
#include "CoreSamplerUnit.h"
#include "SysMidi.h"
#include "SysAudio.h"
#include "main.h"
#include "IOAiffOutput.h"

#include <proto/exec.h>
#include <proto/dos.h>

//public stuff

short CoreSampler::mPeakL[CoreProject::CHANNELS];
short CoreSampler::mPeakR[CoreProject::CHANNELS];
short CoreSampler::mMasterPeakL;
short CoreSampler::mMasterPeakR;
string CoreSampler::mLastProblem = "";


//internal stuff

void destroyProcesses();
void clearMidiBuffer();

void midiReceiver();
void audioMixer();
bool progressSaver();
void audioSaver();

Task *mAppTask;
Process *mMidiProcess;
Process *mMixerProcess;
Process *mSaverProcess;

SysTimer *mTimer;
bool mQuitSystems;

CoreProject *mProject;
CoreSamplerUnit *mSamplerUnits[CoreProject::CHANNELS];
unsigned long mAudioModeID;
long mFrequency;
int mSrcBytesPerSample;
long mBufferSize;
int mMaxVoices;
const char *mMidiPortName;

//midi receiver
const static int MIDI_BUFFER_NOTES = 64;
MidiEvent mMidiEvents[MIDI_BUFFER_NOTES];
int mCurrentMidiIndex;
int mMidiSignal;
bool mMidiReceived;

//audio mixer
short *mMasterBuffer;

//audio saver
const static int SAVER_BUFFER_SECS = 1;
int mSaverNumBlocks;
short *mSaveBuffer;
int mSaverSignal;
int mSaverMixBlockIndex;
int mSaverWriteBlockIndex;


//==================================================================================
// Communication
//==================================================================================

void signalStatus()
{
	//**** signal to GUI
	if (main_statusUpdateSignal != -1)
	{
		IExec->Signal(mAppTask, (1L << main_statusUpdateSignal));
	}
}


//==================================================================================
// Constructor/Destructor
//==================================================================================

CoreSampler::CoreSampler(unsigned long audioModeID, long frequency, int srcBytesPerSample, long bufferSize, int maxVoices, const char *midiPortName, CoreProject *project)
{
//	  cout << "starting Sampler... " << midiPortName << " to audio mode " << hex << audioModeID << dec << endl;

	CoreSampler::mLastProblem = "";

	mAppTask = IExec->FindTask(NULL);
	mMidiProcess = NULL;
	mMixerProcess = NULL;
	mSaverProcess = NULL;

	mTimer = new SysTimer();
	mQuitSystems = false;

	mProject = project;
	for (int i = 0; i < CoreProject::CHANNELS; i++)
	{
		mSamplerUnits[i] = NULL;
	}
	mAudioModeID = audioModeID;
	mFrequency = frequency;
	mSrcBytesPerSample = srcBytesPerSample;
	mBufferSize = bufferSize;
	mMaxVoices = maxVoices;
	mMidiPortName = midiPortName;

	clearMidiBuffer();
	mMidiSignal = -1;

	mMasterBuffer = NULL;

	mSaveBuffer = NULL;
	mSaverSignal = -1;
	mSaverMixBlockIndex = 0;
	mSaverWriteBlockIndex = 0;
	mSaverNumBlocks = max(4, (int)(mFrequency * SAVER_BUFFER_SECS / mBufferSize));

	//====create audio saver process====
	mSaverProcess = IDOS->CreateNewProcTags(
			NP_Entry, &audioSaver,
			NP_Child, TRUE,
			NP_Name, "PhonolithAudioSaver",
			TAG_DONE);

	//====create audio mixer process====
	mMixerProcess = IDOS->CreateNewProcTags(
			NP_Entry, &audioMixer,
			NP_Child, TRUE,
			NP_Name, "PhonolithAudio",
			NP_Priority, 10,
			TAG_DONE);

	//====create MIDI process====
	mMidiProcess = IDOS->CreateNewProcTags(
			NP_Entry, &midiReceiver,
			NP_Child, TRUE,
			NP_Name, "PhonolithMidi",
			NP_Priority, 10,
			TAG_DONE);


}

//----------------------------------------------------------------------------------

CoreSampler::~CoreSampler()
{
	destroyProcesses();
}

//----------------------------------------------------------------------------------

void destroyProcesses()
{
	mQuitSystems = true;

	// signal midi
	if (mMidiProcess && mMidiSignal != -1)
	{
		IExec->Signal(&mMidiProcess->pr_Task, 1L << mMidiSignal);
	}

	// signal saver
	if (mSaverProcess && mSaverSignal != -1)
	{
		IExec->Signal(&mSaverProcess->pr_Task, 1L << mSaverSignal);
	}

	int counter = 0;
	while (mMidiProcess || mMixerProcess || mSaverProcess)
	{
		IDOS->Delay(2);
		if (++counter >= 80)
		{
			runtime_error e("A player process cannot quit.");
			showError(e);
			break;
		}
	}
	delete mTimer;

}

//==================================================================================
// MIDI
//==================================================================================

void midiReceiver()
{
	SysMidi midiSystem;

	mMidiSignal = IExec->AllocSignal(-1);
	if (mMidiSignal == -1)
	{
		CoreSampler::mLastProblem = "Could not allocate MIDI signal";
		signalStatus();
	}
	else
	{
		bool success = midiSystem.openMidi(mMidiPortName, mMidiSignal);

		if (!success)
		{
			CoreSampler::mLastProblem = "Could not create MIDI link";
			signalStatus();
		}
		else
		{
			char status, channel, data1, data2;
			do
			{
				IExec->Wait(1L << mMidiSignal);

				while (midiSystem.getEvent(
						&mMidiEvents[mCurrentMidiIndex].status,
						&mMidiEvents[mCurrentMidiIndex].channel,
						&mMidiEvents[mCurrentMidiIndex].data1,
						&mMidiEvents[mCurrentMidiIndex].data2))
				{
					mMidiEvents[mCurrentMidiIndex].frame = mTimer->getMicros() * mFrequency / 1000000;
					mCurrentMidiIndex++;
					mMidiReceived = true;
					if (mCurrentMidiIndex >= MIDI_BUFFER_NOTES)
					{
						mCurrentMidiIndex = MIDI_BUFFER_NOTES - 1;
					}
				}
			} while (!mQuitSystems);

			midiSystem.closeMidi();
		}

		IExec->FreeSignal(mMidiSignal);
		mMidiSignal = -1;
	}
	mMidiProcess = NULL;
}

//----------------------------------------------------------------------------------

void clearMidiBuffer()
{
	mCurrentMidiIndex = 0;
	for (int i = 0; i < MIDI_BUFFER_NOTES; i++)
	{
		mMidiEvents[i].frame = 0;
		mMidiEvents[i].status = 0;
		mMidiEvents[i].channel = 0;
		mMidiEvents[i].data1 = 0;
		mMidiEvents[i].data2 = 0;
	}
	mMidiReceived = false;
}

//----------------------------------------------------------------------------------

void CoreSampler::resetMidiReceived()
{
	mMidiReceived = false;
}

//==================================================================================
// Unit Control
//==================================================================================

CoreSamplerUnit *CoreSampler::getSamplerUnit(int unitIndex)
{
	return mSamplerUnits[unitIndex];
}

//----------------------------------------------------------------------------------


void CoreSampler::lockUnit(int unitIndex)
{
	if (mSamplerUnits[unitIndex])
	{
		mSamplerUnits[unitIndex]->lock();
	}
}

//----------------------------------------------------------------------------------

void CoreSampler::unlockUnit(int unitIndex)
{
	if (mSamplerUnits[unitIndex])
	{
		mSamplerUnits[unitIndex]->unlock();
	}
}

//----------------------------------------------------------------------------------

void CoreSampler::setUnitNumVoices(int unitIndex, int num)
{
	if (mSamplerUnits[unitIndex])
	{
		mSamplerUnits[unitIndex]->setNumVoices(num);
	}
}

//==================================================================================
// Audio
//==================================================================================

void audioMixer()
{
	float *mixBuffer = NULL;
	float *unitMixBuffer = NULL;
	IOAiffOutput *aiffOutput = NULL;
	SysAudio audioSystem;

	char channel, data1, data2;
	long int frame;
	int status;

	int length = mBufferSize * 2;
	mixBuffer = new float[length];
	mMasterBuffer = new short[length];
	unitMixBuffer = new float[length];

	bool success = audioSystem.openAudio(mAudioModeID, mFrequency, mFrequency, mSrcBytesPerSample, mBufferSize);
	if (!success)
	{
		CoreSampler::mLastProblem = "Could not allocate audio";
		signalStatus();
	}
	else
	{
		audioSystem.activateOutputs(0, 1);

		for (int i = 0; i < CoreProject::CHANNELS; i++)
		{
			mSamplerUnits[i] = new CoreSamplerUnit(mProject->mInstruments[i], unitMixBuffer, mBufferSize, mFrequency, mMaxVoices);
		}

		do
		{
			//===delegate notes to sampler units===
			for (int i = 0; i < mCurrentMidiIndex; i++)
			{
				channel = mMidiEvents[i].channel;
				data1 = mMidiEvents[i].data1;
				data2 = mMidiEvents[i].data2;
				frame = mMidiEvents[i].frame;
				if (frame >= mBufferSize) //can happen at overload
				{
					frame = mBufferSize - 1;
				}
				status = mMidiEvents[i].status;

				if (status == MS_NoteOff || (status == MS_NoteOn && data2 == 0))
				{
					mSamplerUnits[channel]->releasedKey(data1, frame);
				}
				else if (status == MS_NoteOn)
				{
					mSamplerUnits[channel]->pressedKey(data1, data2, frame);
				}
			}
			mTimer->reset();
			mCurrentMidiIndex = 0;

			//===clear master mix buffer===
			for (long i = 0; i < length; i++)
			{
				mixBuffer[i] = 0;
			}

			//===process sampler units and mix to master buffer===
			float ampLeft;
			float ampRight;
			float valueL;
			float valueR;
			float peakL;
			float peakR;
			for (int i = 0; i < CoreProject::CHANNELS; i++)
			{
				mSamplerUnits[i]->process();
				ampLeft  = mProject->mChannelVolume[i] * mProject->mChannelPanoramaAmpL[i];
				ampRight = mProject->mChannelVolume[i] * mProject->mChannelPanoramaAmpR[i];
				peakL = CoreSampler::mPeakL[i];
				peakR = CoreSampler::mPeakR[i];
				for (int p = 0; p < mBufferSize; p++)
				{
					//get values
					valueL = unitMixBuffer[p * 2]     * ampLeft;
					valueR = unitMixBuffer[p * 2 + 1] * ampRight;

					//clip left
					if (valueL > 32767.0)
					{
						valueL = 32767.0;
					}
					else if (valueL < -32767.0)
					{
						valueL = -32767.0;
					}
					if (valueL > peakL)
					{
						peakL = valueL;
					}
					else if (valueL < -peakL)
					{
						peakL = -valueL;
					}

					//clip right
					if (valueR > 32767.0)
					{
						valueR = 32767.0;
					}
					else if (valueR < -32767.0)
					{
						valueR = -32767.0;
					}
					if (valueR > peakR)
					{
						peakR = valueR;
					}
					else if (valueR < -peakR)
					{
						peakR = -valueR;
					}

					//mix to master
					mixBuffer[p * 2]     += valueL;
					mixBuffer[p * 2 + 1] += valueR;
				}
				CoreSampler::mPeakL[i] = (short) peakL;
				CoreSampler::mPeakR[i] = (short) peakR;
			}

			//===find peak of master and clipping===
			float volume = mProject->mMasterVolume;
			peakL = CoreSampler::mMasterPeakL;
			peakR = CoreSampler::mMasterPeakR;
			float value;
			for (int p = 0; p < length; p++)
			{
				value = mixBuffer[p] * volume;
				if (value > 32767.0)
				{
					value = 32767.0;
				}
				else if (value < -32767.0)
				{
					value = -32767.0;
				}
				if (p & 1L) //right
				{
					if (value > peakR)
					{
						peakR = value;
					}
					else if (value < -peakR)
					{
						peakR = -value;
					}
				}
				else //left
				{
					if (value > peakL)
					{
						peakL = value;
					}
					else if (value < -peakL)
					{
						peakL = -value;
					}
				}

				mMasterBuffer[p] = (short)(value);
			}
			CoreSampler::mMasterPeakL = (short) peakL;
			CoreSampler::mMasterPeakR = (short) peakR;


			//**** Audio Output
			audioSystem.copyBufferToOutputs(0, 2, (void *) mMasterBuffer);
			audioSystem.process();

			//**** File Output
			if (!progressSaver())
			{
	//			  CoreSampler::mLastProblem = "mixdown buffer is full";
	//			  signalStatus();
			}

			//**** signal to GUI
			if (main_meterUpdateSignal != -1)
			{
				IExec->Signal(mAppTask, (1L << main_meterUpdateSignal));
			}

		} while (!mQuitSystems);

		audioSystem.closeAudio();
	}

	mProject->mRecordingActive = false;

	for (int i = 0; i < CoreProject::CHANNELS; i++)
	{
		if (mSamplerUnits[i]) delete mSamplerUnits[i];
		mSamplerUnits[i] = NULL;
	}

	if (mixBuffer) delete[] mixBuffer;
	if (unitMixBuffer) delete[] unitMixBuffer;
	if (mMasterBuffer)
	{
		delete[] mMasterBuffer;
		mMasterBuffer = NULL;
	}

	mMixerProcess = NULL;
}

//----------------------------------------------------------------------------------

bool progressSaver()
{
	if (mSaverProcess && mSaverSignal != -1 && mSaveBuffer)
	{
		if (mProject->mRecordingActive)
		{
			//check for buffer overflow
			int over = mSaverMixBlockIndex - mSaverWriteBlockIndex;
			if (over < 0)
			{
				over += mSaverNumBlocks;
			}

			if (over > 0)
			{
				if (over >= mSaverNumBlocks - 1)
				{
					//buffer is full
					return false;
				}
			}

			//copy master-buffer to save-buffer
			int length = mBufferSize * 2;
			int pos = mSaverMixBlockIndex * length;
			for (int i = 0; i < length; i++)
			{
				mSaveBuffer[pos++] = mMasterBuffer[i];
			}
			if (++mSaverMixBlockIndex >= mSaverNumBlocks)
			{
				mSaverMixBlockIndex = 0;
			}
		}

		IExec->Signal(&mSaverProcess->pr_Task, 1L << mSaverSignal);
	}
	return true;
}

//----------------------------------------------------------------------------------

void audioSaver()
{
	IOAiffOutput aiffOutput;

	mSaverSignal = IExec->AllocSignal(-1);

	int length = mBufferSize * 2;
	mSaveBuffer = new short[length * mSaverNumBlocks];

	if (mProject->mRecordingRequested)
	{
		bool success = aiffOutput.openFile(mProject->mRecordFileName.c_str(), 2, mSrcBytesPerSample, mFrequency);
		if (!success)
		{
			CoreSampler::mLastProblem = "Could not create file '" + mProject->mRecordFileName + "'";
			signalStatus();
		}
	}
		
	do
	{
		IExec->Wait(1L << mSaverSignal);

		if (mProject->mRecordingRequested && !mProject->mRecordingActive)
		{
			if (!aiffOutput.isOpen())
			{
				mSaverWriteBlockIndex = 0;
				mSaverMixBlockIndex = 0;
				bool success = aiffOutput.openFile(mProject->mRecordFileName.c_str(), 2, mSrcBytesPerSample, mFrequency);
				if (!success)
				{
					CoreSampler::mLastProblem = "Could not create file '" + mProject->mRecordFileName + "'";
					signalStatus();
				}

			}
			if (mMidiReceived)
			{
				mProject->mRecordingActive = true;
				signalStatus();
			}
		}
		if (mProject->mRecordingActive)
		{
			if (mProject->mRecordingRequested)
			{
				if (aiffOutput.isOpen())
				{
					while (mSaverWriteBlockIndex != mSaverMixBlockIndex)
					{
						bool success = aiffOutput.writeData((void *) &mSaveBuffer[mSaverWriteBlockIndex * length], mBufferSize);
						if (!success)
						{
							aiffOutput.closeFile();
							CoreSampler::mLastProblem = "Error while writing to file";
							signalStatus();
							break;
						}

						if (++mSaverWriteBlockIndex >= mSaverNumBlocks)
						{
							mSaverWriteBlockIndex = 0;
						}
					}
				}
			}
			else
			{
				if (aiffOutput.isOpen())
				{
					aiffOutput.closeFile();
				}
				mProject->mRecordingActive = false;
			}
		}

	} while (!mQuitSystems);

	if (mSaveBuffer) delete[] mSaveBuffer;

	aiffOutput.closeFile();

	if (mSaverSignal != -1)
	{
		IExec->FreeSignal(mSaverSignal);
		mSaverSignal = -1;
	}

	mSaverProcess = NULL;
}

