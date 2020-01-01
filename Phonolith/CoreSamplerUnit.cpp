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
#include <new>

using namespace std;

#include "CoreSamplerUnit.h"
#include "Phonolith.h"

//==================================================================================
// Constructor/Destructor
//==================================================================================

CoreSamplerUnit::CoreSamplerUnit(CoreInstrument *instrument, float *mixBuffer, long bufferSize, unsigned long frequency, int maxVoices)
{
	mInstrument = instrument;

	mMaxVoices = maxVoices;
	mNumVoices = min(mMaxVoices, instrument->mNumVoices);
	mVoices = new CoreVoice*[mMaxVoices];
	mVoiceKeys = new short[mMaxVoices];
	for (int i = 0; i < mMaxVoices; i++)
	{
		mVoices[i] = new CoreVoice(frequency);
		mVoiceKeys[i] = (char)-1;
	}

	mMixBuffer = mixBuffer;
	mBufferSize = bufferSize;
	mFrequency = frequency;
	mToneActive = false;
	mLocked = false;
}

//----------------------------------------------------------------------------------

CoreSamplerUnit::~CoreSamplerUnit()
{
	for (int i = 0; i < mMaxVoices; i++)
	{
		delete mVoices[i];
	}
	delete[] mVoices;
	delete[] mVoiceKeys;
}

//==================================================================================
// Control
//==================================================================================

void CoreSamplerUnit::setNumVoices(int num)
{
	for (int i = num; i < mNumVoices; i++)
	{
		mVoices[i]->abortSound();
	}
	mNumVoices = min(mMaxVoices, num);
}

//----------------------------------------------------------------------------------

void CoreSamplerUnit::abortSounds()
{
	for (int i = 0; i < mMaxVoices; i++)
	{
		mVoices[i]->abortSound();
	}
}

//----------------------------------------------------------------------------------

void CoreSamplerUnit::lock()
{
	mLocked = true;
	abortSounds();
}

//----------------------------------------------------------------------------------

void CoreSamplerUnit::unlock()
{
	mLocked = false;
}

//----------------------------------------------------------------------------------

void CoreSamplerUnit::pressedKey(char key, char velocity, unsigned long frame)
{
	if (mLocked) return;

	Zone *kg;

	//find matching zones
	for (int k = mInstrument->getNumOfZones() - 1; k >= 0; k--)
	{
		kg = mInstrument->getZone(k);
		if (kg->sound && key >= kg->lowKey && key <= kg->highKey && velocity >= kg->lowVelocity && velocity <= kg->highVelocity)
		{
			unsigned long maxPlayTime = 0;
			unsigned long playTime = 0;
			int bestVoice = -1;
			bool noFreeVoices = true;
			//find free voice
			for (int i = 0; i < mNumVoices; i++)
			{
				if (mVoices[i]->isFree())
				{
					mVoices[i]->startSound(kg, key, velocity, &mInstrument->mAmpEnvelope, frame);
					mVoiceKeys[i] = key;
					noFreeVoices = false;
					break;
				}
				playTime = mVoices[i]->mPlayTime;
				if (mVoices[i]->mAmpEnvelope.mState == CoreEnvelopeGenerator::ADSR_RELEASE)
				{
					playTime += 0x7FFFFF;
				}
				if (playTime > maxPlayTime)
				{
					maxPlayTime = playTime;
					bestVoice = i;
				}
			}
			if (noFreeVoices && bestVoice != -1)
			{
				mVoices[bestVoice]->startSound(kg, key, velocity, &mInstrument->mAmpEnvelope, frame);
				mVoiceKeys[bestVoice] = key;
			}
		}
	}

}

//----------------------------------------------------------------------------------

void CoreSamplerUnit::releasedKey(char key, unsigned long frame)
{
	if (mLocked) return;

	for (int i = 0; i < mNumVoices; i++)
	{
		if (mVoiceKeys[i] == key)
		{
			mVoices[i]->stopSound(frame);
			mVoiceKeys[i] = -1;
		}
	}
}

//----------------------------------------------------------------------------------

void CoreSamplerUnit::startTone(unsigned char key)
{
	mToneActive = true;
	changeTone(key);
}

//----------------------------------------------------------------------------------

void CoreSamplerUnit::changeTone(unsigned char key)
{
	mTonePosition = 0;
	mToneFrequency = 440 * 360 * pow(2, (float)(key - 69) / 12);
}

//----------------------------------------------------------------------------------
void CoreSamplerUnit::stopTone()
{
	mToneActive = false;
}

//==================================================================================
// Processing
//==================================================================================

void CoreSamplerUnit::process()
{
	int length = mBufferSize * 2;
	for (int i = 0; i < length; i++)
	{
		mMixBuffer[i] = 0.0;
	}

	if (mLocked) return;

	//Voices
	for (int i = 0; i < mNumVoices; i++)
	{
		mVoices[i]->process(mMixBuffer, mBufferSize);
	}

	//Tone
	if (mToneActive)
	{
		float amplitude;
		for (int i = 0; i < mBufferSize; i++)
		{
			amplitude = (float)(sin(3.14159265 / 180.0 * mTonePosition) * 32767.0);
			mMixBuffer[i * 2]     += amplitude;
			mMixBuffer[i * 2 + 1] += amplitude;
			mTonePosition += mToneFrequency / mFrequency;
			if (mTonePosition >= 158400)
			{
				mTonePosition -= 158400;
			}
		}
	}
}

