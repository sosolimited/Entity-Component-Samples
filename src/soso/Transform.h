//
//  TransformComponent.h
//
//  Created by Soso Limited on 1/8/15.
//
//

#pragma once

#include "HierarchyComponentT.h"

namespace soso {

///
/// A hierarchical spatial transformation.
///
struct Transform : public HierarchyComponentT<Transform>
{
	Transform() = delete;
	using HierarchyComponentT<Transform>::HierarchyComponentT;
	Transform(entityx::Entity entity, const ci::vec3 &position, const ci::vec3 &scale = ci::vec3(1), const ci::vec3 &pivot = ci::vec3(0), const ci::quat &orientation = ci::quat() )
	: HierarchyComponentT(entity),
		position(position),
		scale(scale),
		pivot(pivot),
		orientation(orientation)
	{}

	ci::vec3	position = ci::vec3(0);
	ci::vec3	scale = ci::vec3(1);
	ci::vec3	pivot = ci::vec3(0); // relative center of orientation and scaling.
	ci::quat	orientation;

	const ci::mat4& worldTransform() const { return _world_transform; }
	const ci::mat4& localTransform() const { return _local_transform; }
	ci::vec3 worldPoint() const { return ci::vec3(worldTransform() *  ci::vec4(0, 0, 0, 1)); }

	/// Compose a transform into this transform's world transform.
	void composeTransform(const ci::mat4 &transform) {
		_local_transform = calcLocalTransform();
		_world_transform = transform * _local_transform;
	}

	ci::mat4 calcLocalTransform() const { return glm::translate(position + pivot) * glm::toMat4(orientation) * glm::scale(scale) * glm::translate(- pivot / scale); }

private:
	ci::mat4	_world_transform;
	ci::mat4	_local_transform;
};

#pragma mark - Free functions for creating hierarchies.

/// Make a hierarchy component for an entity.
inline Transform::Handle makeHierarchy(entityx::Entity a) {
	return a.has_component<Transform>() ? a.component<Transform>() : a.assign<Transform>(a);
}

/// Attach a child to a parent in a hierarchy.
inline void attachChildren(Transform::Handle root, entityx::Entity child) {
	root->appendChild(makeHierarchy(child));
}

/// Attach multiple children to a parent in a hierarchy.
template <typename ... Children>
void attachChildren(Transform::Handle root, entityx::Entity child1, entityx::Entity child2, Children ... children) {
	attachChildren(root, child1);
	attachChildren(root, child2, std::forward<Children>(children)...);
}

/// Create a hierarchy where all children are parented to a root.
/// Compose multiple calls to makeHierarchy to create deeper hierarchies.
template <typename ... Children>
entityx::Entity makeHierarchy(entityx::Entity root, Children... children) {
	auto root_handle = makeHierarchy(root);
	attachChildren(root_handle, std::forward<Children>(children)...);
	return root;
}

} // namespace soso
