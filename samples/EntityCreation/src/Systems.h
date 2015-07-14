//
//  Systems.h
//
//  Created by Soso Limited on 7/14/15.
//
//

#pragma once

#include "entityx/Entity.h"
#include "Components.h"
#include "Expires.h"
#include "cinder/Easing.h"

namespace soso {

/// Update position based on motion.
inline void applyMotion(entityx::EntityManager &entities, double dt)
{
	entityx::ComponentHandle<Position> position;
	entityx::ComponentHandle<Motion> motion;

	for (auto __unused e : entities.entities_with_components(position, motion))
	{
		position->position += motion->direction * motion->speed * static_cast<float>(dt);
	}
}

/// Change motion's speed as they near expiration.
inline void slowDownWithAge(entityx::EntityManager &entities)
{
	entityx::ComponentHandle<Expires> ec;
	entityx::ComponentHandle<Motion> mc;

	for (auto __unused e : entities.entities_with_components(ec, mc))
	{
		auto t = glm::clamp(ci::lmap(ec->time, 0.0f, 3.0f, 0.0f, 1.0f), 0.0f, 1.0f);
		mc->speed = ci::easeOutCubic(t) * mc->base_speed;
	}
}

/// Change circle's colors as they near expiration.
inline void fadeWithAge(entityx::EntityManager &entities)
{
	entityx::ComponentHandle<Expires> ec;
	entityx::ComponentHandle<Circle> cc;

	for (auto __unused e : entities.entities_with_components(ec, cc))
	{
		auto t = glm::clamp(ci::lmap(ec->time, 0.0f, 6.0f, 0.0f, 1.0f), 0.0f, 1.0f);
		cc->color = ci::Color::gray(glm::mix(0.1f, 1.0f, t));
	}
}

/// Returns a function that will wrap entitie positions around when they pass outside a rectangle.
auto createWrapFunction(const ci::Rectf &bounds) {

	return [bounds] (entityx::EntityManager &entities) {
		for (auto e : entities.entities_with_components<Position>()) {
			auto p = e.component<Position>();
			auto &pos = p->position;
			auto delta = ci::vec2(0);

			if (pos.x > bounds.x2) {
				delta.x -= bounds.getWidth();
			}
			else if (pos.x < bounds.x1) {
				delta.x += bounds.getWidth();
			}

			if (pos.y > bounds.y2) {
				delta.y -= bounds.getHeight();
			}
			else if (pos.y < bounds.y1) {
				delta.y += bounds.getHeight();
			}

			pos += delta;
		}
	};
}

} // namespace soso
