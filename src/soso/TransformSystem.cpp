//
//  TransformSystem.cpp
//
//  Created by Soso Limited on 1/8/15.
//
//

#include "HierarchyComponent.h"
#include "Transform.h"
#include "TransformSystem.h"

using namespace entityx;
using namespace cinder;
using namespace soso;

void TransformSystem::update(EntityManager &entities, EventManager &events,
                             TimeDelta dt) {
  ComponentHandle<Transform> transform;

  /// If/when entityx allows us to order our components in memory based on their
  /// hierarchy,
  /// we will be able to walk through and simply update in linear order.
  /// We could also do that if we wrote our own memory allocator for transforms
  /// (which we are unlikely to pursue).

  for (Entity e : entities.entities_with_components(transform)) {
    if (transform->isRoot()) {
      transform->composeTransform(mat4(1));
      transform->descend([](const Transform &parent, Transform &child) {
        child.composeTransform(parent.worldTransform());
      });
    }
  }
}
