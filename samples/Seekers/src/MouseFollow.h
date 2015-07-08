//
//  MouseFollow.h
//
//  Created by Soso Limited on 7/8/15.
//
//

#pragma once

#include "Behavior.h"

namespace soso {

struct VerletBody;

class MouseFollow : public BehaviorBase
{
public:
	MouseFollow( entityx::Entity entity, float strength = 1.0f );

	void mouseMove( const ci::app::MouseEvent &event ) final override;

	void update( double dt ) final override;

private:
	entityx::ComponentHandle<VerletBody>	_body;
	ci::vec3															_target;
	float																	_strength = 1.0f;
};

} // namespace soso
