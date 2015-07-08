//
//  VerletBody.h
//
//  Created by Soso Limited on 7/7/15.
//
//

#pragma once

#include "entityx/Entity.h"

namespace soso {

/// A point-mass for verlet simulation.
struct VerletBody
{

VerletBody() = default;
VerletBody(const ci::vec3 &position, float drag = 0.1f)
: position( position ),
	previous_position( position ),
	drag( drag )
{}

/// Change velocity so the body will move \a amount over one second if there is no friction.
void nudge(const ci::vec3 &amount) { acceleration += amount * 60.0f; }
/// Place the body at a given position with no velocity.
void place(const ci::vec3 &pos) { position = pos; previous_position = pos; }
/// Instantaneous velocity, assuming fixed timestep.
ci::vec3 velocity() const { return position - previous_position; }

ci::vec3	position;
ci::vec3	previous_position;
ci::vec3	acceleration;
float			drag = 0.1f;

};

/// A distance constraint between two bodies
struct VerletDistanceConstraint
{
	using BodyHandle = entityx::ComponentHandle<VerletBody>;

	VerletDistanceConstraint( BodyHandle a, BodyHandle b )
	: a( a ),
		b( b ),
		distance( glm::distance( a->position, b->position ) )
	{}

	VerletDistanceConstraint( BodyHandle a, BodyHandle b, float distance )
	: a( a ),
		b( b ),
		distance( distance )
	{}

	BodyHandle a, b;
	float distance;
};

} // namespace soso
