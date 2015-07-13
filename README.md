Entity-Component-Sample
=======================

Sample implementation of an entity component system using EntityX.

Entity Component Systems are excellent for describing worlds of independent elements with varied behaviors. Within an ECS, objects in the world, `entities`, are just an aggregation of attributes, `components`, that happen to be associated with each other.

Other things to read about entities:

https://github.com/alecthomas/entityx
http://cowboyprogramming.com/2007/01/05/evolve-your-heirachy/
http://scottbilas.com/files/2002/gdc_san_jose/game_objects_slides.pdf


What is an Entity?
------------------

### Where did this idea come from?

Entity component systems as a design pattern come from the game industry. There, they are used to define game objects in a way that is more flexible than the previous pattern of a game object inheritance tree.

Inheritance tree:
GameObject base class defines every common attribute and behavior. Subclasses specialize those behaviors through virtual method overrides and add other functionality as needed.

Entity Component Systems:
Components define the attributes needed to perform a given behavior. Systems act on components to create a behavior.

Some possible attributes:
Positionable
Draggable
Physics Shape
Sprite Animation
Renderable Shape
Particle Emission

### Why Entities?

If you have a world of heterogeneous things, ECS can be a great choice.

Entities are good for providing flexibility in the following:
- Flexible type creation: easy to tweak behavior by adding new components and combining existing components differently.
- Clear separation of different functionality.
	- This makes reasoning about and testing code simpler.
- Memory layout can be more efficient.
	- EntityX puts every component in a semi-contiguous array, which makes it cache efficient.
	- Dynamic allocation (e.g. `make_shared`, `make_unique`, `new`), by contrast, doesn't necessarily group things together.

If you have a moment to build some tools to support your project:
- Runtime object definition.
- Easier to build tools that create game objects.
- Adding scripting support is comparatively straightforward.

### Why Not Entities?

- If your world is only comprised of homogeneous things, ECS doesn’t provide much benefit.
	- If you only have one type of thing in your world (and you _really_ aren't going to be adding others), you can just keep a vector<Type>.
- Debugging becomes more challenging. LLDB doesn’t have smarts when it comes to unpacking a set of components.

When you cross the line between straightforward to manage as a shallow inheritance hierarchy and better managed as a set of components and systems is a matter of judgment. We are still working that out with our projects.

### Great Ideas from Entities

We can still use some of the approaches used in Entity Component Systems when we aren't breaking our objects into components. If you decide not to use an ECS, keep the following lessons in mind.

Keep in mind that your project is likely to change while you are working on it. The flexibility provided by an ECS can make it easier to try out different ideas and change direction mid-development.

#### Act on similar things as a group.

Instead of having every object manage its own drawing or update behavior, define that behavior in a function that acts of a collection of those objects. First, this makes it easy to swap out one behavior for another without changing the collection of objects. Also, it makes defining certain behaviors, like flocking, more natural. Finally, batching can enable efficiencies in many operations, like rendering.

If you have many similar objects, consider moving some of the logic about what you do with those objects outside of them. You can think of this like a particle system, where the system applies functions to a collection of particles.

How to use Entities
-------------------

### Examples

#### Gravity Wells

Objects fly through the world and are pulled toward attractors.

Scene defined in JSON.

- Components:
	- VerletBody
		- position
		- previous position
		- friction
		- acceleration
	- Attractor (pulls things toward it)
		- strength
	- Attraction (is attracted to attractors)
		- strength
	- VerletConstraint
		- VerletBody a, b
		- fixed distance
	- RenderMesh
		- gl::BatchRef from svg
	- Name
		- name

#### Star Clusters

Satellites in layered orbit around central star.

- Components:
	- HierarchicalPosition version of VerletBody
		- orientation
		- pivot
		- scale
	- Shape
		- Cube, Sphere, Cone
		- Instanced rendering
	- Name

Entity Lifetime Management
--------------------------

```c++
// Alpha:
auto e = entities.create();

// Omega:
e.destroy();
```

Entity lifetimes are managed by the EntityManager. If we don’t do anything with an entity after it is created, it will be destroyed when the EntityManager falls out of scope. Usually, this coincides with our program closing.

We can also explicitly destroy entities before our program is closed.

One thing to watch out for is losing track of entities. Most of the time, this isn't an issue. However, if you have entities that aren’t visible on screen it might not be obvious when they exist after you intended to destroy them.

Adding Special Functions to a Particular Aggregation
----------------------------------------------------

### Aggregating Entities (Tree, Graph, or Group)

In addition to describing individual entity attributes, we can use components to describe relationships between entities.

There are lots of things to consider when building up a hierarchy component. At its most basic, it should enable traversal of the hierarchy, providing ordered access to each entity along the way. In C++, we also care about object lifetime management, so we make the lifetime of branches dependent on their root. Our Hierarchy component template provides the following:

1) Access to parent, self, and child entities.
2) Lifetime management. Since there is no garbage collection in standard c++, we make sure the leaves are cleaned up with the root of the hierarchy.
3) A template type defining the properties that it makes sense to keep in a hierarchical tree (position, transparency).

The HierarchyComponent allows us to compose things in a graph, so we can meaningfully treat them as a group.

For example, to create a menu with some special effects swirling around one of its entries, we might want something like the following:

```
Menu
	- MenuItemA
	- MenuItemB
		- PromoAnimation
		- ParticleEmitter
	- MenuItemC
```

How do we get things into the group?

First, we need to create the entities that will be in the group. Imagine that we have functions that create their respective entities and return the created entity.

```c++
auto menu = entities.create();
auto menu_item_a = createMenuItem(entities, configuration);
auto menu_item_b = createMenuItem(entities, configuration);
auto menu_item_c = createMenuItem(entities, configuration);
auto animation = createPromoAnimation(entities);
auto particle_emitter = createParticleEmitter(entities);
```

Next, we need to group these entities hierarchically. We can do this with our makeHierarchy function.

```c++
makeHierarchy(menu, menu_item_a, makeHierarchy(menu_item_b, animation, particle_emitter), menu_item_c);

// With different indentation, notice how the above code mirrors our diagram from above.
// We simply prepend `makeHierarchy(` to each entity that has children under it.
makeHierarchy(menu,
	menu_item_a,
	makeHierarchy(menu_item_b,
		animation,
		particle_emitter),
	menu_item_c);
```


### Custom Controls

InterfaceComponent

Sometimes you need an interface that affects a group of entities as a group, and it doesn't make sense to run through every entity in the world to find that group.

Alternatives to consider:
Can this interface be implemented through a Component?

### Custom Update Behavior

BehaviorComponent

Occasionally, you may want to run a special function every frame for just one component.
