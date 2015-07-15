//
//  Systems.h
//
//  Created by Soso Limited on 7/15/15.
//
//

#pragma once

#include "entityx/System.h"

///
/// @file Custom systems for the GravityWells sample application.
/// In a larger application, we would write each system in a separate file.
///
/// Note that systems are essentially functions that operate on the set of entities.
/// When we want to associate some state with a system, we either curry it into another function,
/// or we inherit from the entityx::System class and store the state as member variables.
///

namespace soso {

/// Move attracted entities towards attractors.
void applyPhysicsAttraction(entityx::EntityManager &entities);

/// Apply LinearForce to VerletBody for anything that has both.
void applyLinearForce(entityx::EntityManager &entities);

/// Apply WanderingForce to VerletBody for anything that has both.
void applyWanderingForce(entityx::EntityManager &entities);

/// Destroy any entities that have wandered outside of their own boundaries.
void enforceBoundaries(entityx::EntityManager &entities);

} // namespace soso
