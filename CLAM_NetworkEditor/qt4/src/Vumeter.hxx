#ifndef Vumeter_hxx
#define Vumeter_hxx

#include "Oscilloscope.hxx"
class VumeterMonitor : public OscilloscopeMonitor
{
	const char* GetClassName() const { return "Vumeter"; };
};

#include <QtOpenGL/QGLWidget>
#undef GetClassName
#include <QtGui/QLabel>
#include <CLAM/Processing.hxx>
#include <CLAM/DataTypes.hxx>


class Vumeter : public QWidget
{
	enum Dimensions {
		margin=4
	};
public:
	Vumeter(CLAM::VM::FloatArrayDataSource * dataSource, QWidget * parent=0)
		: QWidget(parent)
		, _dataSource( dataSource)
		, _energy(0)
	{
		startTimer(50);
	}
	void paintEvent(QPaintEvent * event)
	{
		QPainter painter(this);
		painter.setBrush(Qt::red);
		painter.setPen(Qt::black);
		int vumeterHeight = int(height()*(energy()*10));
//		std::cout << "painting.."  << vumeterHeight << "\tenergy: "<<_energy << std::endl;
		painter.drawRect(margin,height()-vumeterHeight,width()-2*margin,height());
	}
	double energy()
	{
		_energy*=0.5;
		if ( !_dataSource) return _energy;
		const CLAM::TData * data = _dataSource->frameData();
		unsigned size = _dataSource->nBins();
		if ( !size)
		{
			_dataSource->release();
			_energy = 0;
			return _energy;
		}
		for (unsigned i=0; i<size; i++)
		{
			const CLAM::TData & bin = data[i];
			_energy+=bin*bin*0.5;
		}
		_energy /= size;
		_dataSource->release();

		return _energy;
	}
	void timerEvent(QTimerEvent *event)
	{
		if ( !_dataSource) return;
		if ( !_dataSource->isEnabled()) return;
			return;
		update();
	}
private:
	CLAM::VM::FloatArrayDataSource * _dataSource;
	double _energy;
};



#endif//Vumeter_hxx
