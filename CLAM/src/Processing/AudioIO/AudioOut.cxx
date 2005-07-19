/*
 * Copyright (c) 2001-2004 MUSIC TECHNOLOGY GROUP (MTG)
 *                         UNIVERSITAT POMPEU FABRA
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "AudioOut.hxx"
#include "AudioManager.hxx"
#include "Factory.hxx"


typedef CLAM::Factory<CLAM::Processing> ProcessingFactory;

namespace CLAM
{

	namespace detail
	{
		static ProcessingFactory::Registrator<AudioOut> regtAudioOut( "AudioOut" );
	}
	
AudioOut::AudioOut() 
	: mInput( "Audio Input", this )
{ 
	mpDevice = 0; 
	Configure(AudioIOConfig()); 
}

AudioOut::AudioOut(const AudioIOConfig &c)
	: mInput( "Audio Input", this )
{ 
	mpDevice = 0; 
	Configure(c); 
}

AudioOut::~AudioOut()
{ 
	if (mpDevice) 
		mpDevice->Unregister(*this); 
}

bool AudioOut::ConcreteConfigure(const ProcessingConfig& c)
	throw(ErrProcessingObj)
{
	CopyAsConcreteConfig(mConfig, c);

	if (mpDevice)
		mpDevice->Unregister(*this);

	AudioManager *m;

	try {
		m = &(AudioManager::Current());
	}
	catch (Err &e) {
		ErrProcessingObj ne("AudioOut::ConcreteConfigure(): No AudioManager found.",this);
		ne.Embed(e);
		throw(ne);
	}

	bool res;
	try {
		res = m->Register(*this);
	}
	catch (Err &e) {
		AddConfigErrorMessage( e.what() );
		res = false;
	}

	if (res == false)
		AddConfigErrorMessage( "AudioOut::ConcreteConfigure(): "
		       "Failed to register in AudioManager.") ;

	mInput.SetSize(mConfig.GetFrameSize());
	return res;
}

bool AudioOut::ConcreteStart(void) 
{
	if (!mpDevice)
		throw Err("AudioOut::ConcreteStart(): No Device found!");
	mpDevice->Start();
	return true;
}

void AudioOut::GetDeviceInfo(AudioDevice::TInfo &info) const
{
	if (mpDevice)
		mpDevice->GetInfo(info);
	else
		info.Reset();
}

bool AudioOut::Do()
{
	bool res = Do(mInput.GetAudio());
	mInput.Consume();;
	return res;
}

}
