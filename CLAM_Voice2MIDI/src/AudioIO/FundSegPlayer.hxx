/*
 * FundSegPlayer.hxx
 * Interface for FundSegPlayer class
 * Used to reproduce the melody represented 
 * by the sequence of notes obtained after the
 * segmentation process.
 *
 * Ismael Mosquera Rivera PFC Voice2MIDI UPF 2004
*/
#ifndef _FUNDSEGPLAYER_
#define _FUNDSEGPLAYER_

#include <pthread.h>
#include "Array.hxx"
#include "Melody.hxx"
#include "Signalv0.hxx"
#include "Slotv0.hxx"
#include "Signalv1.hxx"
#include "Slotv1.hxx"

using CLAM::TSize;
using CLAM::TData;
using CLAM::DataArray;
using CLAM::Melody;
using SigSlot::Signalv0;
using SigSlot::Slotv0;
using SigSlot::Signalv1;
using SigSlot::Slotv1;

class FundSegPlayer 
{
public:
	FundSegPlayer(const Melody& originalMelody,
		          TData samplerate,
				  TSize nSamples,
			      Slotv0& slot0,
		          Slotv1<DataArray>& slot1);
	~FundSegPlayer();

    void Stop();

private:
	pthread_t mThread;
	Melody _melody;
	TData _sampleRate;
	TSize _nSamples;

	Signalv0 mRequestStop;
	Signalv1<DataArray> mSendData;

	bool active;

    void PlayFundSegThreadSafe();
	static void* sPlayFundSegThreadSafe(void* thisObject);
};

#endif
