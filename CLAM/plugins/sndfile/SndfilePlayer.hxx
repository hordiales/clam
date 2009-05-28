/*
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
#ifndef SndfilePlayer_hxx
#define SndfilePlayer_hxx

#include <CLAM/Processing.hxx>
#include <CLAM/AudioOutPort.hxx>
#include <CLAM/InControl.hxx>
#include <CLAM/OutControl.hxx>
#include <CLAM/ProcessingConfig.hxx>
#include <CLAM/AudioInFilename.hxx> 
#include <CLAM/Audio.hxx>
#include <sndfile.hh>
#include <pthread.h>
#include <jack/ringbuffer.h>

namespace CLAM
{
	template <typename BaseType>
	class RingBuffer
	{
	public:
		RingBuffer(unsigned size)
		{
			_rb = jack_ringbuffer_create(sizeof(BaseType)*size);
			memset(_rb->buf, 0, _rb->size);
		}
		~RingBuffer()
		{
			jack_ringbuffer_free (_rb);
		}
		void read(BaseType * buffer, unsigned n)
		{
			jack_ringbuffer_read(_rb, (char*)buffer, n*sizeof(BaseType));
		}
		void writableRegion(BaseType *(&buffer), unsigned & len)
		{
			jack_ringbuffer_data_t writeSpace;
			jack_ringbuffer_get_write_vector(_rb, &writeSpace);
			buffer = (BaseType*) writeSpace.buf;
			len = writeSpace.len/sizeof(BaseType);
		}
		unsigned readSpace()
		{
			return jack_ringbuffer_read_space(_rb)/sizeof(BaseType);
		}
		void advanceWrite(unsigned n)
		{
			jack_ringbuffer_write_advance(_rb, n*sizeof(BaseType));
		}
		void write(BaseType* buffer, unsigned n)
		{
			jack_ringbuffer_write(_rb, (char*) buffer, n*sizeof(BaseType));
		}
	private:
		jack_ringbuffer_t * _rb;
	};

	class SndfilePlayerConfig : public ProcessingConfig
	{
		DYNAMIC_TYPE_USING_INTERFACE( SndfilePlayerConfig,3, ProcessingConfig );
		DYN_ATTRIBUTE( 0, public, AudioInFilename, SourceFile );
		DYN_ATTRIBUTE( 1, public, bool, Loop );
		DYN_ATTRIBUTE( 2, public, unsigned, SavedNumberOfChannels );

		protected:
		void DefaultInit()
		{
			AddAll();
			UpdateData();
			SetSavedNumberOfChannels(0);
		};
	};

	class SndfilePlayer : public  Processing
	{ 
		class Lock
		{
			pthread_mutex_t & _diskThreadLock;
			public:
				Lock(pthread_mutex_t & diskThreadLock)
					: _diskThreadLock(diskThreadLock)
				{
					pthread_mutex_lock(&diskThreadLock);
				}
				~Lock()
				{
					pthread_mutex_unlock(&_diskThreadLock);
				}
		};

		typedef SndfilePlayerConfig Config;
		typedef std::vector<CLAM::AudioOutPort*> OutPorts;
		OutPorts _outports;
		CLAM::FloatOutControl _outControlSeek;
		CLAM::FloatInControl _inControlSeek;
		CLAM::FloatInControl _inControlPause;
		CLAM::FloatInControl _inControlLoop;
		SndfileHandle* _infile;
		SndfilePlayerConfig _config;
		unsigned _sampleSize;
		unsigned _numChannels;
		unsigned _numReadFrames;
		unsigned _numTotalFrames;
		std::vector<float> _buffer; 

		pthread_t _threadId;
		long _overruns;
		const static unsigned _ringBufferSize = 4;
		volatile int _canPlay;
		volatile int _canProcess;
		volatile int _isStopped;
		/* Synchronization between process thread and disk thread. */
		pthread_mutex_t _diskThreadLock;
		pthread_cond_t  _dataReadyCondition;


		RingBuffer<TData> * _ringBuffer;
		RingBuffer<unsigned> * _positionsBuffer;

	public:
		const char* GetClassName() const { return "SndfilePlayer"; }

		SndfilePlayer(const ProcessingConfig& config =  Config()) 
			: _outControlSeek("Position out-Control",this) 
			, _inControlSeek("Seek in-Control",this) 
			, _inControlPause("Pause in-Control",this)
			, _inControlLoop("Loop in-Control",this)
			, _infile(0)
			, _numChannels(0)
			, _numReadFrames(0)
			, _numTotalFrames(0)
			, _overruns(0)
			, _ringBuffer(0)
		{ 
			static pthread_cond_t sPthreadCondInitializer = PTHREAD_COND_INITIALIZER;
			_dataReadyCondition = sPthreadCondInitializer;

			static pthread_mutex_t sPthreadMutexInitializer = PTHREAD_MUTEX_INITIALIZER;
			_diskThreadLock = sPthreadMutexInitializer;

			_isStopped = true;
			Configure( config );
		}

		bool Do()
		{
			/* Do nothing until we're ready to begin. */
			if (not _canProcess) return false;
			if (not _canPlay) return false;

			// PAUSE CONTROL
			if (_inControlPause.GetLastValue()<0.5)
				ReadBufferAndWriteToPorts();
			else
				WriteSilenceToPorts();

			// Tell the ports this is done
			for (unsigned channel=0; channel<_numChannels; channel++)
				_outports[channel]->Produce();

			return true;
		}

		void ReadBufferAndWriteToPorts()
		{
			const unsigned itemSize = sizeof(TData); 
			const unsigned nFrames = _outports[0]->GetAudio().GetBuffer().Size();
			const unsigned nItems = nFrames*_numChannels;
			const unsigned nBytes = nItems*itemSize;

			CLAM::TData* channels[_numChannels];
			for (unsigned channel=0; channel<_numChannels; channel++)
				channels[channel] = &_outports[channel]->GetAudio().GetBuffer()[0];

			// Ring buffer empty (loopless end of file or xrun)
			if (_ringBuffer->readSpace() < nItems)
			{
//				std::cout << "X" << std::flush;
				for (unsigned i=0; i<nFrames; i++) 
					for (unsigned channel = 0; channel<_numChannels; channel++)
						channels[channel][i] = 0;
			}
			else
			{
//				std::cout << "F" << std::flush;
				float frameBuffer[_numChannels];
				for (unsigned i=0; i<nFrames; i++)
				{
					_ringBuffer->read(frameBuffer, _numChannels);
					for (unsigned channel = 0; channel<_numChannels; channel++)
					{
						channels[channel][i] = frameBuffer[channel];
					}
				}
				unsigned currentPosition;
				_positionsBuffer->read(&currentPosition, 1);
				float seekPosition = (float) currentPosition/ (float)_numTotalFrames;
				if (seekPosition<=1) 
					_outControlSeek.SendControl(seekPosition);
			}

			/* Tell the disk thread there is work to do.  If it is already
			 * running, the lock will not be available.  We can't wait
			 * here in the process() thread, but we don't need to signal
			 * in that case, because the disk thread will read all the
			 * data queued before waiting again. */
			if (pthread_mutex_trylock(&_diskThreadLock) == 0) 
			{
				pthread_cond_signal (&_dataReadyCondition);
				pthread_mutex_unlock (&_diskThreadLock);
			}
		}
		void WriteSilenceToPorts(unsigned offset=0)
		{
//			std::cout << "S" << std::flush;
			//all the ports have to have the same buffer size
			const unsigned portSize = _outports[0]->GetAudio().GetBuffer().Size();
			CLAM::TData* channels[_numChannels];

			for (unsigned channel=0; channel<_numChannels; channel++)
				channels[channel] = &_outports[channel]->GetAudio().GetBuffer()[0];

			for (unsigned i=offset; i<portSize; i++) 
				for (unsigned channel = 0; channel<_numChannels; channel++)
					channels[channel][i] = 0;
		}

		void DiskThread()
		{
			pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
			Lock lock(_diskThreadLock);

			while (true) 
			{
				if (not TakeABufferFromDisk())
				{
					// wait until process() signals more data required
//					std::cout << "W" << std::flush;
					pthread_cond_wait(&_dataReadyCondition, &_diskThreadLock);
				}
				if (_isStopped) return;
			}
		}

		bool TakeABufferFromDisk()
		{
			const unsigned nFrames = _outports[0]->GetAudio().GetBuffer().Size();
			const unsigned nItems = nFrames * _numChannels;
			const unsigned nBytes = sizeof(TData) * nItems;

			TData * buffer = 0;
			unsigned writableLength;
			_ringBuffer->writableRegion(buffer, writableLength);

			if (writableLength < nItems) return false;

			if (not _inControlSeek.HasBeenRead())
			{
				float lastControlSeek = _inControlSeek.GetLastValue();
				_numReadFrames = lastControlSeek*_numTotalFrames;
				_infile->seek(_numReadFrames ,SEEK_SET);
			}

//			std::cout << "D" << std::flush;

			unsigned readItems = _infile->read(buffer, nItems);

			if (readItems)
			{
				_positionsBuffer->write(&_numReadFrames, 1);
			}

			if (readItems<nItems and _inControlLoop.GetLastValue()>0.5)
			{
//				std::cout << "L" << std::flush;
				_infile->seek(0, SEEK_SET);
				_numReadFrames = 0;
			}
			if (not readItems) return false;

			// Missing frames to zero
			for (unsigned i = readItems; i<nItems; i++)
				buffer[i]=0.;
			_ringBuffer->advanceWrite(nItems);
			_numReadFrames+=readItems/_numChannels;

			return true;
		}

		static void* DiskThreadCallback (void *arg)
		{
			SndfilePlayer * player = static_cast<SndfilePlayer*> (arg);
			player->DiskThread();
			return 0;
		}

		bool ConcreteStart()
		{
			CLAM_ASSERT(_infile, "SndfilePlayer::ConcreteStart() should have _infile with value.");

			//initial configuration for the controls.
			_inControlSeek.SetBounds(0,1);
			_inControlSeek.DoControl(0.);
			_inControlPause.SetBounds(0,1);
			_inControlPause.DoControl(0.0);
			_outControlSeek.SendControl(0.);
			_numReadFrames = 0;
			_isStopped = false;
			_canProcess = 0;
			const unsigned nFrames = _outports[0]->GetAudio().GetBuffer().Size();
			_ringBuffer = new RingBuffer<TData>(_numChannels*nFrames*_ringBufferSize);
			_positionsBuffer = new RingBuffer<unsigned>(_ringBufferSize);
			_infile->seek(0,SEEK_SET);
			_canPlay = 0;
			pthread_create(&_threadId, NULL, DiskThreadCallback, this);
			_canProcess = 1;
			_canPlay = 1;
			return true; 
		}
		bool ConcreteStop()
		{
			_isStopped = true;
			if (pthread_mutex_trylock(&_diskThreadLock) == 0) 
			{
				pthread_cond_signal (&_dataReadyCondition);
				pthread_mutex_unlock (&_diskThreadLock);
			}
			pthread_join (_threadId, NULL);
			if (_infile) delete _infile;
			_infile = new SndfileHandle(_config.GetSourceFile().c_str(), SFM_READ);

			if (_overruns > 0) 
			{
				CLAM_ASSERT(_overruns,"Overruns is greater than 0. Try a bigger buffer");
			}
			delete _ringBuffer;
			delete _positionsBuffer;
			_numReadFrames = 0;
			return true; 
		}

		const ProcessingConfig& GetConfig() const
		{
			return _config;
		}

		bool ConcreteConfigure(const ProcessingConfig & config)
		{
			CopyAsConcreteConfig(_config, config);
			unsigned portSize = BackendBufferSize();
			std::string nameOutPort = "out";
			if (_outports.empty())
			{
				ResizePorts(1);
				_numChannels = _outports.size();
			}
			// Even if further config fails, if we have SavedChannels, create them
			unsigned savedChannels = _config.GetSavedNumberOfChannels();
			if (savedChannels)
			{
				ResizePorts(savedChannels);
			}

			if ( !_config.HasSourceFile() )
			{
				return AddConfigErrorMessage("The provided config object lacked the field 'SourceFile'");
			}

			const std::string & location = _config.GetSourceFile();
			if ( location == "")
			{
				return AddConfigErrorMessage("No file selected");
			}

			if (_infile) delete _infile;
			_infile = new SndfileHandle(location.c_str(), SFM_READ);
			// check if the file is open
			if (!*_infile)
			{
				return AddConfigErrorMessage("The audio file '" + location + "' could not be opened");
			}

			//report sndfile library errors
			if (_infile->error() != 0)
			{
				return AddConfigErrorMessage(_infile->strError());
			}
			if (savedChannels and savedChannels != _infile->channels() )
			{
				return AddConfigErrorMessage(
					"The configuration have a number of channels saved which \n"
					"does not correspond to the channels in the provided audio \n"
					"file. If you want to just open a file, first choose '0' in the\n"
					"SavedNumberOfChannels config parameter");
			}

			_inControlLoop.DoControl(_config.GetLoop() ? 1 : 0);

			_numChannels = _infile->channels();
			_sampleSize = _numChannels*sizeof(TData);
			_config.SetSavedNumberOfChannels(_numChannels);
			_buffer.resize(portSize*_numChannels);
			_numTotalFrames = _infile->frames();
			// The file has not size, perhaps that's because the file is empty
			if (_numTotalFrames == 0) _numTotalFrames = 1;
			ResizePorts(_numChannels);
			return true;
		}

		~SndfilePlayer()
		{
			if (_infile) delete _infile;
			ResizePorts(0);
		}
	private:
		void ResizePorts(unsigned nPorts)
		{
			const std::string nameBase = "out";
			const unsigned portSize = BackendBufferSize();
			for (unsigned i=_outports.size(); i<nPorts; i++)
			{
				std::ostringstream nameStream;
				nameStream << nameBase << i;
				AudioOutPort * port = new AudioOutPort( nameStream.str(), this);
				_outports.push_back( port );
			}
			for (unsigned i=nPorts; i<_outports.size(); i++)
				delete _outports[i];
			_outports.resize(nPorts);
			for (unsigned i=0; i<_outports.size(); i++)
			{
				_outports[i]->SetSize( portSize );
				_outports[i]->SetHop( portSize );
			}
		}
	};

}// namespace CLAM
#endif // SndfilePlayer_hxx

