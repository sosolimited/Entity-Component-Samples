//
//  PhysicsAttractorSystem.cpp
//
//  Created by Soso Limited on 7/7/15.
//
//

#include "PhysicsAttractorSystem.h"
#include "PhysicsAttraction.h"
#include "PhysicsAttractor.h"
#include "VerletBody.h"

using namespace soso;
using namespace entityx;

void PhysicsAttractorSystem::update( EntityManager &entities, EventManager &events, TimeDelta dt )
{
	ComponentHandle<VerletBody>					body;
	ComponentHandle<PhysicsAttraction>	attraction;

	for( auto __unused e : entities.entities_with_components( body, attraction ) )
	{
		ComponentHandle<PhysicsAttractor> attractor;
		ComponentHandle<VerletBody>				attractor_body;

		for( auto __unused e : entities.entities_with_components( attractor, attractor_body ) )
		{
			auto delta = attractor_body->position - body->position;
			auto len = glm::length( delta );
			auto t = glm::clamp( len / attractor->distance_falloff, 0.0f, 1.0f );
			t = 1.0f - (t * t);
			auto force = delta * attractor->strength * attraction->strength * t;
			body->nudge( force );
		}
	}
}
