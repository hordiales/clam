#include <qapplication.h>
#include <qcursor.h>
#include <qsplashscreen.h>

#include "Annotator.hxx"


#ifdef WIN32
#include <CLAM/InitAudioIO.hxx>
#include <CLAM/InitProcessing.hxx>
#endif

int main( int argc, char ** argv )
{

#ifdef WIN32
	CLAM::ProcessingModule::init();
	CLAM::AudioIOModule::init();
#endif
	
	QApplication app( argc, argv );
	QCoreApplication::setOrganizationName("CLAM");
	QCoreApplication::setOrganizationDomain("clam.iua.upf.edu");
	QCoreApplication::setApplicationName("Music Annotator");
	Annotator * w = new Annotator(argc>1?argv[1]:"");
	w->show();
	return app.exec();
}
