// generated by Fast Light User Interface Designer (fluid) version 1.0011

#include "Fl_Progress.hxx"
#include <cstdlib>

CLAM::Mutex Fl_Progress::smInstanceGuard;

Fl_Progress::Fl_Progress() {
  Fl_Window* w;
  { Fl_Window* o = mWindow = new Fl_Window(341, 100, "Progress");
    w = o;
    o->box(FL_UP_BOX);
    o->user_data((void*)(this));
    { Fl_Slider* o = mSlider = new Fl_Slider(5, 55, 330, 30);
      o->type(3);
      o->box(FL_THIN_DOWN_BOX);
      o->selection_color(60);
    }
    { Fl_Box* o = mLabel = new Fl_Box(5, 15, 330, 30);
      o->align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE);
    }
    o->set_modal();
    o->end();
  }
  timeout();
}

Fl_Progress::~Fl_Progress() 
{
	CLAM::Mutex::ScopedLock lock( smInstanceGuard );
	Fl::remove_timeout(s_timeout,this);
	delete mWindow;
	mWindow = NULL;
	mSlider = NULL;
	mLabel = NULL;
}

float Fl_Progress::getValue() const
{
	CLAM::Mutex::ScopedLock lock( const_cast<CLAM::Mutex& >(mValueGuard) );

	return mValue;
}

void Fl_Progress::setValue( float value )
{
	CLAM::Mutex::ScopedLock lock( mValueGuard );
	mValue = value;
}

void Fl_Progress::s_timeout(void* ptr) {
  ((Fl_Progress*)ptr)->timeout();
}

void Fl_Progress::timeout(void) 
{
	CLAM::Mutex::ScopedLock lock( smInstanceGuard );
	if ( mWindow != NULL ) 
	{
		Fl::add_timeout(0.03,s_timeout,this);
		if ( mSlider != NULL )
			mSlider->value(mValue);
	}
}
