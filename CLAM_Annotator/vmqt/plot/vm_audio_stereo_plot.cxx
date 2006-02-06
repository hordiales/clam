#include <QGridLayout>
#include "Assert.hxx"
#include "vm_ruler.hxx"
#include "vm_scroll_group.hxx"
#include "vm_plot2d.hxx"
#include "vm_grid.hxx"
#include "vm_locator_renderer.hxx"
#include "vm_data_array_renderer.hxx"
#include "vm_data_types.hxx"
#include "vm_audio_stereo_plot.hxx"

namespace CLAM
{
	namespace VM
	{
		AudioStereoPlot::AudioStereoPlot(QWidget* parent)
			: MultiDisplayPlot(parent)
		{
			init_audio_stereo_plot();
		}

		AudioStereoPlot::~AudioStereoPlot()
		{
		}

		void AudioStereoPlot::set_data(const Audio& chn0, const Audio& chn1, bool update)
		{
			CLAM_ASSERT(chn0.GetSize() == chn1.GetSize(), "Size of channels 0 and 1 must be the same!");
			
			if(!update)
			{
				wp_display[MASTER]->set_xrange(0.0,chn0.GetDuration()/1000.0);
				wp_display[SLAVE]->set_xrange(0.0,chn0.GetDuration()/1000.0);
				wp_display[MASTER]->set_yrange(-1.0,1.0);
				wp_display[SLAVE]->set_yrange(-1.0,1.0);
				std::pair<int, int> zoom_steps = get_zoom_steps(chn0.GetBuffer().Size());
				wp_display[MASTER]->set_zoom_steps(zoom_steps.first,zoom_steps.second);
			}
			static_cast<Grid*>(wp_display[MASTER]->get_renderer("grid0"))->set_grid_steps(chn0.GetDuration()/1000.0,1.0);
			static_cast<DataArrayRenderer*>(wp_display[MASTER]->get_renderer("chn0"))->set_data(chn0.GetBuffer());
			static_cast<Grid*>(wp_display[SLAVE]->get_renderer("grid1"))->set_grid_steps(chn0.GetDuration()/1000.0,1.0);
			static_cast<DataArrayRenderer*>(wp_display[SLAVE]->get_renderer("chn1"))->set_data(chn1.GetBuffer());
		}

		void AudioStereoPlot::backgroundWhite()
		{
			setPalette(Qt::white);
			wp_xruler->set_bg_color(Color(255,255,255));
			wp_xruler->set_fg_color(Color(0,0,0));
			wp_yruler0->set_bg_color(Color(255,255,255));
			wp_yruler0->set_fg_color(Color(0,0,0));
			wp_yruler1->set_bg_color(Color(255,255,255));
			wp_yruler1->set_fg_color(Color(0,0,0));
			wp_hscroll->setPalette(Qt::white);
			wp_vscroll->setPalette(Qt::white);
			wp_display[MASTER]->set_bg_color(Color(255,255,255));
			wp_display[SLAVE]->set_bg_color(Color(255,255,255));
			static_cast<Grid*>(wp_display[MASTER]->get_renderer("grid0"))->set_grid_color(Color(0,0,255));
			static_cast<Grid*>(wp_display[SLAVE]->get_renderer("grid1"))->set_grid_color(Color(0,0,255));
			static_cast<DataArrayRenderer*>(wp_display[MASTER]->get_renderer("chn0"))->set_data_color(Color(0,0,255));
			static_cast<DataArrayRenderer*>(wp_display[SLAVE]->get_renderer("chn1"))->set_data_color(Color(0,0,255));
			static_cast<Locator*>(wp_display[MASTER]->get_renderer("locator0"))->set_locator_color(Color(250,160,30));
			static_cast<Locator*>(wp_display[SLAVE]->get_renderer("locator1"))->set_locator_color(Color(250,160,30));
		}

		void AudioStereoPlot::backgroundBlack()
		{
			setPalette(Qt::black);
			wp_xruler->set_bg_color(Color(0,0,0));
			wp_xruler->set_fg_color(Color(255,255,255));
			wp_yruler0->set_bg_color(Color(0,0,0));
			wp_yruler0->set_fg_color(Color(255,255,255));
			wp_yruler1->set_bg_color(Color(0,0,0));
			wp_yruler1->set_fg_color(Color(255,255,255));
			wp_hscroll->setPalette(Qt::darkGreen);
			wp_vscroll->setPalette(Qt::darkGreen);
			wp_display[MASTER]->set_bg_color(Color(0,0,0));
			wp_display[SLAVE]->set_bg_color(Color(0,0,0));
			static_cast<Grid*>(wp_display[MASTER]->get_renderer("grid0"))->set_grid_color(Color(0,255,0));
			static_cast<Grid*>(wp_display[SLAVE]->get_renderer("grid1"))->set_grid_color(Color(0,255,0));
			static_cast<DataArrayRenderer*>(wp_display[MASTER]->get_renderer("chn0"))->set_data_color(Color(0,255,0));
			static_cast<DataArrayRenderer*>(wp_display[SLAVE]->get_renderer("chn1"))->set_data_color(Color(0,255,0));
			static_cast<Locator*>(wp_display[MASTER]->get_renderer("locator0"))->set_locator_color(Color(255,0,0));
			static_cast<Locator*>(wp_display[SLAVE]->get_renderer("locator1"))->set_locator_color(Color(255,0,0));
		}

		void AudioStereoPlot::updateLocator(double value)
		{
			static_cast<Locator*>(wp_display[MASTER]->get_renderer("locator0"))->updateLocator(value);
			static_cast<Locator*>(wp_display[SLAVE]->get_renderer("locator1"))->updateLocator(value);
		}

		void AudioStereoPlot::updateLocator(double value,bool flag)
		{
			static_cast<Locator*>(wp_display[MASTER]->get_renderer("locator0"))->updateLocator(value,flag);
			static_cast<Locator*>(wp_display[SLAVE]->get_renderer("locator1"))->updateLocator(value,flag);
		}

		void AudioStereoPlot::setMaxVScroll(int value)
		{
			int max = value-wp_display[MASTER]->height();
			if(max < 0) max=0;
			if(wp_vscroll->get_max_scroll_value() == max) return;
			wp_vscroll->setMaxScrollValue(max);

		}

		void AudioStereoPlot::create_display()
		{
			wp_display.resize(2);
			for(unsigned i=0; i < wp_display.size(); i++) 
			{
				wp_display[i] = new Plot2D(this);
			}
			wp_display[MASTER]->add_renderer("grid0", new Grid());
			wp_display[MASTER]->add_renderer("locator0", new Locator());
			wp_display[MASTER]->add_renderer("chn0", new DataArrayRenderer());
			wp_display[SLAVE]->add_renderer("grid1", new Grid());
			wp_display[SLAVE]->add_renderer("locator1", new Locator());
			wp_display[SLAVE]->add_renderer("chn1", new DataArrayRenderer());

			wp_display[MASTER]->send_to_back("chn0");
			wp_display[MASTER]->send_to_back("grid0");
			wp_display[SLAVE]->send_to_back("chn1");
			wp_display[SLAVE]->send_to_back("grid1");

			static_cast<Grid*>(wp_display[MASTER]->get_renderer("grid0"))->show_grid(true);
			static_cast<Grid*>(wp_display[SLAVE]->get_renderer("grid1"))->show_grid(true);

			connect(static_cast<Locator*>(wp_display[MASTER]->get_renderer("locator0")),
					SIGNAL(regionChanged(double,double,bool)),
					static_cast<Locator*>(wp_display[SLAVE]->get_renderer("locator1")),
					SLOT(updateRegion(double,double,bool)));
			connect(static_cast<Locator*>(wp_display[SLAVE]->get_renderer("locator1")),
					SIGNAL(regionChanged(double,double,bool)),
					static_cast<Locator*>(wp_display[MASTER]->get_renderer("locator0")),
					SLOT(updateRegion(double,double,bool)));
			connect(static_cast<Locator*>(wp_display[MASTER]->get_renderer("locator0")),
					SIGNAL(selectedRegion(double,double)),this,SIGNAL(selectedRegion(double,double)));
			connect(static_cast<Locator*>(wp_display[SLAVE]->get_renderer("locator1")),
					SIGNAL(selectedRegion(double,double)),this,SIGNAL(selectedRegion(double,double)));
		}
	
		void AudioStereoPlot::init_audio_stereo_plot()
		{
			// common inits			
			init_multidisplay_plot(MASTER);

			// own inits
			wp_yruler0 = new Ruler(this,CLAM::VM::eLeft);
			wp_yruler1 = new Ruler(this,CLAM::VM::eLeft);
			wp_vscroll = new ScrollGroup(CLAM::VM::eVertical,this);
			QFontMetrics fm(wp_yruler0->get_font());
			int yruler_width = fm.width("-0.0e+00")+12;
			wp_yruler0->setFixedWidth(yruler_width);
			wp_yruler1->setFixedWidth(yruler_width);

			wp_vscroll->setFixedWidth(20);

			// layout
			wp_layout = new QGridLayout(this);
			wp_layout->setMargin(0);
			wp_layout->setSpacing(1);
			
			wp_layout->addWidget(wp_xruler,0,1);
			wp_layout->addWidget(wp_yruler0,1,0);
			wp_layout->addWidget(wp_yruler1,2,0);
			wp_layout->addWidget(wp_display[MASTER],1,1);
			wp_layout->addWidget(wp_display[SLAVE],2,1);
			wp_layout->addWidget(wp_vscroll,1,2,2,1);
			wp_layout->addWidget(wp_hscroll,3,1);

			wp_xruler->set_step(0.01);
			wp_yruler0->set_step(0.05);
			wp_yruler1->set_step(0.05);
		   
			// connect yrulers and vertical scroll group
			connect(wp_display[MASTER],SIGNAL(yRulerRange(double,double)),
					wp_yruler0,SLOT(updateRange(double,double)));
			connect(wp_display[MASTER],SIGNAL(yRulerRange(double,double)),
					wp_yruler1,SLOT(updateRange(double,double)));
			
			connect(wp_vscroll,SIGNAL(zoomIn()),wp_display[MASTER],SLOT(vZoomIn()));
			connect(wp_vscroll,SIGNAL(zoomOut()),wp_display[MASTER],SLOT(vZoomOut()));
			connect(wp_vscroll,SIGNAL(scrollValueChanged(int)),wp_display[MASTER],SLOT(updateVScrollValue(int)));

			connect(wp_display[MASTER],SIGNAL(vZoomRatio(QString)),wp_vscroll,SLOT(updateZoomRatio(QString)));
			connect(wp_display[MASTER],SIGNAL(vScrollValue(int)),wp_vscroll,SLOT(updateScrollValue(int)));
			connect(wp_display[MASTER],SIGNAL(vScrollMaxValue(int)),this,SLOT(setMaxVScroll(int)));
			
			// synchronize vertical scrolling
			connect(wp_display[MASTER],SIGNAL(yRulerRange(double,double)),
					wp_display[SLAVE],SLOT(setVBounds(double,double)));

			backgroundWhite();
		}

		std::pair<int,int> AudioStereoPlot::get_zoom_steps(TSize size)
		{
			double n = 100.0;
			int xratio = 0;
			while(n < size)
			{
				n *= 2.0;
				xratio++;
			}
			
			n = 0.2;
			int yratio = 0;
			while(n < 2.0)
			{
				n *= 2.0;
				yratio++;
			}
			return std::make_pair(--xratio,--yratio);
		}
	}
}

// END
