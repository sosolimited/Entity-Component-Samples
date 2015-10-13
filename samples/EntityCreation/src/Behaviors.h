//
//  Behaviors.h
//
//  Created by Soso Limited on 7/14/15.
//
//

#pragma once

#include "entityx/Entity.h"
#include "Behavior.h"

namespace soso {

/// Moves controlled entity while mouse is dragged.
/// Calls optional callback when mouse is up; otherwise destroys entity.
class DragTracker : public BehaviorBase {
public:
	using Callback = std::function<void (const ci::vec2&, const ci::vec2&)>;

	DragTracker(entityx::Entity e)
	: BehaviorBase(e),
		_position(e.component<Position>())
	{}

	void mouseDrag(const ci::app::MouseEvent &event) override {
		_mouse_previous = _mouse;
		_mouse = event.getPos();
		_position->position = _mouse;
	}

	void mouseUp(const ci::app::MouseEvent &event) override {
		if (_mouse_up_callback) {
			_mouse_up_callback(_mouse, _mouse_previous);
		}
		else {
			entity().destroy();
		}
	}

	///
	void setMouseUpCallback(const Callback &callback) { _mouse_up_callback = callback; }

private:
	Callback _mouse_up_callback;
	ci::vec2 _mouse, _mouse_previous;
	entityx::ComponentHandle<Position> _position;
};

} // namespace soso
