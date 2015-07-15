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

struct PhysicsAttraction
{
	explicit PhysicsAttraction(float strength = 1.0f)
	: strength( strength )
	{}

	float strength = 1.0f;
};

} // namespace soso
