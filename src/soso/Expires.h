//
//  Expires.h
//
//  Created by Soso Limited on 7/14/15.
//
//

#pragma once

#include "entityx/Entity.h"

namespace soso {

struct Expires
{
	Expires() = default;

	explicit Expires(float time)
	: time(time)
	{}

	float																	time = 1.0f;
	std::function<void (entityx::Entity)>	last_wish;
};

} // namespace soso
