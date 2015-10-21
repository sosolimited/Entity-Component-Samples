//
//  ExpiresSystem.cpp
//
//  Created by Soso Limited on 7/14/15.
//
//

#include "ExpiresSystem.h"
#include "Expires.h"

using namespace soso;
using namespace entityx;

void ExpiresSystem::update( EntityManager &entities, EventManager &events, TimeDelta dt )
{
  entityx::ComponentHandle<Expires> ec;

  for (auto e : entities.entities_with_components(ec)) {
    ec->time -= dt;
    if (ec->time < 0.0f) {
      if (ec->last_wish) {
        ec->last_wish(e);
      }
      e.destroy();
    }
  }
}
