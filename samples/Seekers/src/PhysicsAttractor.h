//
//  PhysicsAttractor.h
//
//  Created by Soso Limited on 7/7/15.
//
//

#pragma once

#include "entityx/Entity.h"

namespace soso {

struct PhysicsAttractor
{
	PhysicsAttractor() = default;

	float strength = 1.0f;
	float distance_falloff = 300.0f;
};

} // namespace soso
