//
//  ExpiresSystem.h
//
//  Created by Soso Limited on 7/14/15.
//
//

#pragma once

#include "entityx/System.h"

namespace soso {

class ExpiresSystem : public entityx::System<ExpiresSystem>
{
public:
  void update( entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt ) override;

private:

};

} // namespace soso
