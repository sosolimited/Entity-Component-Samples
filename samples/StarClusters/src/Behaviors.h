//
//  Behaviors.h
//
//  Created by Soso Limited on 9/9/15.
//
//

#pragma once

#include "soso/Behavior.h"
#include "soso/BehaviorSystem.h"
#include "soso/Transform.h"

namespace soso {

const double Tau = M_PI * 2;

///
/// Applies a rotation to a transform.
/// If you use a behavior like this frequently, consider encoding it into a Component + System.
///
class ContinuousRotation : public BehaviorBase
{
public:
	explicit ContinuousRotation(entityx::Entity entity, const ci::vec3 &axis, float turns_per_second)
	: BehaviorBase(entity),
		_axis(axis),
		_radians_per_second(Tau * turns_per_second)
	{
		_transform = entity.has_component<Transform>() ? entity.component<Transform>() : entity.assign<Transform>(entity);
	}

	void update(double dt) override {
		_transform->orientation *= glm::angleAxis<float>(_radians_per_second * dt, _axis);
		glm::normalize(_transform->orientation);
	}

private:
	float							_radians_per_second = 1.0f;
	ci::vec3					_axis = ci::vec3(0, 0, 1);
	Transform::Handle _transform;
};

} // namespace soso
