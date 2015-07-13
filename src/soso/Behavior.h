//
//  Behavior.h
//
//  Created by Soso Limited on 7/8/15.
//
//

#pragma once

#include "entityx/Entity.h"

namespace soso {

class BehaviorBase;
using BehaviorRef = std::shared_ptr<BehaviorBase>;

///
/// Stores list of behaviors applied to entity.
///
struct BehaviorComponent
{
	std::vector<BehaviorRef> behaviors;
};

///
/// Base class for custom entity behaviors.
/// Receives event callbacks and an update function.
///
class BehaviorBase
{
public:
	BehaviorBase() = delete;
	explicit BehaviorBase( entityx::Entity entity )
	: _entity( entity )
	{}

	virtual ~BehaviorBase() = default;

	virtual void update( entityx::TimeDelta dt ) {}

	virtual void mouseMove( const ci::app::MouseEvent &event ) {}
	virtual void mouseDown( const ci::app::MouseEvent &event ) {}
	virtual void mouseUp( const ci::app::MouseEvent &event ) {}

	entityx::Entity& entity() { return _entity; }

private:
	entityx::Entity	_entity;
};

/// Construct and assign a behavior of type B to an entity.
template <typename B, typename ... Params>
std::shared_ptr<B> assignBehavior( entityx::Entity entity, Params&& ... params )
{
	auto behavior = std::make_shared<B>( entity, std::forward<Params>( params ) ... );
	auto component = entity.has_component<BehaviorComponent>() ? entity.component<BehaviorComponent>() : entity.assign<BehaviorComponent>();
	component->behaviors.push_back( behavior );
	return behavior;
}

/*
// For now, we require assignBehavior to initialize the behavior with the entity.
// This prevents unexpected behavior if two entities have the same behavior attached to them.
// If we move to passing the entity into each virtual method, we could share behaviors across entities.
inline void assignBehavior( entityx::Entity entity, const BehaviorRef &behavior )
{
	auto component = entity.has_component<BehaviorComponent>() ? entity.component<BehaviorComponent>() : entity.assign<BehaviorComponent>();
	component->behaviors.push_back( behavior );
}
*/

/// Remove all behaviors of type B from entity.
/// e.g. removeBehaviorsOfType<Seeker>( entity );
template <typename B>
void removeBehaviorsOfType( entityx::Entity entity )
{
	auto component = entity.component<BehaviorComponent>();
	if (component) {
		auto &b = component->behaviors;
		b.erase( std::remove_if( b.begin(), b.end(), [] (const BehaviorRef &behavior) {
			return std::dynamic_pointer_cast<B>( behavior );
		} ), b.end() );
	}
}

/// Remove a specific behavior from an entity.
inline void removeBehavior( entityx::Entity entity, const BehaviorRef &behavior )
{
	auto component = entity.component<BehaviorComponent>();
	if (component) {
		auto &b = component->behaviors;
		b.erase( std::remove( b.begin(), b.end(), behavior ), b.end() );
	}
}

} // namespace soso
