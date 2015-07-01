#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "entityx/System.h"
#include "TransformComponent.h"
#include "cinder/Perlin.h"
#include "cinder/Rand.h"

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

struct Expires {
	Expires() = default;
	explicit Expires(float time)
	: time(time)
	{}

	float										time = 1.0f;
	std::function<void (entityx::Entity)>	lastWish;
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
			ph->position += perlin.dfBm(ph->position * scale) * fh->influence * vec3(1, 1, 0.1);
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

void updateExpires(entityx::EntityManager &entities, double dt) {
	entityx::ComponentHandle<Expires> h;
	for (auto e : entities.entities_with_components(h)) {
    h->time -= dt;
		if (h->time < 0.0f) {
			if (h->lastWish) {
				h->lastWish(e);
			}
			e.destroy();
		}
	}
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

	entityx::Entity createDot(const ci::vec3 &position, float diameter);
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

	auto dot = createDot(vec3(getWindowCenter(), 0.0f), 36.0f);
}

entityx::Entity EntityCreationApp::createDot(const ci::vec3 &position, float diameter)
{
	auto dot = entities.create();
	dot.assign<Position>(position);
	dot.assign<Verlet>();
	dot.assign<Circle>(diameter);
	dot.assign<Flow>();

	auto exp = dot.assign<Expires>(mix(2.0f, randFloat(4.0f, 6.0f), diameter / 36.0f));

	exp->lastWish = [this, diameter] (entityx::Entity e) {
		if (diameter > 10.0f) {
			auto pos = e.component<Position>()->position;
			auto l = createDot(pos, diameter * 0.8f);
			auto r = createDot(pos, diameter * 0.81f);

			auto dir = randVec3();

			l.component<Position>()->position -= dir * diameter * 0.5f;
			r.component<Position>()->position += dir * diameter * 0.5f;
		}
	};

	return dot;
}

void EntityCreationApp::mouseDown( MouseEvent event )
{
}

void EntityCreationApp::update()
{
	auto dt = 1.0 / 60.0;

	perlinFlow(entities, dt);
	systems.update<VerletSystem>(dt);
	borderWrap(entities);
	updateExpires(entities, dt);
}

void EntityCreationApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::setMatricesWindowPersp(getWindowSize());

	entityx::ComponentHandle<Circle>		ch;
	entityx::ComponentHandle<Position>	ph;
	for (auto e : entities.entities_with_components(ch, ph)) {
		gl::ScopedModelMatrix mat;
		gl::translate(vec2(ph->position));
		gl::drawSolidCircle(vec2(0), ch->radius());
	}
}

CINDER_APP( EntityCreationApp, RendererGl )
