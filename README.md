Entity-Component-Sample
=======================

Didactic sample applications built using Entity Component Systems on top of EntityX. View the samples in the `samples/` directory.

- Entity Creation
- Gravity Wells
- Star Clusters

Other places to read about entities:

- https://github.com/alecthomas/entityx
- http://cowboyprogramming.com/2007/01/05/evolve-your-heirachy/
- http://scottbilas.com/files/2002/gdc_san_jose/game_objects_slides.pdf

Building and Running
--------------------

### Building Cinder

Clone and build Cinder on your machine. Note that we clone recursively in order to get submodules initialized.

I keep a directory with a handful of Cinder versions on my machine. That way I can sketch things out a bit more quickly and keep old sketches working if they were built against a specific version of Cinder.

```
- Code
	- cinder
		- master
		- v0.8.6
		- v0.8.5
```

To get the master directory as above:

```
mkdir -p Code/cinder
cd Code/cinder
git clone --branch master git@github.com:sosolimited/Cinder.git master --recursive
cd master/xcode
./fullbuild.sh
```

To clone and build Cinder in a new directory ignoring folder structure, do the following:

```
git clone git@github.com:sosolimited/Cinder.git --recursive
cd Cinder/xcode
./fullbuild.sh
```

### Building these Samples

Clone this repository into the `blocks/` directory of a recent version of the Cinder master branch. Note that we clone recursively in order to get submodules initialized.

```
cd Cinder/blocks/
git clone git@github.com:sosolimited/Entity-Component-Sample.git --recursive
```

Open up the samples in XCode and you should be good to go.


What is an Entity?
------------------

Entity Component Systems are excellent for describing worlds of independent elements with varied behaviors. Within an ECS, objects in the world, `entities`, are just an aggregation of attributes, `components`, that happen to be associated with each other.

### Where did this idea come from?

Entity component systems as a design pattern come from the game industry. There, they are used to define game objects in a way that is more flexible than the previous pattern of a game object inheritance tree.

In an inheritance tree, a GameObject base class defines every common attribute and behavior. Subclasses specialize those behaviors through virtual method overrides and add other functionality as needed.

In an Entity Component System, an Entity is defined by the aggregation of a number of attributes, which are stored in components. components define the attributes needed to perform a given behavior. An entity is simply an aggregation of attributes called components.

Entity Component Systems are excellent for describing worlds of independent elements with varied behaviors. Within an ECS, objects in the world, `entities`, are just an aggregation of attributes, `components`, that happen to be associated with each other.

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

Below we discuss some common tasks and how you might accomplish them using Entities and Components. We made some common Systems and Components that can be adapted for most projects.

### The Lifecycle of an Entity

Entities are created and destroyed through an EntityManager. The manager keeps track of all the created entities. Assuming we don’t explicitly destroy an entity ourselves, it will be destroyed when the manager falls out of scope. Usually, this coincides with our program closing.

```c++
// Alpha:
auto e = entities.create();

// Omega:
e.destroy();
```

One thing to watch out for is losing track of entities. Most of the time, this isn't an issue. However, if you have entities that aren’t visible on screen it might not be obvious when they exist after you intended to destroy them. Be careful when creating entities that don’t have an obvious presence at runtime. Although the EntityManager still knows about them, their memory is effectively leaked if they don’t have any components attached.

### Adding Custom Behavior to a Specific Entity

Sometimes, you may want to give an entity a specific behavior that isn’t clearly modeled by any existing component or combination of components. Other times, you may want to provide an entity with a function that manipulates a handful of components at once (say, flipping out some content in a slideshow with a fancy animation).

We define a `BehaviorComponent` as a place to store these kinds of one-off behaviors for an Entity. By extending the `BehaviorBase` class, you can build your own interfaces to special behaviors and run custom functions on update and other events. The Behavior will be registered with the entity, so it will be cleaned up when the entity is destroyed. If you store your own reference to a Behavior, you will need to be careful not to use it once its entity has been destroyed.

Before you start making everything a Behavior, consider whether the behavior could be better modeled using a Component and System (or by adding a new System that manipulates existing components). You can also evaluate whether a Behavior makes more sense as a Component+System once you have implemented it as a Behavior.

### Grouping Entities Together

In addition to describing individual entity attributes, we can use components to describe relationships between entities. That means we can build scene graphs using components when we need them.

There are many things to consider when building up a hierarchy component. At its most basic, it should enable traversal of the hierarchy, providing ordered access to each entity along the way. In C++, we also care about object lifetime management, so we make the lifetime of branches dependent on their root. Here, we define a Hierarchy component template that provides the following:

1) Access to parent, self, and child entities.
2) Lifetime management. Since there is no garbage collection in standard c++, we make sure the leaves are cleaned up with the root of the hierarchy.
3) A template type defining the properties that it makes sense to keep in a hierarchical tree (position, transparency).

Using our Hierarchy component (and systems that care to traverse the hierarchy), we can make construction a group of items like the following straightforward:

```
Menu
	- MenuItemA
	- MenuItemB
		- PromoAnimation
		- ParticleEmitter
	- MenuItemC
```

First, we need to create the entities that will be in the group. Imagine that we have functions that create their respective entities and return the created entity. Creating the underlying objects would look like the following:

```c++
auto menu = entities.create();
auto menu_item_a = createMenuItem(entities, configuration);
auto menu_item_b = createMenuItem(entities, configuration);
auto menu_item_c = createMenuItem(entities, configuration);
auto animation = createPromoAnimation(entities);
auto particle_emitter = createParticleEmitter(entities);
```

Next, we need to group these entities hierarchically. We can do this with our makeHierarchy convenience function.

```c++
makeHierarchy(menu, menu_item_a, makeHierarchy(menu_item_b, animation, particle_emitter), menu_item_c);

// With different indentation, notice how the above code mirrors our diagram from above.
makeHierarchy(menu,
	menu_item_a,
	makeHierarchy(menu_item_b,
		animation,
		particle_emitter),
	menu_item_c);
```

Samples
-------

You can find the following samples in the `samples/` directory.

### Entity Creation

Demonstrates the basics of Entity creation, Component definition, and control through Systems.

### Gravity Wells

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

### Star Clusters

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
