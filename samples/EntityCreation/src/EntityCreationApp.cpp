#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class EntityCreationApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void EntityCreationApp::setup()
{
}

void EntityCreationApp::mouseDown( MouseEvent event )
{
}

void EntityCreationApp::update()
{
}

void EntityCreationApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( EntityCreationApp, RendererGl )
