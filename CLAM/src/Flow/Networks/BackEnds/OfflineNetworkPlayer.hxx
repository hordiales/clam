#ifndef OfflineNetworkPlayer_hxx
#define OfflineNetworkPlayer_hxx

#include "NetworkPlayer.hxx"
#include "AudioSource.hxx"
#include "AudioSink.hxx"
#include <fstream>
#include <sndfile.hh>  

#include "ControlSequencer.hxx"

namespace CLAM
{

class OfflineNetworkPlayer : public NetworkPlayer
{
	typedef std::vector<std::string> FileNames;
	FileNames _inFileNames;
	FileNames _outFileNames;

	typedef std::vector<int> ChannelsFiles;
	ChannelsFiles _outChannelsFiles;
	
	int _format;
	bool _enableLoopInputWavs;
	float _resultWavsTime;

	ControlSequencer* _controlSequencer;
	std::string  _controlFile;

public:
	OfflineNetworkPlayer()
		: _enableLoopInputWavs(false)
		, _resultWavsTime(0.0)
		, _controlSequencer(0)
	{
	}
	
	~OfflineNetworkPlayer()
	{
		delete _controlSequencer;
	}

	void AddInputFile( const std::string& );
	void AddOutputFile( const std::string& );
	void AddNumChannels(int channel);
	void SetFormat(int format);
	// base class (virtual) interface:
	bool IsWorking();
	std::string NonWorkingReason();
	virtual bool IsRealTime() const { return false; }
	
	typedef std::vector<SndfileHandle*> SndFileHandles;
	std::string listOfSourcesSinksAndFiles(const SndFileHandles & infiles, 
		const SndFileHandles & outfiles);
	
	virtual void Start();
	virtual void Stop();

	void ProcessInputFile();

	// specific interface - not inherited	
	void EnableLoopInputWavs() 
	{
		_enableLoopInputWavs=true;
	}
	void SetResultWavsTime( double time)
	{
		_resultWavsTime=time;
	}

	void SetControlFile(std::string const& controlFile)
	{
		_controlFile = controlFile;
	}
	unsigned BackendBufferSize() 
	{
		return 512;
	}

};

}
#endif

