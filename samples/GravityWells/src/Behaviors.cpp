//
//  Behaviors.cpp
//
//  Created by Soso Limited on 7/15/15.
//
//

#include "Behaviors.h"
#include "VerletBody.h"

using namespace soso;
using namespace cinder;

MouseFollow::MouseFollow(entityx::Entity entity, float strength)
: BehaviorBase(entity),
  _strength(strength)
{
  _body = entity.has_component<VerletBody>() ? entity.component<VerletBody>() : entity.assign<VerletBody>();
}

void MouseFollow::mouseMove(const app::MouseEvent &event)
{
  _target = vec3( event.getPos(), 0.0f );
}

void MouseFollow::update(double dt)
{
  auto delta = _target - _body->position;
  if (glm::length2(delta) < 1.0f && glm::length2(_body->velocity()) < 1.0f)
  {
    _body->place(_target);
  }
  else
  {
    _body->nudge(delta * _strength);
  }
}
