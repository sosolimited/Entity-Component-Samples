//
//  DragSystem.h
//
//  Created by Soso Limited on 7/14/15.
//
//

#pragma once

#include "entityx/System.h"

namespace soso {

///
/// A simple dragging system for moving entities relative to the mouse.
/// Does not handle complex hierachical transformations; you can only effectively drag roots.
///
class DragSystem : public entityx::System<DragSystem>
{
public:
	explicit DragSystem(entityx::EntityManager &entities)
	: _entities(entities)
	{}

	void configure(entityx::EventManager &events) override;
	void update(entityx::EntityManager &entities, entityx::EventManager &events, entityx::TimeDelta dt) override {}
	void mouseDown(ci::app::MouseEvent &event);
	void mouseDrag(ci::app::MouseEvent &event);

	/// Set the radius for grabbing things (if we don't have a component specifying the bounds of the shape).
	void setGrabRadius(float radius) { _grab_radius = radius; }

private:
	using ScopedConnectionRef = std::shared_ptr<ci::signals::ScopedConnection>;
	std::vector<ScopedConnectionRef> _signal_connections;
	entityx::EntityManager					&_entities;
	entityx::Entity									_dragging_entity;
	ci::vec3												_entity_start;
	ci::vec3												_drag_start;
	float														_grab_radius = 50.0f;
};

} // namespace soso
