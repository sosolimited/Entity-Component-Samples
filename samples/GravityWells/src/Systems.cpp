//
//  Systems.cpp
//
//  Created by Soso Limited on 7/15/15.
//
//

#include "Systems.h"
#include "Components.h"
#include "VerletBody.h"

#include "cinder/Rand.h"

using namespace soso;
using namespace entityx;

namespace {

ci::vec3 safeHeading(const ci::vec3 &velocity) {
	auto len = length(velocity);
	if (len > std::numeric_limits<float>::epsilon()) {
		return velocity / len;
	}
	return ci::vec3(1, 0, 0);
};

} // namespace

void soso::applyPhysicsAttraction(EntityManager &entities)
{
	ComponentHandle<VerletBody>					body;
	ComponentHandle<PhysicsAttraction>	attraction;

	for (auto __unused e : entities.entities_with_components(body, attraction))
	{
		ComponentHandle<PhysicsAttractor> attractor;
		ComponentHandle<VerletBody>				attractor_body;

		for (auto __unused e : entities.entities_with_components(attractor, attractor_body))
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

void soso::applyLinearForce(entityx::EntityManager &entities)
{
	ComponentHandle<LinearForce>  force;
	ComponentHandle<VerletBody>		body;
	for (auto __unused e : entities.entities_with_components(force, body)) {
		body->nudge(force->force);
	}
}

void soso::applyWanderingForce(entityx::EntityManager &entities)
{
	ComponentHandle<WanderingForce> force;
	ComponentHandle<VerletBody>			body;

	for (auto __unused e : entities.entities_with_components(force, body)) {
		auto heading = safeHeading(body->velocity());
		auto half_fov = force->fov_radians / 2;
		auto wander = ci::randFloat(- half_fov, half_fov);
		heading = glm::rotate(heading, wander, ci::vec3(0, 0, 1));

		auto f = heading * force->impulse;
		body->nudge(f);
	}
}

void soso::enforceBoundaries(entityx::EntityManager &entities)
{
	entityx::ComponentHandle<VerletBody> vc;
	entityx::ComponentHandle<Bounded> bc;
	for (auto e : entities.entities_with_components(vc, bc)) {
		if (! bc->contains(vc->position)) {
			e.destroy();
		}
	}
}

