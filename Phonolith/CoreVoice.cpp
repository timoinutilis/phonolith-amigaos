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
#include <cmath>

using namespace std;

#include "CoreVoice.h"
#include "CoreCalc.h"


//==================================================================================
// Constructor/Destructor
//==================================================================================

CoreVoice::CoreVoice(unsigned long mixFrequency)
{
	mSound = 0;
	mPosition = 0;
	mFrequency = 0;
	mInUse = false;
	mMixFrequency = mixFrequency;
	mAmpEnvelope.mMixFrequency = mixFrequency;
	mPlayTime = 0;
}

//----------------------------------------------------------------------------------

CoreVoice::~CoreVoice()
{
}

//==================================================================================
// Control
//==================================================================================

void CoreVoice::startSound(Zone *kg, char key, char velocity, CoreEnvelope *envelope, long frame)
{
	mSound = kg->sound;
	mPosition = kg->startPosition;
	mEndPosition = kg->endPosition;
	mFrequency = mSound->mSampleRate * pow(2, ((float)key - kg->baseKey + kg->mainTune + (kg->fineTune / 100.0)) / 12);
	mStartAtFrame = frame;
	mStopAtFrame = -1;
	mPlayTime = 0;

	mAmpEnvelope.init(envelope, velocity);

	mLeftAmp = CoreCalc::panoramaAmpLeft(kg->panning);
	mRightAmp = CoreCalc::panoramaAmpRight(kg->panning);

	mInUse = true;
}

//----------------------------------------------------------------------------------

void CoreVoice::stopSound(long frame)
{
	mStopAtFrame = frame;
}

//----------------------------------------------------------------------------------

bool CoreVoice::isFree()
{
	return !mInUse;
}

//----------------------------------------------------------------------------------

void CoreVoice::abortSound()
{
	mInUse = false;

}

//==================================================================================
// Process
//==================================================================================

void CoreVoice::process(float *mixBuffer, long bufferFrames)
{
	if (!mInUse)
	{
		return;
	}

	short *data = (short *) mSound->mData;
	short value1L, value2L, value1R, value2R;
	float interValueL, interValueR;
	float ratio;
	float amp;
	unsigned long position;
	for (long i = mStartAtFrame; i < bufferFrames; i++)
	{
		amp = mAmpEnvelope.getNextVolume();

		if (i == mStopAtFrame)
		{
			mStartAtFrame = -1;
			mAmpEnvelope.beginRelease();
		}

		if (amp == 0)
		{
			mInUse = false;
			break;
		}

		//===MIX===
		ratio = mPosition - (unsigned long)mPosition;
		position = (unsigned long)mPosition * mSound->mNumChannels;

		//left
		value1L = data[position];
		if (mPosition + 1 < mSound->mNumSampleFrames)
			value2L = data[(unsigned long)(mPosition + 1) * mSound->mNumChannels];
		else
			value2L = 0;

		interValueL = ((1 - ratio) * value1L) + (ratio * value2L);

		if (mSound->mNumChannels > 1)
		{
			//STEREO right
			value1R = data[position + 1];
			if (mPosition + 1 < mSound->mNumSampleFrames)
				value2R = data[(unsigned long)(mPosition + 1) * mSound->mNumChannels + 1];
			else
				value2R = 0;

			interValueR = ((1 - ratio) * value1R) + (ratio * value2R);
		}
		else
		{
			//MONO right
			interValueR = interValueL;
		}

		mixBuffer[i * 2]     += interValueL * amp * mLeftAmp;
		mixBuffer[i * 2 + 1] += interValueR * amp * mRightAmp;


		//position
		mPosition += mFrequency / mMixFrequency;
		if (mSound->mLoopActive)
		{
			if (mPosition >= mSound->mLoopEndFrame)
			{
				mPosition -= mSound->mLoopEndFrame - mSound->mLoopBeginFrame;
			}
		}
		else
		{
			if (mPosition >= mEndPosition) // || mPosition >= mSound->mNumSampleFrames)
			{
				mInUse = false;
				break;
			}
		}
	}
	mStartAtFrame = 0;
	mPlayTime++;
}

