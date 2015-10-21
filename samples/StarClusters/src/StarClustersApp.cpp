#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"

#include "entityx/quick.h"
#include "Transform.h"

#include "cinder/Rand.h"
#include "Behaviors.h"
#include "Components.h"
#include "Systems.h"
#include "RenderLayer.h"

#include "RenderFunctions.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace soso;

///
/// Creates a planet and a smaller orbitting satellite.
/// Returns the planet entity.
///
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

///
/// Creates a solar system: a sun surrounded by orbitting planets.
/// Returns the sun entity.
///
entityx::Entity createSolarSystem(entityx::EntityManager &entities, const ci::vec3 &center)
{
  auto sun = entities.create();
  sun.assign<Circle>(50.0f, Color(CM_HSV, 0.1f, 0.4f, 1.0f));
  sun.assign<Transform>(sun, center);
  sun.assign<Draggable>(vec2(1, 1));
  sun.assign<Sun>();
  sun.assign<RenderLayer>(2);

  auto child_holder = entities.create();
  child_holder.assign<RenderLayer>(0);
  makeHierarchy(sun, child_holder);

  for (auto i = 0; i < 20; i += 1) {
    makeHierarchy(child_holder, createPlanetoid(entities));
  }

  return sun;
}

///
/// @file StarClustersApp demonstrates the creation of a hierarchy of entities.
/// This application, as a teaching tool, is much more heavily commented than a production codebase.
///
/// Drag suns to reposition them.
/// Press 'c' to create a new solar system.
/// Number keys cycle through render functions.
///
class StarClustersApp : public App {
public:
  StarClustersApp();

  void setup() override;
  void keyDown(KeyEvent event) override;
  void update() override;
  void draw() override;

  /// Make every sun smaller.
  void shrinkSolarSystems();
private:
  entityx::EventManager    _events;
  entityx::EntityManager  _entities;
  entityx::SystemManager  _systems;

  ci::Timer                _frame_timer;
  /// We specify the render function as a free function.
  /// This lets us swap out rendering approaches at runtime.
  using RenderFunction = std::function<void (entityx::EntityManager &)>;
  RenderFunction          _render_function = &renderCircles;
};

// Initialize our EntityManager and SystemManager to know about events and each other.
StarClustersApp::StarClustersApp()
: _entities(_events),
  _systems(_entities, _events)
{}

void StarClustersApp::setup()
{
  // Create our systems for controlling entity behavior.
  // We also use free functions for entity behavior, which don't need to be instantiated.
  _systems.add<BehaviorSystem>(_entities);
  _systems.add<DragSystem>(_entities);
  _systems.add<TransformSystem>();
  _systems.configure();

  // Create an initial solar system on screen.
  createSolarSystem(_entities, vec3(getWindowCenter(), 0.0f));
}

void StarClustersApp::keyDown(KeyEvent event)
{
  // 'c' creates a new solar system
  // Numbers change rendering modes.

  switch (event.getCode())
  {
    case KeyEvent::KEY_c:
    {
      shrinkSolarSystems();

      auto center = vec2(getWindowCenter());
      auto offset = randVec2() * vec2(getWindowSize()) * randFloat(0.1f, 0.5f);
      createSolarSystem(_entities, vec3(center + offset, 0.0f));
    }
    break;
    case KeyEvent::KEY_1:
      CI_LOG_I("Rendering circles with depth testing");
      _render_function = &renderCircles;
    break;
    case KeyEvent::KEY_2:
      CI_LOG_I("Rendering circles depth-sorted (back-to-front)");
      _render_function = &renderCirclesDepthSorted;
    break;
    case KeyEvent::KEY_3:
      CI_LOG_I("Rendering circles through scene graph.");
      _render_function = &renderCirclesHierarchically;
    break;
    case KeyEvent::KEY_4:
      CI_LOG_I("Rendering circles by layer.");
      _render_function = &renderCirclesByLayer;
    break;
    case KeyEvent::KEY_0:
      CI_LOG_I("Rendering everything as a circle.");
      _render_function = &renderAllEntitiesAsCircles;
    break;
    default:
    break;
  }
}

void StarClustersApp::shrinkSolarSystems()
{
  entityx::ComponentHandle<Transform> xf;
  entityx::ComponentHandle<Sun> sun;
  for (auto e : _entities.entities_with_components(xf, sun))
  {
    xf->scale *= 0.8f;
    if (xf->scale.x < 0.33f)
    {
      e.destroy();
    }
  }
}

void StarClustersApp::update()
{
  auto dt = _frame_timer.getSeconds();
  _frame_timer.start();
  if (dt > 0.5f || dt < 0.001f) {
    dt = 1.0 / 60.0;
  }
  _systems.update<BehaviorSystem>(dt);
  _systems.update<TransformSystem>(dt);
}

void StarClustersApp::draw()
{
  gl::clear( Color( 0, 0, 0 ) );
  gl::enableAlphaBlending();
  gl::color(ColorA(1.0f, 1.0f, 0.0f, 0.8f));
  gl::setMatricesWindowPersp(getWindowSize());

  // Render all the entities with our chosen render function.
  // By separating the rendering code from the entities themselves, it becomes
  // easy to swap out one rendering approach for another (as we do on keypress in this application).
  _render_function(_entities);
}

inline void prepareSettings(app::App::Settings *settings) {
  settings->setWindowSize(1024, 768);
}

CINDER_APP( StarClustersApp, RendererGl, prepareSettings )
