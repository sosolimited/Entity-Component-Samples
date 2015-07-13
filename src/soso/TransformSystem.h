//
//  TransformSystem.h
//
//  Created by Soso Limited on 1/8/15.
//
//

#pragma once

#include "entityx/System.h"

namespace soso {

/// Applies nested transformations and calculates transform matrices.
class TransformSystem : public entityx::System<TransformSystem>
{
public:
	void update( entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt ) override;

private:

};

} // namespace soso
