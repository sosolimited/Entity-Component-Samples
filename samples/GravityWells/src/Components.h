//
//  Components.h
//
//  Created by Soso Limited on 7/15/15.
//
//

#pragma once

#include "entityx/Entity.h"

///
/// @file Custom components for the GravityWells sample application.
/// In a larger application, we would write each component in a separate file.
///

namespace soso {

///
/// Describes how strongly this entity attracts or repels other entities.
/// Distance falloff is used to limit the attraction's range.
///
/// Used by the applyPhysicsAttraction function.
///
struct PhysicsAttractor
{
	PhysicsAttractor() = default;
	PhysicsAttractor(float strength, float distance_falloff)
	: strength(strength),
		distance_falloff(distance_falloff)
	{}

	float strength = 1.0f;
	float distance_falloff = 300.0f;
};

///
/// Describe how strongly this entity is attracted to attractors.
/// move away < 0
/// no attraction = 0,
/// move toward > 0
///
/// Used by the applyPhysicsAttraction function.
///
struct PhysicsAttraction
{
	explicit PhysicsAttraction(float strength = 1.0f)
	: strength( strength )
	{}

	float strength = 1.0f;
};

///
/// WanderingForce describes a force that may be applied to an entity to cause
/// it to move in a straight line.
///
/// Used by the applyLinearForce function.
///
struct LinearForce
{
	explicit LinearForce(const ci::vec3 &force)
	: force(force)
	{}

	ci::vec3 force;
};

///
/// WanderingForce describes a force that may be applied to an entity to cause
/// it to appear to wander in random directions.
///
/// Used by the applyWanderingForce function.
///
struct WanderingForce
{
	explicit WanderingForce(const ci::vec3 &impulses = ci::vec3(1.0f), float fov_radians = M_PI)
	: impulse(impulses),
		fov_radians(fov_radians)
	{}

	ci::vec3 impulse;
	float fov_radians;
};

///
/// Bounded describes a 3d box where an entity is bounded.
///
/// Used by the enforceBoundaries function.
///
struct Bounded
{
	Bounded(const ci::vec3 &minima, const ci::vec3 &maxima)
	: minima(glm::min(minima, maxima)),
		maxima(glm::max(minima, maxima))
	{}

	bool contains(const ci::vec3 &point) {
		// (minima <= point <= maxima)
		// see http://glm.g-truc.net/0.9.4/api/a00137.html for more on vector relational functions
		return glm::all(glm::lessThanEqual(minima, point)) && glm::all(glm::lessThanEqual(point, maxima));
	}

	ci::vec3 minima;
	ci::vec3 maxima;
};

} // namespace soso
