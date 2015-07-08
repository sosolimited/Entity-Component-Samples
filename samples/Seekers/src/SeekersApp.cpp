#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "entityx/Entity.h"
#include "VerletPhysicsSystem.h"
#include "VerletBody.h"
#include "PhysicsAttractor.h"
#include "PhysicsAttraction.h"
#include "PhysicsAttractorSystem.h"
#include "BehaviorSystem.h"
#include "Behavior.h"
#include "MouseFollow.h"

#include "cinder/Rand.h"

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
	systems.add<PhysicsAttractorSystem>();
	systems.add<BehaviorSystem>( entities );
	systems.configure();

	auto e = entities.create();
	e.assign<PhysicsAttraction>();
	auto body = e.assign<VerletBody>( vec3( getWindowCenter(), 0.0f ) );
	body->drag = 0.05f;
	body->nudge(vec3(500.0f, 0.0f, -500.0f));

	auto attractor = entities.create();
	attractor.assign<VerletBody>( vec3( 320.0f, 240.0f, 100.0f ) );
	attractor.assign<PhysicsAttractor>();

	attractor = entities.create();
	attractor.assign<VerletBody>( vec3( 400.0f, 500.0f, -150.0f ) )->drag = 0.24f;
	attractor.assign<PhysicsAttractor>();
	assignBehavior<MouseFollow>( attractor, 2.4f );
}

void SeekersApp::mouseDown( MouseEvent event )
{
	auto e = entities.create();
	e.assign<PhysicsAttraction>( 0.5f );
	auto body = e.assign<VerletBody>( vec3( event.getPos(), 0.0f ) );
	body->drag = randFloat(0.04f, 0.08f);
}

void SeekersApp::update()
{
	auto dt = frameTimer.getSeconds();
	frameTimer.start();
	if ((dt < 1.0 / 120.0) || (dt > 1.0 / 10.0)) {
    dt = 1.0 / 60.0;
	}

	systems.update<BehaviorSystem>( dt );
	systems.update<PhysicsAttractorSystem>( dt );
	systems.update<VerletPhysicsSystem>( dt );
}

void SeekersApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatricesWindowPersp( getWindowSize() );

	entityx::ComponentHandle<VerletBody> body;
	for( auto e : entities.entities_with_components( body ) )
	{
//		gl::ScopedModelMatrix mat;
//		gl::translate( body->position );
//		gl::drawSolidCircle( vec2(0), 32.0f );
		auto attr = e.component<PhysicsAttractor>();
		auto size = attr ? 8.0f : 24.0f;
		gl::drawSphere( body->position, size );
		if( attr ) {
			gl::ScopedModelMatrix mat;
			gl::translate( body->position );
			gl::drawStrokedCircle( vec2( 0 ), attr->distance_falloff, 16 );
		}

	}
}

CINDER_APP( SeekersApp, RendererGl )
