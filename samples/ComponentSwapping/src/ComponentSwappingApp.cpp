#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Rand.h"

#include "entityx/Entity.h"
#include "soso/BehaviorSystem.h"
#include "soso/ExpiresSystem.h"
#include "soso/Expires.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace soso;

///
/// @file Example showing basic assignment and removal of components.
/// Use number keys 1-3 to add/remove different components from our entity.
///

///
/// Each struct can be used as a component to indicate properties and behaviors of an entity.
///
struct Oscillate
{
  ci::vec2  amount = ci::vec2(2.0f, 0.0f);
  double    time = 0.0;
};

struct Grow
{
  Grow() = default;
  Grow(float rate)
  : rate(rate)
  {}

  float rate = 1.0f;
};

struct Scintillate
{
  float rate = 1.0f;
};

struct Position
{
  ci::vec2 position;
};

struct Circle
{
  float radius = 24.0f;
};

struct HSVColor
{
  ci::vec3 hsv = ci::vec3(1.0f);
};

///
/// Free functions are the simplest kind of system.
/// They each look through the collection of entities,
/// select one or more components,
/// and do one thing based on those components.
///

void applyOscillation(entityx::EntityManager &entities, double dt)
{
  entityx::ComponentHandle<Oscillate> w;
  entityx::ComponentHandle<Position> p;
  for (auto __unused e: entities.entities_with_components(p, w))
  {
    w->time += dt;
    p->position += (float)cos(w->time) * w->amount;
  }
}

void applyGrowth(entityx::EntityManager &entities, double dt)
{
  entityx::ComponentHandle<Grow> g;
  entityx::ComponentHandle<Circle> c;

  for (auto __unused e: entities.entities_with_components(g, c))
  {
    c->radius += g->rate * dt;
  }
}

void applyScintillation(entityx::EntityManager &entities, double dt)
{
  entityx::ComponentHandle<HSVColor>    color;
  entityx::ComponentHandle<Scintillate> scint;
  for (auto __unused e: entities.entities_with_components(scint, color))
  {
    color->hsv.x += scint->rate * dt;
    if (color->hsv.x > 1.0f) {
      color->hsv.x -= 1.0f;
    }
  }
}

void drawEntities(entityx::EntityManager &entities)
{
  entityx::ComponentHandle<Position>  p;
  entityx::ComponentHandle<Circle>    c;

  for (auto __unused e: entities.entities_with_components(p, c))
  {
    gl::ScopedColor color_scope(Color::white());

    auto color = e.component<HSVColor>();
    if (color) {
      gl::color(Color(CM_HSV, color->hsv));
    }
    gl::drawSolidCircle(p->position, c->radius);
  }
}

class ComponentSwappingApp : public App
{
public:
  ComponentSwappingApp();
  void setup() override;
  /// On KeyDown, our application adds or removes components from our test entity.
  void keyDown(KeyEvent event) override;
  void update() override;
  void draw() override;


private:
  entityx::EventManager  _events;
  entityx::EntityManager _entities;
  entityx::SystemManager _systems;
  ci::Timer              _frame_timer;
  entityx::Entity         _entity;
};

ComponentSwappingApp::ComponentSwappingApp()
: _entities(_events),
  _systems(_entities, _events)
{}

void ComponentSwappingApp::setup()
{
  _systems.add<BehaviorSystem>(_entities);
  _systems.add<ExpiresSystem>();
  _systems.configure();

  _entity = _entities.create();
  _entity.assign<Circle>();
  auto pos = _entity.assign<Position>();
  pos->position = getWindowCenter();
  _entity.assign<Oscillate>();
}

void ComponentSwappingApp::keyDown(cinder::app::KeyEvent event)
{
  switch (event.getCode())
  {
    case KeyEvent::KEY_1:
      if (_entity.has_component<Oscillate>()) {
        _entity.remove<Oscillate>();
      }
      else {
        _entity.assign<Oscillate>();
      }
    break;
    case KeyEvent::KEY_2:
      if (_entity.has_component<Grow>()) {
        _entity.remove<Grow>();
      }
      else {
        auto rate = event.isShiftDown() ? -18.0f : 18.0f;
        _entity.assign<Grow>(rate);
      }
    break;
    case KeyEvent::KEY_3:
      if (_entity.has_component<Scintillate>()) {
        _entity.remove<Scintillate>();
        _entity.remove<HSVColor>();
      }
      else {
        _entity.assign<Scintillate>();
        _entity.assign<HSVColor>();
      }
    break;
    default:
    break;
  }
}

void ComponentSwappingApp::update()
{
  auto dt = _frame_timer.getSeconds();
  if (dt < std::numeric_limits<double>::epsilon() || dt > 0.1)
  {
    dt = 1.0 / 60.0;
  }
  _frame_timer.start();

  _systems.update<BehaviorSystem>(dt);
  _systems.update<ExpiresSystem>(dt);
  applyOscillation(_entities, dt);
  applyGrowth(_entities, dt);
  applyScintillation(_entities, dt);
}

void ComponentSwappingApp::draw()
{
  gl::clear();

  drawEntities(_entities);
}

CINDER_APP( ComponentSwappingApp, RendererGl )
