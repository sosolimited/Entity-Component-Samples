#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "entityx/quick.h"
#include "Transform.h"
#include "TransformSystem.h"
#include "BehaviorSystem.h"
#include "RenderFunctions.h"

#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace soso;

entityx::Entity createPlanetoid(entityx::EntityManager &entities)
{
	auto size = randFloat(1.0f, 3.0f);
	auto planet = entities.create();
	planet.assign<Transform>(planet, vec3(randFloat() * getWindowWidth(), randFloat() * getWindowHeight(), 0.0f), vec3(size));

	auto moon = entities.create();
	moon.assign<Transform>(moon, vec3(size * 10.0f, size * 10.0f, 0.0f), vec3(size * 0.05f));
	makeHierarchy(planet, moon);

	return planet;
}

class HierarchicalOrbitsApp : public App {
public:
	HierarchicalOrbitsApp();

	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
private:
	entityx::EventManager	 events;
	entityx::EntityManager entities;
	entityx::SystemManager systems;
};

HierarchicalOrbitsApp::HierarchicalOrbitsApp()
: entities( events ),
	systems( entities, events )
{}

void HierarchicalOrbitsApp::setup()
{
	systems.add<TransformSystem>();
	systems.add<BehaviorSystem>(entities);
	systems.configure();

	createPlanetoid(entities);
}

void HierarchicalOrbitsApp::mouseDown( MouseEvent event )
{
}

void HierarchicalOrbitsApp::update()
{
	auto dt = 1.0 / 60.0;
	systems.update<BehaviorSystem>(dt);
	systems.update<TransformSystem>(dt);
}

void HierarchicalOrbitsApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatricesWindowPersp(getWindowSize());

//	renderAllEntitiesAsCircles(entities);
	renderEntitiesWithGraph(entities);
}

CINDER_APP( HierarchicalOrbitsApp, RendererGl )
