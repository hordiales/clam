#ifndef HoaEncoder_hxx
#define HoaEncoder_hxx
#include <CLAM/AudioInPort.hxx>
#include <CLAM/AudioOutPort.hxx>
#include <CLAM/Processing.hxx>
#include <CLAM/Audio.hxx>
#include <CLAM/InControl.hxx>
#include <cmath>
#include "Orientation.hxx"

/**
 Encodes an audio signal in Higher Order Ambisonics (HOA).
 Considers the audio a plane wave coming from the direction
 indicated by azimuth and elevation controls (in degrees).
 
 @param[in] Input [Port] Audio to be encoded.
 @param[out] W/X/Y/Z [Port] Pressure that the virtual sound emits.
 @ingroup SpatialAudio
 @see AmbisonicsConventions
*/

class HoaEncoder : public CLAM::Processing
{
	CLAM::AudioInPort _input;
	typedef std::vector<CLAM::AudioOutPort*> OutPorts;
	OutPorts _outputs;
	CLAM::InControl _azimuth;
	CLAM::InControl _elevation;
	float _deltaAngle;
	float _deltaNumeric;

	float rad( float deg ) const
	{
		return deg / 180 * M_PI;
	}
public:
	class Config : public CLAM::ProcessingConfig
	{
		DYNAMIC_TYPE_USING_INTERFACE(Config,2,ProcessingConfig);
		DYN_ATTRIBUTE(0, public, unsigned, Order);
		DYN_ATTRIBUTE(1, public, bool, UseFuMa);
	protected:
		void DefaultInit()
		{
			AddAll();
			UpdateData();
			SetOrder(1);
			SetUseFuMa(true);
		};
	};
private:
	Config _config;

public:
	const char* GetClassName() const { return "HoaEncoder"; }
	HoaEncoder(const Config& config = Config()) 
		: _input("Input", this)
		, _azimuth("azimuth", this) // angle in degrees
		, _elevation("elevation", this) // angle in degrees
	{
		Configure( config );
		_azimuth.SetBounds(-360, 360); //a complete spin on each slider direction
		_elevation.SetBounds(-90, 90);
		_azimuth.SetDefaultValue(0);
		_elevation.SetDefaultValue(0);
		_deltaAngle = 0.001; 
		_deltaNumeric = 0.00001; 
	}

	bool ConcreteConfigure(const CLAM::ProcessingConfig& config)
	{
		CopyAsConcreteConfig(_config, config);
		const unsigned buffersize = BackendBufferSize();
		unsigned order = _config.GetOrder();
		if (order>3) 
		{
			std::ostringstream os;
			os << "Order " << order << " not supported." << std::endl;
			return AddConfigErrorMessage(os.str());
		}
		CLAM::SphericalHarmonicsDefinition *sh = CLAM::Orientation::sphericalHarmonics();
		unsigned i=0;
		for (;sh[i].name; i++)
		{
			if (sh[i].order > order) break;
			if (i<_outputs.size()) continue;
			CLAM::AudioOutPort * port = new CLAM::AudioOutPort( sh[i].name, this);
			port->SetSize( buffersize );
			port->SetHop( buffersize );
			_outputs.push_back( port );
		}
		unsigned actualSize=i;
		for (;i<_outputs.size(); i++)
		{
			delete _outputs[i];
		}
		_outputs.resize(actualSize);

		_elevation.DoControl(0.);
		_azimuth.DoControl(0.);
		_input.SetSize(buffersize);
		_input.SetHop(buffersize);
		return true;
	}
	
	const CLAM::ProcessingConfig & GetConfig() const { return _config; }

	bool Do()
	{
		CLAM::Orientation incidence(_azimuth.GetLastValue(), _elevation.GetLastValue());
		const CLAM::DataArray& w =_input.GetAudio().GetBuffer();
		CLAM::SphericalHarmonicsDefinition *sh = CLAM::Orientation::sphericalHarmonics();
		for (unsigned i=0; i<_outputs.size(); i++)
		{
			double gainToApply = incidence.sphericalHarmonic(sh[i].order, sh[i].zProjection, sh[i].sign>=0);
			if (_config.GetUseFuMa()) gainToApply *= sh[i].weightFuMa;
			CLAM::DataArray& out =_outputs[i]->GetAudio().GetBuffer();
			for (int sample=0; sample<w.Size(); sample++)
				out[sample] = w[sample]*gainToApply;
			_outputs[i]->Produce();
		}
		_input.Consume();
		return true;
	}

};
#endif

