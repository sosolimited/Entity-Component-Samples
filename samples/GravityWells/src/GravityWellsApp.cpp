#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "entityx/Entity.h"
#include "VerletPhysicsSystem.h"
#include "VerletBody.h"
#include "BehaviorSystem.h"
#include "Behavior.h"

#include "Behaviors.h"
#include "Components.h"
#include "Systems.h"

#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace soso;

class GravityWellsApp : public App {
public:
	GravityWellsApp();

	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

	void createGravityWells();
	entityx::Entity createGravityWell(const ci::vec3 &position, float distance_falloff);
private:
	entityx::EventManager	 events;
	entityx::EntityManager entities;
	entityx::SystemManager systems;

	ci::Timer							 frame_timer;
	const pair<vec3, vec3> world_bounds = std::make_pair(vec3(0, 0, -640), vec3(640, 480, 640));
};

GravityWellsApp::GravityWellsApp()
: entities(events),
	systems(entities, events)
{}

void GravityWellsApp::setup()
{
	systems.add<VerletPhysicsSystem>();
	systems.add<BehaviorSystem>(entities);
	systems.configure();

	auto e = entities.create();
	e.assign<PhysicsAttraction>();
	e.assign<Bounded>(world_bounds.first, world_bounds.second);
	e.assign<LinearForce>(vec3(10.0f, 0.0f, 0.0f));
	auto body = e.assign<VerletBody>(vec3(getWindowCenter(), 0.0f));
	body->drag = 0.05f;
	body->nudge(vec3(500.0f, 0.0f, -500.0f));

	createGravityWells();
}

void GravityWellsApp::createGravityWells()
{
	auto mouse_follower = createGravityWell(vec3(getWindowCenter(), 0), 150.0f);
	assignBehavior<MouseFollow>(mouse_follower, 2.4f);
	mouse_follower.component<VerletBody>()->drag = 0.24f;

	createGravityWell(vec3(100.0f, 100.0f, 50.0f), 200.0f);
	createGravityWell(vec3(400.0f, 300.0f, -50.0f), 150.0f);
	createGravityWell(vec3(600.0f, 100.0f, 50.0f), 90.0f);
}

entityx::Entity GravityWellsApp::createGravityWell(const ci::vec3 &position, float distance_falloff)
{
	auto e = entities.create();
	e.assign<VerletBody>(position);
	auto attractor = e.assign<PhysicsAttractor>();
	attractor->distance_falloff = distance_falloff;
	attractor->strength = 0.66f;

	return e;
}

void GravityWellsApp::mouseDown(MouseEvent event)
{
	// Put into a chain with the previous entity.
	auto e = entities.create();
	e.assign<PhysicsAttraction>( 0.5f );
	e.assign<Bounded>(world_bounds.second, world_bounds.first);
	e.assign<WanderingForce>(vec3(20.0f, 10.0f, 1.0f));
	auto body = e.assign<VerletBody>(vec3(event.getPos(), 0.0f));
	body->drag = randFloat(0.04f, 0.08f);
}

void GravityWellsApp::update()
{
	auto dt = frame_timer.getSeconds();
	frame_timer.start();
	if ((dt < 1.0 / 120.0) || (dt > 1.0 / 10.0)) {
    dt = 1.0 / 60.0;
	}

	systems.update<BehaviorSystem>(dt);
	applyPhysicsAttraction(entities);
	applyLinearForce(entities);
	applyWanderingForce(entities);
	systems.update<VerletPhysicsSystem>(dt);
	enforceBoundaries(entities);
}

void GravityWellsApp::draw()
{
	gl::clear(Color(0, 0, 0));
	gl::setMatricesWindowPersp(getWindowSize());

	entityx::ComponentHandle<VerletBody> body;
	for (auto e : entities.entities_with_components(body))
	{
		auto attractor = e.component<PhysicsAttractor>();
		auto size = attractor ? 8.0f : 24.0f;
		gl::drawSphere(body->position, size);
		if (attractor)
		{
			gl::ScopedModelMatrix mat;
			gl::translate(body->position);
			gl::drawStrokedCircle(vec2(0), attractor->distance_falloff, 16);
		}
	}
}

CINDER_APP( GravityWellsApp, RendererGl )
