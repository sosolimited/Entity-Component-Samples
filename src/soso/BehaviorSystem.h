//
//  BehaviorSystem.h
//
//  Created by Soso Limited on 7/8/15.
//
//

#pragma once

#include "entityx/System.h"

namespace soso {

class BehaviorSystem : public entityx::System<BehaviorSystem>
{
public:
  explicit BehaviorSystem(entityx::EntityManager &entities);

  void update( entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt ) override;

  void mouseMove( const ci::app::MouseEvent &event );
  void mouseDown( const ci::app::MouseEvent &event );
  void mouseDrag( const ci::app::MouseEvent &event );
  void mouseUp( const ci::app::MouseEvent &event );

private:
  using ScopedConnectionRef = std::shared_ptr<ci::signals::ScopedConnection>;
  std::vector<ScopedConnectionRef> _signal_connections;
  entityx::EntityManager          &_entities;
};

} // namespace soso
