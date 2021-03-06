/*
 * Copyright (c) 2001-2006 MUSIC TECHNOLOGY GROUP (MTG)
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

#include <QtGui/QGridLayout>
#include "vmRuler.hxx"
#include "vmPlotCanvas.hxx"
#include "vmScrollGroup.hxx"
#include <CLAM/Segmentation.hxx>
#include "vmSegmentEditor.hxx"
#include "vmLocatorRenderer.hxx"
#include "vmSegmentationPlot.hxx"

namespace CLAM
{
	namespace VM
	{
		SegmentationPlot::SegmentationPlot(QWidget* parent)
			: WPlot(parent)
			, mCurrentSegmentFollowsPlay(true)
		{
			InitSegmentationPlot();
		}

		SegmentationPlot::~SegmentationPlot()
		{
		}

		void SegmentationPlot::SetXRange(double xmin, double xmax, ERulerScale scale)
		{
			mPlot->SetXRange(xmin,xmax);
			mXRuler->SetRange(xmin,xmax);
			mXRuler->SetScale(scale);
		}

		void SegmentationPlot::SetYRange(double ymin, double ymax, ERulerScale scale)
		{
			mPlot->SetYRange(ymin,ymax);
			mYRuler->SetRange(ymin,ymax);
			mYRuler->SetScale(scale);
		}

		void SegmentationPlot::SetSegmentation(Segmentation* s)
		{
			mSegmentation->SetSegmentation(s);
		}

		void SegmentationPlot::SetZoomSteps(int hsteps, int vsteps)
		{
			mPlot->SetZoomSteps(hsteps,vsteps);
		}

		void SegmentationPlot::backgroundWhite()
		{
			setPalette(Qt::white);
			mXRuler->SetBackgroundColor(QColor(255,255,255));
			mXRuler->SetForegroundColor(QColor(0,0,0));
			mYRuler->SetBackgroundColor(QColor(255,255,255));
			mYRuler->SetForegroundColor(QColor(0,0,0));
			mHScroll->setPalette(Qt::white);
			mVScroll->setPalette(Qt::white);
			mPlot->SetBackgroundColor(QColor(255,255,255));
			_locator->SetLocatorColor(QColor(250,160,30));
		}

		void SegmentationPlot::backgroundBlack()
		{
			setPalette(Qt::black);
			mXRuler->SetBackgroundColor(QColor(0,0,0));
			mXRuler->SetForegroundColor(QColor(255,255,255));
			mYRuler->SetBackgroundColor(QColor(0,0,0));
			mYRuler->SetForegroundColor(QColor(255,255,255));
			mHScroll->setPalette(Qt::darkGreen);
			mVScroll->setPalette(Qt::darkGreen);
			mPlot->SetBackgroundColor(QColor(0,0,0));
			_locator->SetLocatorColor(QColor(255,0,0));
		}

		void SegmentationPlot::setCurrentSegmentFollowsPlay(bool active)
		{
			mCurrentSegmentFollowsPlay=active;
			mSegmentation->allowChangeCurrent(active);
		}

		void SegmentationPlot::updateLocator(double value)
		{
			_locator->updateLocator(value);
			mSegmentation->checkCurrent(value);
		}

		void SegmentationPlot::updateLocator(double value, bool flag)
		{
			_locator->updateLocator(value,flag);
			mSegmentation->checkCurrent(value);
		}

		void SegmentationPlot::setVisibleXRange(double min, double max)
		{
			mPlot->setHBounds(min, max);
		}

		void SegmentationPlot::InitSegmentationPlot()
		{
			mXRuler = new Ruler(this,CLAM::VM::eTop);
			mYRuler = new Ruler(this,CLAM::VM::eLeft);

			mHScroll = new ScrollGroup(CLAM::VM::eHorizontal,this);
			mVScroll = new ScrollGroup(CLAM::VM::eVertical,this);

			mPlot = new PlotCanvas(this);
			_locator =  new Locator();
			mPlot->AddRenderer("locator", _locator);
			mSegmentation = new SegmentEditor();
			mPlot->AddRenderer("segmentation", mSegmentation);
			mPlot->BringToFront("locator");

			mLayout = new QGridLayout(this);
			mLayout->setMargin(0);
			mLayout->setSpacing(0);
			mLayout->addWidget(mXRuler,0,1);
			mLayout->addWidget(mYRuler,1,0);
			mLayout->addWidget(mPlot,1,1); 
			mLayout->addWidget(mVScroll,1,2);
			mLayout->addWidget(mHScroll,2,1);

			backgroundWhite();

			mPlot->setXRuler(mXRuler);
			mPlot->setYRuler(mYRuler);
			mPlot->setXRangeController(mHScroll);
			mPlot->setYRangeController(mVScroll);

			connect(mVScroll,SIGNAL(zoomIn()),mPlot,SLOT(vZoomIn()));
			connect(mVScroll,SIGNAL(zoomOut()),mPlot,SLOT(vZoomOut()));
			connect(mVScroll,SIGNAL(scrollValueChanged(int)),mPlot,SLOT(updateVScrollValue(int)));

			connect(mHScroll,SIGNAL(zoomIn()),mPlot,SLOT(hZoomIn()));
			connect(mHScroll,SIGNAL(zoomOut()),mPlot,SLOT(hZoomOut()));
			connect(mHScroll,SIGNAL(scrollValueChanged(int)),mPlot,SLOT(updateHScrollValue(int)));
			
			connect(mPlot, SIGNAL(hBoundsChanged(double,double)),this, SIGNAL(visibleXRangeChanged(double,double)));
			
			connect(_locator,SIGNAL(selectedRegion(double,double)),this,SIGNAL(selectedRegion(double,double)));

			connect(mSegmentation,SIGNAL(segmentOnsetChanged(unsigned,double)),
					this,SIGNAL(segmentOnsetChanged(unsigned,double)));
			connect(mSegmentation,SIGNAL(segmentOffsetChanged(unsigned,double)),
					this,SIGNAL(segmentOffsetChanged(unsigned,double)));
			connect(mSegmentation,SIGNAL(segmentInserted(unsigned)),this,SIGNAL(segmentInserted(unsigned)));
			connect(mSegmentation,SIGNAL(segmentDeleted(unsigned)),this,SIGNAL(segmentDeleted(unsigned)));
			connect(mSegmentation,SIGNAL(currentSegmentChanged()),this,SIGNAL(currentSegmentChanged()));
		}
	}
}

// END

