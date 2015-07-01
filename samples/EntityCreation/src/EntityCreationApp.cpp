#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "entityx/System.h"
#include "TransformComponent.h"
#include "cinder/Perlin.h"

using namespace ci;
using namespace ci::app;
using namespace std;

struct Position {
	Position() = default;
	explicit Position(const ci::vec3 &position)
	: position(position)
	{}

	ci::vec3 position;
};

struct Verlet {
	Verlet() = default;
	ci::vec3 previous_position;
};

struct Circle {
	Circle() = default;
	explicit Circle(float diameter)
	: diameter(diameter)
	{}

	float radius() const { return diameter / 2.0f; }
	float diameter = 0.0f;
};

struct Flow {
	float influence = 1.0f;
};

struct Jitter {
	ci::vec3 range = ci::vec3(1.0f);
};

///
/// Explicitly inherit from system and receiver if you want a system to receive events.
///
class VerletSystem : public entityx::System<VerletSystem>, public entityx::Receiver<VerletSystem> {

public:

	void configure(entityx::EventManager &events) override {
		events.subscribe<entityx::ComponentAddedEvent<Verlet>>(*this);
	}

	void receive(const entityx::ComponentAddedEvent<Verlet> &event) {
		auto e = event.entity;
		auto c = event.component;
		auto p = e.has_component<Position>() ? e.component<Position>() : e.assign<Position>();
		c->previous_position = p->position;
	}

	void update(entityx::EntityManager &entities, entityx::EventManager &events, double dt) override {
		entityx::ComponentHandle<Verlet>		v;
		entityx::ComponentHandle<Position>	p;

		for (auto e : entities.entities_with_components(v, p)) {
			auto vel = p->position - v->previous_position;
			v->previous_position = p->position;
			p->position += vel * 0.96f;
		}
	}

private:

};

namespace {

auto createFlowFunction(float scale) {
	Perlin perlin;

	return [perlin, scale] (entityx::EntityManager &entities, double dt) {
		entityx::ComponentHandle<Position> ph;
		entityx::ComponentHandle<Flow>		 fh;

		for (auto e : entities.entities_with_components(ph, fh)) {
			ph->position += perlin.dfBm(ph->position * scale) * fh->influence;
		}
	};
}

auto createWrapFunction(const ci::Rectf &bounds) {

	return [bounds] (entityx::EntityManager &entities) {
		for (auto e : entities.entities_with_components<Position>()) {
			auto p = e.component<Position>();
			auto &pos = p->position;
			auto delta = vec3(0);

			if (pos.x > bounds.x2) {
				delta.x -= bounds.getWidth();
			}
			else if (pos.x < bounds.x1) {
				delta.x += bounds.getWidth();
			}

			if (pos.y > bounds.y2) {
				delta.y -= bounds.getHeight();
			}
			else if (pos.y < bounds.y1) {
				delta.y += bounds.getHeight();
			}

			pos += delta;
			if (length2(delta) > 0.0f && e.has_component<Verlet>()) {
				e.component<Verlet>()->previous_position += delta;
			}
		}
	};
}

auto perlinFlow = createFlowFunction(0.1f);
auto borderWrap = createWrapFunction(Rectf(0, 0, 640, 480));

} // namespace

class EntityCreationApp : public App {
public:
	EntityCreationApp();

	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
private:
	entityx::EventManager	 events;
	entityx::EntityManager entities;
	entityx::SystemManager systems;
};

EntityCreationApp::EntityCreationApp()
: entities(events),
	systems(entities, events)
{}

void EntityCreationApp::setup()
{
	systems.add<VerletSystem>();
	systems.configure();

	auto dot = entities.create();
	dot.assign<Position>(vec3(100, 100, 0));
	dot.assign<Verlet>();
	dot.assign<Circle>(12.0f);
	dot.assign<Flow>();
}

void EntityCreationApp::mouseDown( MouseEvent event )
{
}

void EntityCreationApp::update()
{
	perlinFlow(entities, 1.0 / 60.0);
	systems.update<VerletSystem>(1.0 / 60.0);
	borderWrap(entities);
}

void EntityCreationApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatricesWindowPersp(getWindowSize());

	entityx::ComponentHandle<Circle>		ch;
	entityx::ComponentHandle<Position>	ph;
	for (auto e : entities.entities_with_components(ch, ph)) {
		gl::ScopedModelMatrix mat;
		gl::translate(ph->position);
		gl::drawSolidCircle(vec2(0), ch->radius());
	}
}

CINDER_APP( EntityCreationApp, RendererGl )
