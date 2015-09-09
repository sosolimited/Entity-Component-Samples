#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "entityx/quick.h"
#include "Transform.h"
#include "TransformSystem.h"
#include "Circle.h"
#include "Behavior.h"
#include "BehaviorSystem.h"
#include "RenderFunctions.h"
#include "Draggable.h"
#include "DragSystem.h"

#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace soso;

const double Tau = M_PI * 2;

///
/// Applies a rotation to a transform.
/// If you use a behavior like this frequently, consider encoding it into a Component + System.
///
class ContinuousRotation : public BehaviorBase
{
public:
	explicit ContinuousRotation(entityx::Entity entity, const ci::vec3 &axis, float turns_per_second)
	: BehaviorBase(entity),
		_axis(axis),
		_radians_per_second(Tau * turns_per_second)
	{
		_transform = entity.has_component<Transform>() ? entity.component<Transform>() : entity.assign<Transform>(entity);
	}

	void update(double dt) override {
		_transform->orientation *= glm::angleAxis<float>(_radians_per_second * dt, _axis);
		glm::normalize(_transform->orientation);
	}

private:
	float							_radians_per_second = 1.0f;
	ci::vec3					_axis = ci::vec3(0, 0, 1);
	Transform::Handle _transform;
};

///
/// New System/Component types can be prototyped as behaviors.
/// This shows the basics of making an entity draggable.
///
class DragBehavior : public BehaviorBase
{
public:
	using BehaviorBase::BehaviorBase;

	void mouseDown(const app::MouseEvent &event) override {
		auto xf = entity().component<Transform>();
		xf->position = vec3(event.getPos(), 0.0f);
	}
	void mouseDrag(const app::MouseEvent &event) override {
		auto xf = entity().component<Transform>();
		xf->position = vec3(event.getPos(), 0.0f);
	}
private:
};

entityx::Entity createPlanetoid(entityx::EntityManager &entities)
{
	auto size = randFloat(20.0f, 40.0f);
	auto planet = entities.create();
	auto distance = randFloat(60.0f, 360.0f);
	auto theta = randFloat(Tau);
	auto planet_pos = vec3(cos(theta) * distance, sin(theta) * distance, 0.0f);
	planet.assign<Transform>(planet, planet_pos, vec3(1.0f), - planet_pos);
	planet.assign<Circle>( size, Color(CM_HSV, 0.65f + randFloat(-0.01f, 0.01f), 0.8f, 0.9f + randFloat(-0.01f, 0.01f)));
	assignBehavior<ContinuousRotation>(planet, randVec3(), randFloat(0.18f, 0.22f));

	auto moon_pos = vec3(size, size, 0.0f) * randFloat(1.0f, 1.5f);
	auto moon = entities.create();
	moon.assign<Transform>(moon, moon_pos, vec3(1.0f), - moon_pos);
	moon.assign<Circle>(size * 0.33f, Color(CM_HSV, 0.5f + randFloat(-0.01f, 0.01f), 0.8f, 0.96f + randFloat(0.02f)));
	makeHierarchy(planet, moon);

	assignBehavior<ContinuousRotation>(moon, randVec3(), randFloat(0.8f, 0.9f));

	return planet;
}

class StarClustersApp : public App {
public:
	StarClustersApp();

	void setup() override;
	void keyDown(KeyEvent event) override;
	void update() override;
	void draw() override;

	void createSolarSystem(const ci::vec3 &center);
	void shrinkPastSolarSystems();
private:
	entityx::EventManager	 events;
	entityx::EntityManager entities;
	entityx::SystemManager systems;

	entityx::Entity sun;
	std::vector<entityx::Entity>	suns;

	ci::Timer frameTimer;
};

StarClustersApp::StarClustersApp()
: entities( events ),
	systems( entities, events )
{}

void StarClustersApp::setup()
{
	systems.add<BehaviorSystem>(entities);
	systems.add<DragSystem>(entities);
	systems.add<TransformSystem>();
	systems.configure();

	createSolarSystem(vec3(getWindowCenter(), 0.0f));
}

void StarClustersApp::keyDown(KeyEvent event)
{
	if (event.getCode() == KeyEvent::KEY_c)
	{
		shrinkPastSolarSystems();

		auto center = vec2(getWindowCenter());
		auto offset = randVec2() * vec2(getWindowSize()) * 0.5f;
		createSolarSystem(vec3(center + offset, 0.0f));
	}
}

void StarClustersApp::shrinkPastSolarSystems()
{
	for (auto &s : suns) {
		auto xf = s.component<Transform>();
		xf->scale *= 0.8f;
		if (xf->scale.x < 0.2f) {
			s.destroy();
		}
	}

	suns.erase(std::remove_if(suns.begin(), suns.end(), [] (entityx::Entity e) { return ! e.valid(); }), suns.end());
}

void StarClustersApp::createSolarSystem(const ci::vec3 &center)
{
	sun = entities.create();
	sun.assign<Circle>(50.0f, Color(CM_HSV, 0.1f, 0.4f, 1.0f));
	sun.assign<Transform>(sun, center);
	sun.assign<Draggable>(vec2(1, 1));

	for (auto i = 0; i < 20; i += 1) {
		makeHierarchy(sun, createPlanetoid(entities));
	}

	suns.push_back(sun);
}

void StarClustersApp::update()
{
	auto dt = frameTimer.getSeconds();
	frameTimer.start();
	if (dt > 0.5f || dt < 0.001f) {
    dt = 1.0 / 60.0;
	}
	systems.update<BehaviorSystem>(dt);
	systems.update<TransformSystem>(dt);
}

void StarClustersApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::enableAlphaBlending();
	gl::color(ColorA(1.0f, 1.0f, 0.0f, 0.8f));
	gl::setMatricesWindowPersp(getWindowSize());

//	renderAllEntitiesAsCircles(entities);
	renderCircles(entities);
//	renderCirclesDepthSorted(entities);
//	renderEntitiesWithGraph(entities);
}

inline void prepareSettings(app::App::Settings *settings) {
	settings->setWindowSize(1024, 768);
}

CINDER_APP( StarClustersApp, RendererGl, prepareSettings )
