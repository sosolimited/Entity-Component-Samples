//
//  Draggable.h
//
//  Created by Soso Limited on 7/14/15.
//
//

#pragma once

#include "entityx/Entity.h"

namespace soso {

struct Draggable
{
	Draggable() = default;
	explicit Draggable(const ci::vec2 &axes)
	: axes(axes)
	{}

	ci::vec2 axes = ci::vec2(1.0f);
};

} // namespace soso
