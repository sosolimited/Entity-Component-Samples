//
//  VerletPhysicsSystem.cpp
//
//  Created by Soso Limited on 7/7/15.
//
//

#include "VerletPhysicsSystem.h"
#include "VerletBody.h"

#include "cinder/Log.h"
#include "cinder/Rand.h"

using namespace soso;
using namespace cinder;
using namespace entityx;

void VerletPhysicsSystem::update( EntityManager &entities, EventManager &events, TimeDelta dt )
{
  ComponentHandle<VerletBody> body;
  for( auto e : entities.entities_with_components( body ) )
  {
    auto &b = *body.get();
    auto current = b.position;
    auto velocity = (b.position - b.previous_position) * static_cast<float>((dt / previous_dt)) + b.acceleration * static_cast<float>(dt * dt);
    // Friction as viscous drag.
    velocity *= (1.0 - b.drag);

    /*
    // Friction as a fixed-ish force.
    // Perhaps do some kind of ground test and apply when things are on ground.
    auto l2 = glm::length2(velocity);
    if (l2 > 0.0f) {
      auto len = std::sqrt(l2);
      auto friction = (velocity / len) * std::min(b.friction, len);
      velocity -= friction;
    }
    */

    b.position += velocity;
    b.previous_position = current;

    // We reset the acceleration so other systems/effects can simply add forces each frame.
    // TODO: consider alternative approaches to this.
    b.acceleration = vec3(0);
    previous_dt = dt;
  }

  // solve constraints
  ComponentHandle<VerletDistanceConstraint> constraint;
  const auto constraint_iterations = 2;
  for( auto e : entities.entities_with_components( constraint ) )
  {
    if( (! constraint->a.valid()) || (! constraint->b.valid()) ) {
      // would be cooler if the bodies knew about all constraints on them so this couldn't happen.
      constraint.remove();
      continue;
    }

    for( int i = 0; i < constraint_iterations; i += 1 ) {
      auto &a = *constraint->a.get();
      auto &b = *constraint->b.get();

      auto center = (a.position + b.position) / 2.0f;
      auto delta = a.position - b.position;
      auto len = glm::length( delta );
      if( len < std::numeric_limits<float>::epsilon() ) {
        delta = randVec3();
        len = 1.0f;
      }
      delta *= constraint->distance / (len * 2.0f); // get half delta
      a.position = center + delta;
      b.position = center - delta;
    }
  }
}
