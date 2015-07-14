//
//  PhysicsAttraction.h
//
//  Created by Soso Limited on 7/7/15.
//
//

#pragma once

#include "entityx/Entity.h"

namespace soso {

struct PhysicsAttraction
{
	explicit PhysicsAttraction(float strength = 1.0f)
	: strength( strength )
	{}

	float strength = 1.0f;
};

} // namespace soso
