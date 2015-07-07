#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "entityx/Entity.h"
#include "VerletPhysicsSystem.h"
#include "VerletBody.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace soso;

class SeekersApp : public App {
public:
	SeekersApp();

	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
private:
	entityx::EventManager	 events;
	entityx::EntityManager entities;
	entityx::SystemManager systems;

	ci::Timer								frameTimer;
};

SeekersApp::SeekersApp()
: entities( events ),
	systems( entities, events )
{}

void SeekersApp::setup()
{
	systems.add<VerletPhysicsSystem>();
	systems.configure();

	auto e = entities.create();
	auto body = e.assign<VerletBody>( vec3( getWindowCenter(), 0.0f ) );
	body->drag = 0.05f;
	body->nudge(vec3(500.0f, 0.0f, -500.0f));
}

void SeekersApp::mouseDown( MouseEvent event )
{
}

void SeekersApp::update()
{
	auto dt = frameTimer.getSeconds();
	frameTimer.start();
	if ((dt < 1.0 / 120.0) || (dt > 1.0 / 10.0)) {
    dt = 1.0 / 60.0;
	}

	systems.update<VerletPhysicsSystem>( dt );
}

void SeekersApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatricesWindowPersp( getWindowSize() );

	entityx::ComponentHandle<VerletBody> body;
	for( auto __unused e : entities.entities_with_components( body ) )
	{
//		gl::ScopedModelMatrix mat;
//		gl::translate( body->position );
//		gl::drawSolidCircle( vec2(0), 32.0f );
		gl::drawSphere( body->position, 24.0f );
	}
}

CINDER_APP( SeekersApp, RendererGl )
