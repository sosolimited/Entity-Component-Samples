//
//  BehaviorSystem.cpp
//
//  Created by Soso Limited on 7/8/15.
//
//

#include "BehaviorSystem.h"
#include "Behavior.h"
#include "cinder/app/App.h"

using namespace soso;
using namespace cinder;
using namespace cinder::app;
using namespace entityx;

BehaviorSystem::BehaviorSystem( entityx::EntityManager &entities )
: _entities( entities )
{
	auto window = app::getWindow();
	auto mouse_move = [this] (MouseEvent &event) { mouseMove( event ); };
	auto mouse_down = [this] (MouseEvent &event) { mouseDown( event ); };
	auto mouse_drag = [this] (MouseEvent &event) { mouseDrag( event ); };
	auto mouse_up = [this] (MouseEvent &event) { mouseUp( event ); };

	_signal_connections.emplace_back( std::make_shared<ci::signals::ScopedConnection>( window->getSignalMouseMove().connect( mouse_move ) ) );
	_signal_connections.emplace_back( std::make_shared<ci::signals::ScopedConnection>( window->getSignalMouseDown().connect( mouse_down ) ) );
	_signal_connections.emplace_back( std::make_shared<ci::signals::ScopedConnection>( window->getSignalMouseDrag().connect( mouse_drag ) ) );
	_signal_connections.emplace_back( std::make_shared<ci::signals::ScopedConnection>( window->getSignalMouseUp().connect( mouse_up ) ) );
}

void BehaviorSystem::mouseDown( const ci::app::MouseEvent &event )
{
	ComponentHandle<BehaviorComponent> behavior;
	for( auto __unused e : _entities.entities_with_components( behavior ) )
	{
		auto &behaviors = behavior->behaviors;
		for( auto &b : behaviors ) {
			b->mouseDown( event );
		}
	}
}

void BehaviorSystem::mouseDrag( const ci::app::MouseEvent &event )
{
	ComponentHandle<BehaviorComponent> behavior;
	for( auto __unused e : _entities.entities_with_components( behavior ) )
	{
		auto &behaviors = behavior->behaviors;
		for( auto &b : behaviors ) {
			b->mouseDrag( event );
		}
	}
}

void BehaviorSystem::mouseMove( const ci::app::MouseEvent &event )
{
	ComponentHandle<BehaviorComponent> behavior;
	for( auto __unused e : _entities.entities_with_components( behavior ) )
	{
		auto &behaviors = behavior->behaviors;
		for( auto &b : behaviors ) {
			b->mouseMove( event );
		}
	}
}

void BehaviorSystem::mouseUp( const ci::app::MouseEvent &event )
{
	ComponentHandle<BehaviorComponent> behavior;
	for( auto __unused e : _entities.entities_with_components( behavior ) )
	{
		auto &behaviors = behavior->behaviors;
		for( auto &b : behaviors ) {
			b->mouseUp( event );
		}
	}
}

void BehaviorSystem::update( EntityManager &entities, EventManager &events, TimeDelta dt )
{

	ComponentHandle<BehaviorComponent> behavior;

	for( auto e : entities.entities_with_components( behavior ) )
	{
		auto &behaviors = behavior->behaviors;
		for( auto &b : behaviors ) {
			b->update( dt );
		}
	}

}
