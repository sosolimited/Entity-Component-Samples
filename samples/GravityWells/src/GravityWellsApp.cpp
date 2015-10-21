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

///
/// @file GravityWells demonstrates creation of entities and some of the uses of components and systems.
/// This application, as a teaching tool, is much more heavily commented than a production codebase.
/// It shows the interplay of a handful of systems.
/// Click to add wandering orbs.
/// Move the mouse to control a gravity well.
///

class GravityWellsApp : public App {
public:
  GravityWellsApp();

  void setup() override;
  void mouseDown( MouseEvent event ) override;
  void update() override;
  void draw() override;

  entityx::Entity createFloater(const ci::vec3 &position);
  void createGravityWells();
  entityx::Entity createGravityWell(const ci::vec3 &position, float distance_falloff);

private:
  entityx::EventManager   events;
  entityx::EntityManager entities;
  entityx::SystemManager systems;

  ci::Timer               frame_timer;
  const pair<vec3, vec3> world_bounds = std::make_pair(vec3(0, 0, -640), vec3(640, 480, 640));
};

GravityWellsApp::GravityWellsApp()
: entities(events),
  systems(entities, events)
{}

void GravityWellsApp::setup()
{
  // Initialize any systems that need initializing here.
  systems.add<VerletPhysicsSystem>();
  systems.add<BehaviorSystem>(entities);
  // Calls each systems configure method.
  systems.configure();

  auto f = createFloater(vec3(getWindowCenter(), 0.0f));
  auto body = f.component<VerletBody>();
  body->nudge(vec3(500.0f, 0.0f, -500.0f));

  createGravityWells();
}

entityx::Entity GravityWellsApp::createFloater(const ci::vec3 &position)
{
  auto e = entities.create();
  e.assign<PhysicsAttraction>();
  e.assign<Bounded>(world_bounds.first, world_bounds.second);
  e.assign<LinearForce>(vec3(10.0f, 0.0f, 0.0f));
  e.assign<VerletBody>(position, 0.05f);

  return e;
}

void GravityWellsApp::createGravityWells()
{
  auto mouse_follower = createGravityWell(vec3(getWindowCenter(), 0), 100.0f);
  assignBehavior<MouseFollow>(mouse_follower, 2.4f);
  mouse_follower.component<VerletBody>()->drag = 0.24f;

  createGravityWell(vec3(100.0f, 100.0f, 50.0f), 150.0f);
  createGravityWell(vec3(400.0f, 300.0f, -50.0f), 100.0f);
  createGravityWell(vec3(550.0f, 200.0f, 50.0f), 60.0f);
}

entityx::Entity GravityWellsApp::createGravityWell(const ci::vec3 &position, float distance_falloff)
{
  // Create an entity in our world and assign components.
  auto e = entities.create();
  e.assign<VerletBody>(position);
  // Use the handle to the PhysicsAttractor component to modify the component after creation.
  auto attractor = e.assign<PhysicsAttractor>();
  attractor->distance_falloff = distance_falloff;
  attractor->strength = 0.66f;

  return e;
}

void GravityWellsApp::mouseDown(MouseEvent event)
{
  auto e = createFloater(vec3(event.getPos(), 0.0f));
  // Get the existing VerletBody component and modify it.
  e.component<VerletBody>()->drag = randFloat(0.04f, 0.08f);
  // Assign a WanderingForce component, since the entity didn't have one already.
  e.assign<WanderingForce>(vec3(20.0f, 20.0f, 1.0f));
}

void GravityWellsApp::update()
{
  auto dt = frame_timer.getSeconds();
  frame_timer.start();
  if ((dt < 1.0 / 120.0) || (dt > 1.0 / 10.0)) {
    dt = 1.0 / 60.0;
  }

  // Update all our systems to change the state of the world.
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

  // Loop through every entity with a physical location and draw something.
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
