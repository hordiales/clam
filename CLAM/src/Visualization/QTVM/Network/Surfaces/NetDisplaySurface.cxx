#include "NetDisplaySurface.hxx"

namespace CLAM
{
	namespace VM
	{
		NetDisplaySurface::NetDisplaySurface(QWidget* parent) 
			: QGLWidget(parent), _thread(this)
		{
			setAutoBufferSwap(false);
			SetBackgroundColor(0.0,0.0,0.0);
			InitView();

			_controller = NULL;
		}

		NetDisplaySurface::~NetDisplaySurface()
		{
			stopRendering();
			if(_controller) delete _controller;
		}

		void NetDisplaySurface::SetBackgroundColor(double r, double g, double b)
		{
			_thread.setBackgColor(r, g, b);
		}

		void NetDisplaySurface::SetController(NetPlotController* controller)
		{
			_controller = controller;
			connect(_controller,SIGNAL(sendView(SView)),this,SLOT(receivedView(SView)));
			_thread.setController(_controller);
		}

		void NetDisplaySurface::startRendering()
		{
			_thread.Start();
		}

		void NetDisplaySurface::stopRendering()
		{
			_thread.Stop();
		}

		void NetDisplaySurface::resizeEvent(QResizeEvent* re)
		{ 
		    int w = re->size().width();
		    int h = re->size().height();
		    _thread.resizeViewport(w,h);
		}

		void NetDisplaySurface::paintEvent(QPaintEvent* pe)
		{
			// handled by the GLThread
		}

		void NetDisplaySurface::closeEvent(QCloseEvent* ce)
		{
			stopRendering();
			QGLWidget::closeEvent(ce);
		}

		void NetDisplaySurface::receivedView(SView v)
		{
			_thread.setView(v.left, v.right, v.bottom, v.top);
		}

		void NetDisplaySurface::InitView()
		{
			_thread.setView(0.0, 1.0, -1.0, 1.0);
		}
	}
}

// END

