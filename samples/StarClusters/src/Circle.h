//
//  Circle.h
//
//  Created by Soso Limited on 9/9/15.
//
//

#pragma once

#include "entityx/Entity.h"

namespace soso {

struct Circle
{
	Circle() = default;
	Circle(float radius, const ci::Color &color = ci::Color(1.0f, 1.0f, 1.0f))
	: radius(radius),
		color(color)
	{}

	float			radius = 12.0f;
	ci::Color color = ci::Color(1.0f, 1.0f, 1.0f);
};

} // namespace soso
