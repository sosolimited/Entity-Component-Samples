//
//  VerletBody.h
//
//  Created by Soso Limited on 7/7/15.
//
//

#pragma once

#include "entityx/Entity.h"

namespace soso {

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

ci::vec3	position;
ci::vec3	previous_position;
ci::vec3	acceleration;
float			drag = 0.1f;

};

} // namespace soso
