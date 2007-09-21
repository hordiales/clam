/*
 * Copyright (c) 2007 Fundació Barcelona Media Universitat Pompeu Fabra
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

#ifndef ImpulseResponseLoaderAndInterpolator_hxx
#define ImpulseResponseLoaderAndInterpolator_hxx

#include <CLAM/InPort.hxx>
#include <CLAM/OutPort.hxx>
#include <CLAM/Processing.hxx>
#include "ComplexSpectrum.hxx"
#include "ComplexSpectrumMixer.hxx"
#include "LoadImpulseResponse.hxx"
#include <CLAM/AudioInFilename.hxx>

namespace CLAM
{

class ImpulseResponseLoaderAndInterpolator : public Processing
{ 
public:
	class Config : public ProcessingConfig
	{
	    DYNAMIC_TYPE_USING_INTERFACE( Config, 3, ProcessingConfig );
	    DYN_ATTRIBUTE( 0, public, int, FrameSize);
	    DYN_ATTRIBUTE( 1, public, AudioInFilename, OriginImpulseResponse);
	    DYN_ATTRIBUTE( 2, public, AudioInFilename, FinalImpulseResponse);

	protected:
	    void DefaultInit()
	    {
		  AddAll();
		  UpdateData();
		  SetFrameSize(512);
	    };
	};

private:
	Config _config;
	InControl _position;
	OutPort< std::vector<ComplexSpectrum>* > _impulseResponse;
	std::vector<ComplexSpectrum> _responseSpectrums;

	double _lastPosition;
	std::vector<ComplexSpectrum> _originResponseSpectrums;
	std::vector<ComplexSpectrum> _finalResponseSpectrums;


public:
	const char* GetClassName() const { return "ImpulseResponseLoaderAndInterpolator"; }
	ImpulseResponseLoaderAndInterpolator(const Config& config = Config()) 
		: _position("Position", this)
		, _impulseResponse("ImpulseResponse", this)
	{
		Configure( config );
		_position.SetBounds(0,1);
	}
	bool ConcreteConfigure(const ProcessingConfig & config)
	{
		CopyAsConcreteConfig(_config, config);

		std::string errorMsg;
		if (!computeResponseSpectrums( _config.GetOriginImpulseResponse(), _originResponseSpectrums, _config.GetFrameSize(), errorMsg ))
		{
			AddConfigErrorMessage(errorMsg);
			return false;
		}
		if (!computeResponseSpectrums( _config.GetFinalImpulseResponse(), _finalResponseSpectrums, _config.GetFrameSize(), errorMsg ))
		{
			AddConfigErrorMessage(errorMsg);
			return false;
		}

		ConfigureInterpolatedResponseSpectrums();
		_position.DoControl(_position.DefaultValue());
		InterpolateResponseSpectrums();

		return true;
	}
	const ProcessingConfig & GetConfig() const { return _config; }

	void ConfigureInterpolatedResponseSpectrums()
	{
		const unsigned nBlocks = std::min(_originResponseSpectrums.size(), _finalResponseSpectrums.size());
		_responseSpectrums.resize(nBlocks);
		for (unsigned i=0; i<nBlocks; i++)
		{
			ComplexSpectrum & spectrum = _responseSpectrums[i];
			spectrum.spectralRange=_finalResponseSpectrums[0].spectralRange;
			spectrum.bins.assign(_finalResponseSpectrums[0].bins.size(),std::complex<CLAM::TData>());
		}
	}
	void InterpolateResponseSpectrums()
	{
		if (std::abs( _position.GetLastValue() - _lastPosition) < 0.001 )
			return;
		std::cout << "ImpulseResponseLoaderAndInterpolator: position changed"<<std::endl;
		_lastPosition = _position.GetLastValue();
		ComplexSpectrumMixer mixer;
		mixer.GetInControl("Gain1").DoControl(_lastPosition);
		mixer.GetInControl("Gain2").DoControl(1-_lastPosition);
		unsigned nBlocks = _responseSpectrums.size();
		for (unsigned i=0; i<nBlocks; i++) //TODO paramter to limit the nblocks to interpolate
		{
			mixer.Do( _originResponseSpectrums[i], _finalResponseSpectrums[i], _responseSpectrums[i]);
		}
	}

	bool Do()
	{
		InterpolateResponseSpectrums();
		_impulseResponse.GetData()= &_responseSpectrums;
		_impulseResponse.Produce();
		return true;
	}
};

} // namespace CLAM

#endif // ImpulseResponseLoaderAndInterpolator_hxx

