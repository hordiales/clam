/*
 * Copyright (c) 2004 MUSIC TECHNOLOGY GROUP (MTG)
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

#ifndef __SPECTRUMPLOTCONTROLLER__
#define __SPECTRUMPLOTCONTROLLER__

#include <vector>
#include "Spectrum.hxx"
#include "DataRenderer.hxx"
#include "SpectralPeakArray.hxx"
#include "SpectralPeaksRenderer.hxx"
#include "PlotController.hxx"

namespace CLAM
{
    namespace VM
    {
		class SpectrumPlotController : public PlotController
		{
			Q_OBJECT

		public:
			SpectrumPlotController();
			~SpectrumPlotController();

			void SetData(const Spectrum& spec);
			void SetData(const Spectrum& spec,const SpectralPeakArray& peaks);
 
			void SetDataColor(Color c);
			void SetPeaksColor(Color cline,Color cpoint);

			void DisplayDimensions(const int& w, const int& h);

			void Draw();
			void SetMousePos(const double& x, const double& y);

			void SetSelPos(const double& value, bool render);

		signals:
			void sendMagFreq(double,double);

		public slots:
			void setHBounds(double, double);
			void setSelectedXPos(double);

		protected:
			void SetHBounds(const double& left,const double& right);				
			void SetVBounds(const double& bottom,const double& top);

			void FullView();				
		   
		private:
			Spectrum     mSpec;
			DataRenderer mRenderer;
			DataArray    mMagBuffer;
			DataArray    mProcessedData;
			double       mSpectralRange;
			bool         mMustProcessData;
			bool         mHasData;

			SpectralPeakArray         mPeaks;
			std::vector<FreqMagPoint> mProcessedPeaks;
			std::vector<FreqMagPoint> mCachedPeaks;        
			SpectralPeaksRenderer     mPeaksRenderer;
			bool                      mHasPeaks;

			void AdaptSpectralData();
			void CacheData();
			void ProcessData();

			void CachePeaksData();
			void ProcessPeaksData();

		};
    }
}

#endif


