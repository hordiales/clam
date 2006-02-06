#include "vm_ruler.hxx"
#include "vm_plot2d.hxx"
#include "vm_grid.hxx"
#include "vm_data_array_renderer.hxx"
#include "vm_audio_plot.hxx"

namespace CLAM
{
	namespace VM
	{
		AudioPlot::AudioPlot(QWidget* parent)
			: SegmentationPlot(parent)
		{
			init_audio_plot();
		}

		AudioPlot::~AudioPlot()
		{
		}

		void AudioPlot::set_data(const Audio& audio, bool update)
		{
			if(!update)
			{
				set_xrange(0.0,audio.GetDuration()/1000.0);
				set_yrange(-1.0,1.0);
				std::pair<int, int> zoom_steps = get_zoom_steps(audio.GetBuffer().Size());
				set_zoom_steps(zoom_steps.first,zoom_steps.second);
			}
			static_cast<Grid*>(wp_plot->get_renderer("grid"))->set_grid_steps(audio.GetDuration()/1000.0,1.0);
			static_cast<DataArrayRenderer*>(wp_plot->get_renderer("audio"))->set_data(audio.GetBuffer());
		}

		void AudioPlot::backgroundWhite()
		{
			SegmentationPlot::backgroundWhite();
			static_cast<Grid*>(wp_plot->get_renderer("grid"))->set_grid_color(Color(0,0,255));
			static_cast<DataArrayRenderer*>(wp_plot->get_renderer("audio"))->set_data_color(Color(0,0,255));
		}

		void AudioPlot::backgroundBlack()
		{
			SegmentationPlot::backgroundBlack();
			static_cast<Grid*>(wp_plot->get_renderer("grid"))->set_grid_color(Color(0,255,0));
			static_cast<DataArrayRenderer*>(wp_plot->get_renderer("audio"))->set_data_color(Color(0,255,0));
		}

		void AudioPlot::init_audio_plot()
		{
			wp_plot->add_renderer("grid", new Grid());
			wp_plot->add_renderer("audio", new DataArrayRenderer());
			wp_plot->send_to_back("audio");
			wp_plot->send_to_back("grid");
			wp_plot->bring_to_front("locator");
			wp_xruler->set_step(0.01);
			wp_yruler->set_step(0.05);
			static_cast<Grid*>(wp_plot->get_renderer("grid"))->show_grid(true);
			backgroundWhite();
		}

		std::pair<int,int> AudioPlot::get_zoom_steps(TSize size)
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

		void AudioPlot::set_xrange(double xmin, double xmax, ERulerScale scale)
		{
			SegmentationPlot::set_xrange(xmin,xmax,scale);
		}

		void AudioPlot::set_yrange(double ymin, double ymax, ERulerScale scale)
		{
			SegmentationPlot::set_yrange(ymin,ymax,scale);
		}

		void AudioPlot::set_zoom_steps(int hsteps, int vsteps)
		{
			SegmentationPlot::set_zoom_steps(hsteps,vsteps);
		}
	}
}

// END
