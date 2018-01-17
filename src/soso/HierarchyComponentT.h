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
/// A component containing a hierarchy of entities with a given Derived component.
/// Inherit into a Transform component to build a scene graph. e.g. `class Transform : public HierarchyComponentT<Transform>`
///
template <typename Derived>
class HierarchyComponentT
{
public:
  using Handle = entityx::ComponentHandle<Derived>;

  /// Returns an invalid handle to type.
  static constexpr Handle invalidHandle() { return Handle(); }

  /// Constructs an invalid HierarchyComponentT.
  HierarchyComponentT() = delete;
  /// Constructs a HierarchyComponentT for the provided entity.
  /// We store the entity so during hierarchical iteration other components may be accessed.
  explicit HierarchyComponentT(entityx::Entity entity)
  : _entity(entity)
  {}
  /// Remove references to self on destruction.
  /// Also destroy all children when destroyed.
  virtual ~HierarchyComponentT()
  {
    removeFromParent();
    for( auto &child : _children ) {
      child->_parent = invalidHandle();
      child->entity().destroy();
    }
  }

  /// Append an entity to this hierarchy. Returns a handle to that entity's HierarchyComponentT.
  inline Handle  appendChild(entityx::Entity entity);
  inline void  appendChild(Handle handle);
  /// Remove this branch from its parent hierarchy. Safe to call whether parent exists or not.
  inline void    removeFromParent();

  /// Returns the entity this component is attached to.
  /// Used during iteration
  inline entityx::Entity entity() const { return _entity; }
  /// Destroys this component's underlying entity (and hence this component).
  inline void destroy() { entity().destroy(); }

  /// Returns true iff this is the root of a hierarchy (has no parent).
  inline bool isRoot() const { return ! _parent; }
  /// Returns true iff this is the leaf of a hierarchy (has no children).
  inline bool isLeaf() const { return _children.empty(); }
  /// Returns a handle to this branch's parent.
  inline Handle parent() const { return _parent; }
  /// Returns a reference to this branch's _children.
  inline const std::vector<Handle>& children() const { return _children; }
  /// Returns the number of direct _children in this hierarchy.
  inline size_t numChildren() const { return _children.size(); }
  inline size_t numDescendants() const;
  inline void destroyChildren();

  /// Visit all of this components' descendents depth-first. TODO: separate visitor patterns from data.
  inline void descend(const std::function<void (const Derived &parent, Derived &child)> &fn);
  /// Visit all of this components' ancestors. TODO: separate visitor patterns from data.
  inline void ascend(const std::function<void (const Derived &child, Derived &parent)> &fn);

  /// Visit all of this components' descendents depth-first until the visitor function returns false.
  /// Stops descending when function returns true.
  inline void descendUntil(const std::function<bool (const Derived &parent, Derived &child)> &fn);

  /// Visit all of this components' ancestors until the visitor function returns false.
  /// Stops ascending when function returns true.
  inline void ascendUntil(const std::function<bool (const Derived &child, Derived &parent)> &fn);

private:
  entityx::Entity            _entity;
  Handle                    _parent = invalidHandle();
  std::vector<Handle>        _children;

  /// Returns a pointer to this as derived type.
  inline Derived* self() { return static_cast<Derived*>(this); }
  inline const Derived* self() const { return static_cast<const Derived*>(this); }
  /// Returns a handle to this component.
  inline Handle handle() { return _entity.component<Derived>(); }

  /// Remove an entity from this hierarchy. Use handle->removeFromParent() to safely remove an item from its hierarchy.
  inline void removeChild(entityx::ComponentHandle<Derived> handle);
};

//#pragma mark - Template Implementation

template <typename T>
void HierarchyComponentT<T>::removeFromParent()
{
  if( _parent ) {
    _parent->removeChild( handle() );
  }
}

template <typename T>
typename HierarchyComponentT<T>::Handle HierarchyComponentT<T>::appendChild(entityx::Entity child)
{
  auto handle = child.component<T>();
  if (! handle) {
    handle = child.assign<T>(child);
  }

  appendChild(handle);
  return handle;
}

template <typename T>
void HierarchyComponentT<T>::appendChild(Handle child_handle)
{
  if (child_handle.get() != self()) {
    child_handle->removeFromParent();
    child_handle->_parent = handle();
    _children.push_back(child_handle);
  }
}

template <typename T>
void HierarchyComponentT<T>::removeChild(entityx::ComponentHandle<T> handle)
{
  if (handle) {
    handle->_parent = invalidHandle();
    auto begin = std::remove_if(_children.begin(), _children.end(), [&](const Handle &e) { return e == handle; });
    _children.erase(begin, _children.end());
  }
}

template <typename T>
size_t HierarchyComponentT<T>::numDescendants() const
{
  // recursively count children
  using function = std::function<size_t (const T&, size_t)>;
  function recur = [&recur] (const T& handle, size_t count) {
    count += handle.numChildren();
    for (auto &child: handle.children())
    {
      count = recur(*child.get(), count);
    }
    return count;
  };

  return recur(*self(), 0);
}

template <typename T>
void HierarchyComponentT<T>::destroyChildren()
{
  for( auto &child : _children ) {
    child->_parent = invalidHandle();
    child->entity().destroy();
  }
  _children.clear();
}

template <typename T>
void HierarchyComponentT<T>::descend( const std::function<void (const T&, T&)> &fn )
{
  for( auto &c : _children ) {
    fn( *self(), *c.get() );
    c->descend( fn );
  }
}

template <typename T>
void HierarchyComponentT<T>::ascend( const std::function<void (const T&, T&)> &fn )
{
  if( _parent ) {
    fn( *self(), *_parent.get() );
    _parent->ascend( fn );
  }
}


template <typename T>
void HierarchyComponentT<T>::descendUntil( const std::function<bool (const T&, T&)> &fn )
{
  for( auto &c : _children ) {
    auto reached_end = fn( *self(), *c.get() );
    if (! reached_end) {
      c->descend( fn );
    }
  }
}

template <typename T>
void HierarchyComponentT<T>::ascendUntil( const std::function<bool (const T&, T&)> &fn )
{
  if( _parent ) {
    auto reached_end = fn( *self(), *_parent.get() );
    if (! reached_end) {
      _parent->ascend( fn );
    }
  }
}

} // namespace soso
