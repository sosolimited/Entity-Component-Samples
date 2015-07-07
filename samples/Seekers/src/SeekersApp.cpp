#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class SeekersApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void SeekersApp::setup()
{
}

void SeekersApp::mouseDown( MouseEvent event )
{
}

void SeekersApp::update()
{
}

void SeekersApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( SeekersApp, RendererGl )
