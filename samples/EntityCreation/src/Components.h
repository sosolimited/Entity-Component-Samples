//
//  Components.h
//
//  Created by Soso Limited on 7/14/15.
//
//

#pragma once

#include "entityx/Entity.h"

namespace soso {

struct Position {
  Position() = default;
  explicit Position(const ci::vec2 &position)
  : position(position)
  {}

  ci::vec2 position;
};

struct Motion {
  Motion() = default;
  explicit Motion(const ci::vec2 &direction, float speed=1.0f)
  : direction(direction),
    speed(speed),
    base_speed(speed)
  {}

  ci::vec2  direction = ci::vec2(1.0f, 0.0f);
  float      speed = 1.0f;
  float      base_speed = 1.0f;
};

struct Circle {
  Circle() = default;
  explicit Circle(float diameter, const ci::Color &color = ci::Color(1.0f, 1.0f, 1.0f))
  : diameter(diameter),
    color(color)
  {}

  float radius() const { return diameter / 2.0f; }
  float      diameter = 0.0f;
  ci::Color color = ci::Color::white();
};

} // namespace soso
