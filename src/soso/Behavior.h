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

	/// Call update function.
	virtual void update( entityx::TimeDelta dt ) {}

	virtual void mouseMove( const ci::app::MouseEvent &event ) {}
	virtual void mouseDown( const ci::app::MouseEvent &event ) {}
	virtual void mouseUp( const ci::app::MouseEvent &event ) {}

	entityx::Entity& entity() { return _entity; }

private:
	entityx::Entity	_entity;
};

template <typename B, typename ... Params>
std::shared_ptr<B> assignBehavior( entityx::Entity entity, Params&& ... params )
{
	auto behavior = std::make_shared<B>( entity, std::forward<Params>( params ) ... );
	auto component = entity.has_component<BehaviorComponent>() ? entity.component<BehaviorComponent>() : entity.assign<BehaviorComponent>();
	component->behaviors.push_back( behavior );
	return behavior;
}

} // namespace soso
