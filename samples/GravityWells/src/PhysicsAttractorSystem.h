//
//  PhysicsAttractorSystem.h
//
//  Created by Soso Limited on 7/7/15.
//
//

#pragma once

#include "entityx/System.h"

namespace soso {

class PhysicsAttractorSystem : public entityx::System<PhysicsAttractorSystem>
{
public:
	void update( entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt ) override;

private:

};

} // namespace soso
