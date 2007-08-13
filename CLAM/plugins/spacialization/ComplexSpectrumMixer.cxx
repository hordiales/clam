#include "ComplexSpectrumMixer.hxx"
#include <CLAM/ProcessingFactory.hxx>
namespace CLAM
{
namespace Hidden
{
	static const char* metadata[] = {
		"key", "ComplexSpectrumMixer",
		"category", "Spectral Processing",
		"description", "ComplexSpectrumMixer",
		0
	};
	static CLAM::FactoryRegistrator<CLAM::ProcessingFactory, ComplexSpectrumMixer>
	reg = metadata;

}

}
