//
//  Behaviors.h
//
//  Created by Soso Limited on 7/15/15.
//
//

#pragma once

#include "Behavior.h"

///
/// @file Custom behaviors for the GravityWells sample application.
/// In a larger application, we would write each behavior in a separate file.
///
/// Behaviors are components that manage their own update behavior.
/// They frequently modify the state of one or more other components.
///

namespace soso {

struct VerletBody;

///
/// MouseFollow behavior causes an entity with a VerletBody to seek the mouse.
///
class MouseFollow : public BehaviorBase
{
public:
  MouseFollow(entityx::Entity entity, float strength = 1.0f);

  void mouseMove(const ci::app::MouseEvent &event) final override;

  void update(double dt) final override;

private:
  entityx::ComponentHandle<VerletBody>  _body;
  ci::vec3                              _target;
  float                                  _strength = 1.0f;
};

} // namespace soso
