//
//  TransformComponent.h
//
//  Created by Soso Limited on 1/8/15.
//
//

#pragma once

#include "HierarchyComponentT.h"

namespace soso {

// A nestable transformation.
struct TransformComponent : public HierarchyComponentT<TransformComponent>
{
	TransformComponent() = delete;
	explicit TransformComponent(entityx::Entity entity)
	: HierarchyComponentT(entity)
	{}

	ci::vec3	position = ci::vec3(0);
	ci::vec3	pivot = ci::vec3(0);
	ci::vec3	scale = ci::vec3(1);
	ci::quat	orientation;

	const ci::mat4& worldTransform() const { return _world_transform; }

private:
	ci::mat4	_world_transform;

	ci::mat4 calcLocalTransform() const { return glm::translate( position + pivot ) * glm::toMat4( orientation ) * glm::scale( scale ) * glm::translate( - pivot ); }
};

} // namespace soso
