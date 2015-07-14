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

/// Follows the mouse while it's down, then destroys itself.
class DragTracker : public BehaviorBase {
public:
	using BehaviorBase::BehaviorBase;

	void mouseDrag(const ci::app::MouseEvent &event) override {
		entity().component<Position>()->position = event.getPos();
	}
	void mouseUp(const ci::app::MouseEvent &event) override {
		entity().destroy();
	}
};

} // namespace soso
