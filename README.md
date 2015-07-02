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


Aggregating Entities (A Graph, Tree, or Group)
----------------------------------------------

So, how we we describe an aggregation of aggregations? We can use a component.

There are lots of things to consider when building up a hierarchy component, but at its most basic, it should enable traversal of the hierarchy and access to each entity along the way. We care about a few more things, like having a reason to traverse the hierarchy and object lifetime management. Our Hierarchy component template provides the following:

1) Ordered traversal of the group of entities.
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

### Custom Controls

InterfaceComponent

Sometimes you need an interface that affects a group of entities as a group, and it doesn't make sense to run through every entity in the world to find that group.

Alternatives to consider:
Can this interface be implemented through a Component?

### Custom Update Behavior

BehaviorComponent

Occasionally, you may want to run a special function every frame for just one component.

