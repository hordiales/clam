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


#ifndef _MyIFFT_
#define _MyIFFT_

#include <CLAM/IFFT_base.hxx>
#include <CLAM/Audio.hxx>
#include <CLAM/Spectrum.hxx>

namespace CLAM {

	/** Implementation of the IFFT using the Fastest Fourier in the West version 3
	* @see <a HREF="http://www.fftw.org/"> FFTW Home Page</a>
	*/
	class MyIFFT: public IFFT_base
	{
		struct Implementation;
		Implementation * _fftw3;

		/* IFFT possible execution states.
		 */
		typedef enum {
			sComplex, // We just need to read the complex array.
			sOther // The complex array is not present.
		} IFFTState;

		/** Execution state of the IFFT object. It includes I/O
		    prototypes state */
		IFFTState mState;

		void DefaultInit();

		inline void CheckTypes(const Spectrum& in, const Audio &out) const;

		// Output conversions

		/** Converts a complex array to the RIFFTW input format.
		 * @param The spectrum object from which the complex array is taken. It 
		 *  must have its ComplexArray attribute instantiated.
		 * @see the <a HREF="http://www.fftw.org/doc/fftw_2.html#SEC5"> RFFTW docs </a>
		 * for a description of this format.
		 */
		inline void ComplexToRIFFTW(const Spectrum &in) const;
		inline void OtherToRIFFTW(const Spectrum &in) const;
		void ReleaseMemory();
		void SetupMemory();

		/** Configuration change method
		 * @pre argument should be a IFFTConfig
		 */
		bool ConcreteConfigure(const ProcessingConfig&);

	public:

		MyIFFT(const IFFTConfig &c=IFFTConfig());
		~MyIFFT();
		const char * GetClassName() const {return "MyIFFT";}

		bool Do();
		bool Do(const Spectrum& in, Audio &out) const;
		bool MayDisableExecution() const {return true;}

		// Port interfaces.
		bool SetPrototypes(const Spectrum& in,const Audio &out);
		bool SetPrototypes();
		bool UnsetPrototypes();

	};

}

#endif // _MyIFFT_

