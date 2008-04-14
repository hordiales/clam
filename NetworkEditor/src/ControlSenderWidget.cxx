#include "ControlSenderWidget.hxx"
#include <QtGui/QSlider>
#include <QtGui/QDial>
#include <QtGui/QLabel>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QHBoxLayout>
#include "QSynthKnob.hxx"

ControlSenderWidget::ControlSenderWidget(CLAM::Processing * processing)
	: _dial(0)
	, _slider(0)
	, _spinBox(0)
	, _updating(false)
{
	_sender = dynamic_cast<CLAM::OutControlSender* >(processing);
	CLAM_ASSERT(_sender, "ControlSenderWidget only works "
				"with OutControlSender processings.");
	init();
}

ControlSenderWidget::~ControlSenderWidget()
{
}

void ControlSenderWidget::init()
{
	const CLAM::OutControlSenderConfig * config = 
		dynamic_cast<const CLAM::OutControlSenderConfig *>(&_sender->GetConfig());
	CLAM_ASSERT( config, "Unexpected Configuration type for an OutControlSender" );

	_min = config->GetMin();
	_default = config->GetDefault();
	_max = config->GetMax();
	_step = config->GetStep();

	_mappingMode = config->GetMapping();

	switch (config->GetControlRepresentation().GetValue()) {
	case CLAM::OutControlSenderConfig::EControlRepresentation::eUndetermined:
		break;
	case CLAM::OutControlSenderConfig::EControlRepresentation::eKnot:
		setLayout(new QVBoxLayout);
		createDial();
		createSpinBox();
		break;
	case CLAM::OutControlSenderConfig::EControlRepresentation::eHorizontalSlider:
		setLayout(new QHBoxLayout);
		createSlider(Qt::Horizontal);
		createSpinBox();
		break;
	case CLAM::OutControlSenderConfig::EControlRepresentation::eVerticalSlider:
		setLayout(new QVBoxLayout);
		createSlider(Qt::Vertical);
		createSpinBox();
		break;
	case CLAM::OutControlSenderConfig::EControlRepresentation::eSpinBox:
		setLayout(new QHBoxLayout);
		createSpinBox();
		break;
	}
	layout()->setMargin(1);
}

void ControlSenderWidget::createDial()
{
	_dial = new QSynthKnob;
	setupSlider(_dial);
	_dial->setNotchesVisible(true);
	layout()->addWidget(_dial);
}

void ControlSenderWidget::createSlider(Qt::Orientation align)
{
	_slider = new QSlider(align);
	_slider->setMinimumHeight(_slider->sizeHint().height());
	_slider->setMinimumWidth(_slider->sizeHint().width());
	setupSlider(_slider);
	if (align == Qt::Vertical) {
		QWidget *hbox = new QWidget;
		hbox->setLayout(new QHBoxLayout);
		hbox->layout()->addWidget(_slider);
		layout()->addWidget(hbox);
	}
	else
		layout()->addWidget(_slider);
}

void ControlSenderWidget::setupSlider(QAbstractSlider *slider)
{
	slider->setMinimum(0);
	slider->setMaximum(int(ceil((_max - _min) / _step)));
	slider->setValue(int(floor(((_default - _min) / _step)+0.5)));

	connect(slider, SIGNAL(valueChanged(int)), 
			this, SLOT(stepControlChanged(int)));
}

void ControlSenderWidget::createSpinBox()
{
	_spinBox = new QDoubleSpinBox();
	layout()->addWidget(_spinBox);

	_spinBox->setMinimum(_min);
	_spinBox->setMaximum(_max);
	_spinBox->setSingleStep(_step);
	_spinBox->setValue(_default);

	connect(_spinBox, SIGNAL(valueChanged(double)), 
		this, SLOT(continuousControlChanged(double)));
}

void ControlSenderWidget::stepControlChanged(int value)
{
	if (_updating) return;
	_updating = true;

	double dvalue = _min + _step * value;
	if (_spinBox) _spinBox->setValue(dvalue);
	_sender->SendControl(mapValue(dvalue));

	_updating = false;
}

void ControlSenderWidget::continuousControlChanged(double value)
{
	if (_updating) return;
	_updating = true;

	int svalue = int(floor(((value - _min) / _step)+0.5));
	if (_slider) _slider->setValue(svalue);
	if (_dial) _dial->setValue(svalue);
	_sender->SendControl( mapValue(svalue) );

	_updating = false;
}

inline double ControlSenderWidget::mapValue(double value)
{
	double mapped_value = 0.;
	double tmp_max = 0.;
	switch( _mappingMode ) {
	case CLAM::OutControlSenderConfig::EMapping::eLinear:
		mapped_value = value;
		break;
	case CLAM::OutControlSenderConfig::EMapping::eInverted:
		mapped_value = fabs(_max - value + _min);
		break;
	case CLAM::OutControlSenderConfig::EMapping::eLog:
		CLAM_ASSERT(_max>=_min, "min > max in Log mapping!" );
		tmp_max=_max-_min;
		mapped_value = CLAM_pow((value-_min)/tmp_max,4.)*tmp_max + _min;
		break;
	case CLAM::OutControlSenderConfig::EMapping::eReverseLog:
		CLAM_ASSERT(_max>=_min, "min > max in ReverseLog mapping!" );
		if (value>=_max-0.01) return _max;
		tmp_max=_max-_min;
		mapped_value =  (1. - CLAM_exp(-(value-_min)/tmp_max*4.))*tmp_max + _min;
		break;
	default:
		CLAM_ASSERT(false,"Bad control mapping value");
	}
	return mapped_value;
}
