Entity-Component-Sample
=======================

This repository contains didactic sample applications built using an Entity Component Systems (ECS) architecture. They use the [Cinder](https://libcinder.org/) and [EntityX](https://github.com/alecthomas/entityx) libraries. Read more about ECS below, and open the samples in XCode to give them a whirl.

For everthing to work out of the box, clone this repository as a Cinder block.

### Contents
<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->


- [Understanding Entity Component Systems](#understanding-entity-component-systems)
  - [Components](#components)
  - [Entities](#entities)
  - [Systems](#systems)
  - [Visualizing Entity Component Systems](#visualizing-entity-component-systems)
  - [What We’re replacing: Classical Inheritance](#what-we%E2%80%99re-replacing-classical-inheritance)
  - [Why Entities?](#why-entities)
  - [Why Not Entities?](#why-not-entities)
  - [Further Reading on Entity-Component-Systems:](#further-reading-on-entity-component-systems)
- [Great Ideas in ECS](#great-ideas-in-ecs)
  - [Keep Interfaces Small](#keep-interfaces-small)
  - [Favor Composition over Inheritance](#favor-composition-over-inheritance)
  - [Act on similar things as a group.](#act-on-similar-things-as-a-group)
  - [Your Program is Just Data and Functions](#your-program-is-just-data-and-functions)
- [How to use Entities](#how-to-use-entities)
  - [The Lifecycle of an Entity](#the-lifecycle-of-an-entity)
  - [Adding and Removing Components](#adding-and-removing-components)
  - [Using a Set of Components](#using-a-set-of-components)
  - [Adding Custom Behavior to a Specific Entity](#adding-custom-behavior-to-a-specific-entity)
  - [Grouping Entities Together](#grouping-entities-together)
- [Samples](#samples)
  - [Entity Creation](#entity-creation)
  - [Star Clusters](#star-clusters)
  - [Gravity Wells](#gravity-wells)
- [Building and Running](#building-and-running)
  - [Building these Samples](#building-these-samples)
  - [Building Cinder](#building-cinder)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

Understanding Entity Component Systems
--------------------------------------

Entity component systems are a design pattern originating from the game industry. They are used to define game objects in a way that is simpler and more flexible than class-based inheritance. Instead of inheritance, they provide a straightforward way to compose behaviors to generate interesting objects in a game world.

Entity component systems consist of three main parts: `entities`, `components`, and `systems`. A `component` contains a bit of data. An `entity` is a collection of components. A `system` is a function that accepts entities as a parameter and does something based on their components.

### Components

A `component` stores the minimal amount of data relevant to some task. It could be the position of an object in space, its bounding volume, a name, a flag for special treatment, or a list of other entities. In general, we try to store one attribute per component.

### Entities

An `entity` is a collection of components. It relates the components to each other, so you can say that some entity has both the name component "ginger" and the color component "red" and the point of interest component.

### Systems

A `system` is a function that uses the data in components.

In order to make things happen in an ECS, all the entities are passed to functions that use one or more of the components to do their work. These functions are called systems. When systems need to store state (such as a collection of 3d meshes, movies, or audio files), they can be specified as an instance of a class or as a lambda closure.

### Visualizing Entity Component Systems

At its most basic an ECS can be thought of as a table with every component as a column and every entity as a row. To operate on a single component, we can just move down the column and grab each row that is filled. To operate on an entity, we can pick a row and see what components are filled in.

Because of the simplicity of the structure, it is easy to traverse the table and act on the components we care about in our systems.

[DIAGRAM: Components and Entities as a table]

[DIAGRAM: System traversing the table]

### What We’re replacing: Classical Inheritance

In a classical inheritance tree, a GameObject base class defines every common attribute and behavior. Subclasses specialize those behaviors through virtual method overrides and add other functionality as needed. At its most basic, the inheritance tree might look like the following:

```
class GameObject {
	virtual void update() {}
	// stores position, render data, whatnot
};

class FlockingThing : public GameObject {
	void update() override {} // does the flocking
};

class VideoPlayer : public GameObject  {
	void update() override {} // plays a video
};

class FlockingVideo : public FlockingThing {
	void update() override {} // duplicates behavior from video...
}
```

Because of the consistent interface, objects in the inheritance tree are easy to bundle into a scene graph. While at first this feels great, it can quickly take on many layers of responsibility.

This is complicated, because we are really dealing with two trees that are interleaved: a scene graph, and an inheritance graph.

Challenges arise in determining where particular functionality should go, and avoiding duplicate behavior. More challenging is untangling the tree of function calls when debugging or trying to eke out more performance from an application (should the blur be a child of the flocking element, or did we only use the graph for placement of those objects?).

So, while class-based hierarchies of GameObject-like structures can feel great (e.g. Flash sprites), they become unwieldy pretty quickly. Additionally, by hiding many kinds of complex behavior behind a simple-looking interface, they are difficult to reason about.

[DIAGRAM: Object inheritance tree]

### Why Entities?

If you have a world of heterogeneous things, ECS can be a great choice.

Entities provide interesting and useful characteristics:

- Flexible type creation.
	- tweak behavior by adding new components and combining existing components differently.
- Clear separation of different functionality.
	- This makes reasoning about and testing code simpler.
- Memory layout can be more efficient.
	- EntityX puts every component in a semi-contiguous array, which makes it cache efficient.
	- Dynamic allocation (e.g. `make_shared`, `make_unique`, `new`), by contrast, doesn't necessarily group things together.

If you have a moment to build some tools to support your project, ECS can become even more powerful.

- Runtime object definition.
- Easier to build tools that create game objects.
- Adding scripting support is comparatively straightforward.
- Data-driven objects. Very nearly everything can be laid out in a spreadsheet.

Of the above, the clarity around when things happen in your program, and what sections are responsible for what, are the biggest benefits of using an ECS.

### Why Not Entities?

You may not want or need to use entities for your project if the following is true:

- Everything in the world is the same or very similar (maybe make a particle system).
- There are only a few types of behaviors those things engage in.

Other downsides to using entities regardless of your project:

- Your debugger doesn't always capture the information you want. Since entities are just an id in a table, LLDB has a hard time converting them into their component values when debugging.
- You should be able to dereference component handles in the console (e.g. `expr xf.get()`), but it doesn't always work. If you need the debug info, dereference the handle in the body of the function so the debugger unpacks the info for you.

```c++
auto handle = entity.component<C>();
auto &c = handle.get(); // c will show up nicely in the debugger.
```

### Further Reading on Entity-Component-Systems:

- [EntityX, the ECS library we use](https://github.com/alecthomas/entityx).
- [Evolve Your Hierarchy](http://cowboyprogramming.com/2007/01/05/evolve-your-heirachy/)
- [A Data-Driven Game Object System](http://scottbilas.com/files/2002/gdc_san_jose/game_objects_slides.pdf)

Great Ideas in ECS
------------------

If you decide not to use an Entity Component System in your project, you can still use many of the concepts underlying the architecture.

Keep in mind that your project is likely to change while you are working on it. The flexibility provided by an ECS can make it easier to try out different ideas and change direction mid-development.

### Keep Interfaces Small

Keep the scope of what one object (or function) is responsible for small. If you have a scene graph describing how things are drawn, only use it to describe how things are drawn.

You can always compose objects and functions to produce more complicated effects, with the benefit of each stage being easier to understand. Which brings us to:

### Favor Composition over Inheritance

This one is from the original Design Patterns book. To add functionality, don’t inherit from something that provides it. Instead, compose an instance of something that provides that functionality.

With Entities, sophisticated composition is straightforward. We just add a component to our entity that has the new functionality we want. Then whatever functions or systems act

### Act on similar things as a group.

Instead of having every object manage its own drawing or update behavior, define that behavior in a function that acts of a collection of those objects. First, this makes it easy to swap out one behavior for another without changing the collection of objects. Also, it makes defining certain behaviors, like flocking, more natural. Finally, batching enables efficiencies in many operations, like rendering.

If you have many similar objects, consider moving some of the logic about what you do with those objects outside of them. You can think of this like a particle system, where the system applies functions to a collection of particles.

### Your Program is Just Data and Functions

It can help to think of your program as a set of data and a set of functions that operate on that data. Try to write your functions so they are easy to use across a range of data types. If you can clearly model the data and see the flow of its state changes, it becomes much easier to write functions that do what you want with that data.

How to use Entities
-------------------

Below we discuss some common tasks and how you might accomplish them using Entities and Components. The common Systems and Components referred to below are used in the samples and can be adapted for most projects.

The syntax below is based on the EntityX library and C++11. You may use a different library or language, and the same concepts will apply.

### The Lifecycle of an Entity

Entities are created and destroyed through an EntityManager. The manager keeps track of all the created entities. Assuming we don’t explicitly destroy an entity ourselves, it will be destroyed when the manager falls out of scope. Usually, this coincides with our program closing.

```c++
// Alpha:
auto e = entities.create();

// Omega:
e.destroy();
```

One thing to watch out for is losing track of entities. Most of the time, this isn't an issue. However, if you have entities that aren’t visible on screen it might not be obvious when they exist after you intended to destroy them. Be careful when creating entities that don’t have an obvious presence at runtime. Although the EntityManager still knows about them, their memory is effectively leaked if they don’t have any components attached.

### Adding and Removing Components

As mentioned above, entities are an aggregation of components. We build them up by assigning a number of components to the same entity.

```c++
auto e = entities.create();
e.assign<Transform>();
e.assign<Color>();
…
e.remove<Color>();
```

Construction parameters can be passed to components when they are assigned to give them initial values.

```c++
auto e = entities.create();
e.assign<Transform>(vec3(10, 10, 0));
e.assign<Color>(vec3(1.0, 0.5, 0.0));
```

Now, it is trivial to imagine how to model the above entity in e.g. JSON:

```
{
  transform: {[10, 10, 0]},
  color: {[1.0, 0.5, 0.0]}
}
```

Given the above JSON, you might imagine how you can describe a whole project’s worth of entities, including possible new object types, in data files (which you will want a project-specific tool to generate).

### Using a Set of Components

Systems look for entities that have a specific combination of components and use those components to perform actions. For example, a circle drawing system might draw every entity that has a transform and circle component, and set the color if the entity also has an optional style component.

```c++
ComponentHandle<Transform>  xf;
ComponentHandle<Circle>     cc;
for (auto e : entities.entities_with_components(xf, cc)) {
  auto sc = e.component<Style>();
  if (sc) {
    setColor(sc->color);
  }
  drawCircle(xf->position, cc->radius);
}
```

Inside the loop above, both the transform and circle handles are guaranteed to be valid. This guarantee is provided by the `entities_with_components` function, which skips any entity that doesn’t have all the components we specify in the function call. When we want to access a different component of the entity the requested components are associated with, we need to check whether that new component is valid before using it.

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

Next, we need to group these entities hierarchically. We can do this with our `makeHierarchy` convenience function. `makeHierarchy` constructs a hierarchy where the first parameter is the parent of every other parameter.

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

Click and drag to create new entities.

### Star Clusters

Satellites in orbit around central star. Demonstrates creation of a scene graph and various approaches to traversing the graph for rendering.

### Gravity Wells

Objects fly through the world and are pulled toward attractors.

Building and Running
--------------------

### Building these Samples

Clone this repository into the `blocks/` directory of a recent version of the Cinder master branch. Note that we clone recursively in order to get submodules initialized.

```
cd Cinder/blocks/
git clone git@github.com:sosolimited/Entity-Component-Sample.git --recursive
```

Now open up the samples in XCode and you should be good to go.

All samples were tested using in XCode 6.4. If you run into issues with an earlier version of XCode (like empty project files), please upgrade XCode.

### Building Cinder

If you don't already have Cinder installed, clone and build Cinder on your machine. Note that we clone recursively in order to get submodules initialized.

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
