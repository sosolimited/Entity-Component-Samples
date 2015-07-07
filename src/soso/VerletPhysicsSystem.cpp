//
//  VerletPhysicsSystem.cpp
//
//  Created by Soso Limited on 7/7/15.
//
//

#include "VerletPhysicsSystem.h"
#include "VerletBody.h"

#include "cinder/Log.h"

using namespace soso;
using namespace cinder;
using namespace entityx;

void VerletPhysicsSystem::update( EntityManager &entities, EventManager &events, TimeDelta dt )
{
	ComponentHandle<VerletBody> body;
	for( auto __unused e : entities.entities_with_components( body ) )
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
}
