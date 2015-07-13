//
//  VerletPhysicsSystem.h
//
//  Created by Soso Limited on 7/7/15.
//
//

#pragma once

#include "entityx/System.h"

namespace soso {

///
/// Performs time-corrected verlet integration.
///
class VerletPhysicsSystem : public entityx::System<VerletPhysicsSystem>
{
public:
	void update( entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt ) override;

private:
	entityx::TimeDelta	previous_dt = 1.0 / 60.0;
};

} // namespace soso
