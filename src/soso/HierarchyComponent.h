//
//  HierarchyComponentT.h
//
//  Created by David Wicks on 4/16/15.
//
//

#pragma once

#include "entityx/Entity.h"

namespace soso {

///
/// A component containing a hierarchy of entities.
///
class HierarchyComponent
{
public:
  using Handle = entityx::ComponentHandle<HierarchyComponent>;

  /// Returns an invalid handle to type.
  static Handle invalidHandle() { return Handle(); }

  /// Constructs an invalid HierarchyComponentT.
  HierarchyComponent() = delete;
  /// Constructs a HierarchyComponentT for the provided entity.
  /// We store the entity so during hierarchical iteration other components may be accessed.
  explicit HierarchyComponent(entityx::Entity entity)
  : _entity(entity)
  {}
  /// Remove references to self on destruction.
  /// Also destroy all children when destroyed.
  ~HierarchyComponent()
  {
    removeFromParent();
    for( auto &child : _children ) {
      child->_parent = invalidHandle();
      child->entity().destroy();
    }
  }

  /// Append an entity to this hierarchy. Returns a handle to that entity's HierarchyComponentT.
  Handle  appendChild(entityx::Entity entity);
  void    appendChild(Handle handle);
  /// Remove this branch from its parent hierarchy. Safe to call whether parent exists or not.
  void    removeFromParent();

  /// Returns the entity this component is attached to.
  /// Used during iteration
  entityx::Entity entity() const { return _entity; }
  /// Destroys this component's underlying entity (and hence this component).
  void destroy() { entity().destroy(); }

  /// Returns true iff this is the root of a hierarchy (has no parent).
  bool isRoot() const { return ! _parent; }
  /// Returns true iff this is the leaf of a hierarchy (has no children).
  bool isLeaf() const { return _children.empty(); }
  /// Returns a handle to this branch's parent.
  Handle parent() const { return _parent; }
  /// Returns a reference to this branch's _children.
  const std::vector<Handle>& children() { return _children; }
  /// Returns the number of direct _children in this hierarchy.
  size_t numChildren() const { return _children.size(); }
  void destroyChildren();

  /// Visit all of this components' descendents depth-first. TODO: separate visitor patterns from data.
  void descend(const std::function<void (const HierarchyComponent &parent, HierarchyComponent &child)> &fn);
  /// Visit all of this components' ancestors. TODO: separate visitor patterns from data.
  void ascend(const std::function<void (const HierarchyComponent &child, HierarchyComponent &parent)> &fn);

  /// Visit all of this components' descendents depth-first until the visitor function returns false.
  /// Stops descending when function returns true.
  void descendUntil(const std::function<bool (const HierarchyComponent &parent, HierarchyComponent &child)> &fn);

  /// Visit all of this components' ancestors until the visitor function returns false.
  /// Stops ascending when function returns true.
  void ascendUntil(const std::function<bool (const HierarchyComponent &child, HierarchyComponent &parent)> &fn);

private:
  entityx::Entity            _entity;
  Handle                    _parent = invalidHandle();
  std::vector<Handle>        _children;

  /// Returns a handle to this component.
  Handle handle() { return _entity.component<HierarchyComponent>(); }

  /// Remove an entity from this hierarchy. Use handle->removeFromParent() to safely remove an item from its hierarchy.
  void removeChild(Handle handle);
};

#pragma mark - Template Implementation

void HierarchyComponent::removeFromParent()
{
  if( _parent ) {
    _parent->removeChild( handle() );
  }
}

HierarchyComponent::Handle HierarchyComponent::appendChild(entityx::Entity child)
{
  auto handle = child.has_component<HierarchyComponent>() ? child.component<HierarchyComponent>() : child.assign<HierarchyComponent>(child);

  appendChild(handle);
  return handle;
}

void HierarchyComponent::appendChild(Handle child_handle)
{
  if (child_handle.get() != this) {
    child_handle->removeFromParent();
    child_handle->_parent = handle();
    _children.push_back(child_handle);
  }
}

void HierarchyComponent::removeChild(Handle handle)
{
  if (handle) {
    handle->_parent = invalidHandle();
    auto begin = std::remove_if(_children.begin(), _children.end(), [&](const Handle &e) { return e == handle; });
    _children.erase(begin, _children.end());
  }
}

void HierarchyComponent::destroyChildren()
{
  for( auto &child : _children ) {
    child->_parent = invalidHandle();
    child->entity().destroy();
  }
  _children.clear();
}

void HierarchyComponent::descend( const std::function<void (const HierarchyComponent&, HierarchyComponent&)> &fn )
{
  for( auto &c : _children ) {
    fn( *this, *c.get() );
    c->descend( fn );
  }
}

void HierarchyComponent::ascend( const std::function<void (const HierarchyComponent&, HierarchyComponent&)> &fn )
{
  if( _parent ) {
    fn( *this, *_parent.get() );
    _parent->ascend( fn );
  }
}

void HierarchyComponent::descendUntil( const std::function<bool (const HierarchyComponent&, HierarchyComponent&)> &fn )
{
  for( auto &c : _children ) {
    auto reached_end = fn( *this, *c.get() );
    if (! reached_end) {
      c->descendUntil( fn );
    }
  }
}

void HierarchyComponent::ascendUntil( const std::function<bool (const HierarchyComponent&, HierarchyComponent&)> &fn )
{
  if( _parent ) {
    auto reached_end = fn( *this, *_parent.get() );
    if (! reached_end) {
      _parent->ascendUntil( fn );
    }
  }
}

} // namespace soso
