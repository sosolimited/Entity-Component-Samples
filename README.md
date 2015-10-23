Entity Component Systems
========================

This repository contains didactic sample applications built using an Entity Component System (ECS) architecture. The samples rely on [Cinder](https://libcinder.org/) and [EntityX](https://github.com/alecthomas/entityx) to provide a graphics framework and an entity component framework, respectively.

An overview of the Entity Component System architecture and its benefits (and some drawbacks) continues below.

For everything to work out of the box, clone this repository as a Cinder block. The samples are set up for OSX and Xcode, but the code should work with any modern C++ compiler (and the ideas are agnostic).

Have fun!

### Contents
<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->


- [Understanding Entity Component Systems](#understanding-entity-component-systems)
  - [The Basics](#the-basics)
    - [Components](#components)
    - [Entities](#entities)
    - [Systems](#systems)
    - [Visualizing Entity Component Systems](#visualizing-entity-component-systems)
  - [Considerations when choosing an ECS architecture](#considerations-when-choosing-an-ecs-architecture)
    - [Why entities?](#why-entities)
    - [Why not entities?](#why-not-entities)
    - [What we are replacing](#what-we-are-replacing)
  - [Great ideas in ECS](#great-ideas-in-ecs)
    - [Favor composition over inheritance](#favor-composition-over-inheritance)
    - [Keep interfaces small](#keep-interfaces-small)
    - [Act on similar things as a group](#act-on-similar-things-as-a-group)
    - [Separate your data and the functions that act on it](#separate-your-data-and-the-functions-that-act-on-it)
  - [Further reading](#further-reading)
- [Entities in Practice](#entities-in-practice)
  - [The lifecycle of an entity](#the-lifecycle-of-an-entity)
  - [Adding and Removing Components](#adding-and-removing-components)
  - [Using components in systems](#using-components-in-systems)
  - [Adding custom behavior to a single entity](#adding-custom-behavior-to-a-single-entity)
  - [Grouping entities together](#grouping-entities-together)
  - [Things to watch out for](#things-to-watch-out-for)
- [Building this project](#building-this-project)
  - [Cinder](#cinder)
  - [Samples](#samples)
  - [Project template](#project-template)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

Understanding Entity Component Systems
--------------------------------------

### The Basics

Entity component systems are a design pattern originating from the game industry. They are used to define game objects in a way that is simpler and more flexible than class-based inheritance. They provide a straightforward way to compose behaviors to generate interesting objects in a game world.

Entity component systems consist of three main parts: `entities`, `components`, and `systems`. A `component` contains a bit of data. An `entity` is a collection of related components. A `system` is a function that accepts entities as a parameter and does something based on their components.

#### Components

A `component` stores the minimal amount of data relevant to some task. It could be the position of an object in space, its bounding volume, a name, a flag for special treatment, or a list of other entities.

#### Entities

An `entity` is a collection of components. It relates the components to each other, so you can say that some entity has three components: the name "ginger", the color "brown", and the "point of interest".

#### Systems

A `system` is a function that uses the data stored in components.

In order to make things happen in an ECS, entities are passed to functions that use one or more component(s) to do their work. These functions are called systems. When systems need to store state (such as a collection of 3d meshes, movies, or audio files), they can be specified as an instance of a class or as a lambda closure.

#### Visualizing Entity Component Systems

An Entity Component System can be visualized as a table with columns of components and rows of entities. To operate on a single component, we select its column and look at each cell. To operate on an entity, we selects its row and look at each cell.

![entity-component-table](https://cloud.githubusercontent.com/assets/81553/10465153/5ef5e228-71bb-11e5-92af-261da2752cca.png)

Because of the simplicity of the structure, it is easy to traverse the table and act on the components we care about in our systems. For example, a spatialized audio system might traverse the position and sound components and play audio for all the entities where both exist.

![entity-component-audio](https://cloud.githubusercontent.com/assets/81553/10465305/31c7f75e-71bc-11e5-9aad-dfa7ed1da6a8.png)

A rendering system can traverse all the position and shape components to decide what to draw on screen.
![entity-component-rendering](https://cloud.githubusercontent.com/assets/81553/10465399/aa869efc-71bc-11e5-9fb9-0985d1812c98.png)


### Considerations when choosing an ECS architecture

If your project is small enough, you may not need to worry about architecting it all that carefully. In that case, you can likely fit everything in a single file and have it work fine. As projects grow larger, however, you will almost certainly want a more general abstraction of your virtual world. At that point, using an ECS to organize your project can be very beneficial.

Entity Component Systems provide a flexible way to describe a virtual world and a way to easily find objects in that world. While things aren’t all roses with entities, they are generally better than the alternative class hierarchy.

#### Why entities?

Entities naturally model heterogeneous objects. They provide a single type that can describe many kinds of objects and easily access the attributes you want later.

Entities provide interesting and useful characteristics:

- Flexible type creation.
  - tweak behavior by adding new components and combining existing components differently.
- Runtime object definition.
  - Add and remove components at runtime to change how an object behaves.
- Clear separation of different functionality.
  - This makes reasoning about and testing code simpler.
- Easy to add new behaviors.
  - Simply create a new component and system, and tack it on to an existing entity.
- Memory layout can be more efficient.
  - EntityX puts every component in a semi-contiguous array, which makes it cache efficient.
  - Dynamic allocation (e.g. `make_shared`, `make_unique`, `new`), by contrast, doesn't necessarily group things together.
- Easier tool building since world is just data.

The main reason we choose entity component systems is that they encourage composition. This makes the division of responsibility within a program clearer and makes it easier to change out one behavior for another.

#### Why not entities?

You may not want or need to use entities for your project if the following is true:

- Everything in the world is the same or very similar (maybe make a particle system).
- There are only a few types of behaviors those things engage in.
- Your programming language provides other powerful object composition techniques.
- You aren’t modeling a world full of stuff.

#### What we are replacing

We’ve talked down class hierarchies along the way, so now let’s talk briefly about what they are.

A classical inheritance hierarchy provides an alternative way to model a virtual world. I strongly discourage this alternative. Think of the platypus: it is easy to describe each part that makes it up (bill, egg-laying, fur, poison claws), but hard to categorize since those parts are associated with different branches of traditional animal taxonomy (birds, reptiles/birds, mammals, ?). Classical inheritance trees force us to put things in a taxonomy, despite it often being easier to describe them by their component parts.

In a classical inheritance tree, a base class defines every common attribute and behavior of objects in the game world. Subclasses specialize those behaviors through virtual method overrides and add other functionality as needed.

Because of the consistent interface, it is easy to store objects from an inheritance tree in a single container. While at first this abstraction feels good, it quickly becomes confusing: the interleaving of types within a container also interleaves the order in which different sections of code are run, making it difficult to reason about how the state of the world changes over time.

Additionally, an inheritance hierarchy introduces new questions about where functionality belongs in a project. It becomes challenging to avoid duplicating behavior across multiple child types or storing extraneous behavior in a parent type.

If your world only consists of a single or very few types of things, a shallow class hierarchy might still be a good fit. However, if you want to change out behavior on the fly or have more organized control over how your objects are managed, having functionality dispersed across many subclasses and their parent class can be confusing.

### Great ideas in ECS

Entity Component Systems encourage some good habits in your programming by making certain roads easier to follow. If you decide not to use an Entity Component System in your project, you can still use many of the concepts underlying the architecture.

#### Favor composition over inheritance

Composition is the big idea underlying entity component systems, and it is a concept discussed in the original [Design Patterns](https://en.wikipedia.org/wiki/Design_Patterns) book. This is the old is-a versus has-a relationship from OOP, where you should almost always choose has-a.

To add functionality to an object, don’t inherit from something that provides it. Instead, compose something that provides that functionality. This allows for easy switching of behavior and the combination of many behaviors. In a class, composition is simply adding member variables.

In an entity component system, composition is simply adding components to an entity. Entities wouldn’t mean anything at all without the components they are composed of (see how often forms of the word composition show up there?). An extra-nice feature of composition in an ECS is that it can be done at run-time.

#### Keep interfaces small

Keep the scope of what one object (or function) is responsible for small. For example, if you have a scene graph object describing how things are laid out in space, only use it to describe how things are laid out in space. Keep the management of the sights, sounds, and smells of those things separate.

By breaking the world down into components and systems, ECS encourages us to think about only the pieces of data we really need inside our systems.

#### Act on similar things as a group

If you have many similar objects, consider moving some of the logic about what you do with those objects outside of them. You can think of this like a particle system, where the system applies functions to a collection of particles, or as a rendering system, which can find efficiencies in drawing a group of objects that the individual objects cannot.

ECS encourages batch processing, as it splits behavior into systems that act on a collection of components.

#### Separate your data and the functions that act on it

Keeping data simple makes it easy to write a range of functions that can act on it. Additionally, having functions separate from data makes it easier to generalize the functions to work on a range of types of data.

Systems being kept separate from components in ECS makes it easy to swap out how you manage the behavior of the world without changing your model of the world. The underlying data stays the same, but you can try out many different approaches to using it (and even swap them out at runtime).

### Further reading

- [EntityX, the ECS library we use](https://github.com/alecthomas/entityx).
- [Evolve Your Hierarchy](http://cowboyprogramming.com/2007/01/05/evolve-your-heirachy/)
- [A Data-Driven Game Object System](http://scottbilas.com/files/2002/gdc_san_jose/game_objects_slides.pdf)


Entities in Practice
--------------------

Below we discuss some common tasks and how you might accomplish them using Entities and Components. The common Systems and Components referred to below are used in the samples and can be adapted for most projects.

The syntax below is based on the EntityX library and C++11. You may use a different library or language, and the same concepts will apply.

### The lifecycle of an entity

Entities are created and destroyed through an EntityManager. The manager keeps track of all the created entities. Assuming we don’t explicitly destroy an entity ourselves, it will be destroyed when the manager falls out of scope. Usually, this coincides with our program closing.

```c++
// Alpha:
auto e = entities.create();

// Omega:
e.destroy();
```

One thing to watch out for is losing track of entities. Most of the time, this isn't an issue. However, if you have entities that aren’t visible on screen it might not be obvious when they exist after you intended to destroy them.

### Adding and Removing Components

We define an entity’s behavior by assigning a number of components to it.

```c++
auto e = entities.create();
e.assign<Transform>();
e.assign<Color>();
e.assign<Oscillate>();
```

When you no longer want an entity to have an attribute, simply remove the relevant component.

```c++
e.remove<Oscillate>();
```

If you want to initialize a component with values other than its defaults, you can pass constructor parameters to the assign method.

```c++
auto e = entities.create();
e.assign<Transform>(vec3(10, 10, 0));
e.assign<Color>(vec3(1.0, 0.5, 0.0));
```

If you want to edit a component after assignment, use the returned handle.

```c++
auto transform_handle = e.assign<Transform>();
transform_handle->position = vec3(10);
transform_handle->scale = vec3(0.25);
```

### Using components in systems

Systems look for entities that have a specific combination of components and use those components to perform actions. For example, a circle drawing system might draw every entity that has a transform and circle component, and set the color if the entity also has an optional style component.

```c++
ComponentHandle<Transform>  transform;
ComponentHandle<Circle>     circle;

for (auto e : entities.entities_with_components(transform, circle)) {
  auto style = e.component<Style>();
  if (style) {
    setColor(style->color);
  }
  drawCircle(transform->position, circle->radius);
}
```

Inside the loop above, both the transform and circle handles are guaranteed to be valid. This guarantee is provided by the `entities_with_components` function, which skips any entity that doesn’t have all the components we specify in the function call. When we want to access a different component of the entity the requested components are associated with, we need to check whether that new component is valid before using it.

### Adding custom behavior to a single entity

Sometimes, you may want to give an entity a specific behavior that isn’t clearly modeled by any existing component or combination of components. Other times, you may want to provide an entity with a function that manipulates a handful of components at once (say, flipping out some content in a slideshow with a fancy animation).

We define the `BehaviorComponent` as a place to store these kinds of one-off behaviors for an Entity. By extending the `BehaviorBase` class, you can build your own interfaces to special behaviors and run custom functions on update and other events. The behavior will be registered with the entity, so it will be cleaned up when the entity is destroyed. If you store your own reference to a behavior, you will need to be careful not to use it once its entity has been destroyed.

Behaviors are assigned to an entity through a free function that handles wiring up the behavior’s lifetime. Update-only behaviors can be specified as a lambda.

```c++
auto e = _entities.create();
auto behavior = assignBehavior<BehaviorType>(e);
assignBehavior(e, [] (Entity entity, double dt) {
  …
});
```

Before you start making everything a behavior, consider whether the behavior could be better modeled using a component and system (or by adding a new system that manipulates existing components). You can also evaluate whether a behavior makes more sense as a component+system once you have implemented it as a behavior.

### Grouping entities together

In addition to describing individual entity attributes, we can use components to describe relationships between entities. That means we can build scene graphs using components when we need them.

There are many things to consider when building up a hierarchy component. At its most basic, it should enable traversal of the hierarchy, providing ordered access to each entity along the way. In C++, we also care about object lifetime management, so we make the lifetime of branches dependent on their root. In this repository, we define a Hierarchy component template that provides the following:

1. Access to parent, self, and child entities.
2. Lifetime management. Since there is no garbage collection in standard c++, we make sure the leaves are cleaned up with the root of the hierarchy.
3. A derived template type defining the properties that it makes sense to keep in a hierarchical tree (position, transparency).

Using a hierarchy component (and systems that care to traverse the hierarchy), grouping items like the following becomes straightforward:

```
Menu
  - MenuItemA
  - MenuItemB
    - PromoAnimation
    - ParticleEmitter
  - MenuItemC
```

First, we need to create the entities that will be in the group. Imagine that we have functions that create their respective entities and return the created entity. Creating the individual objects in the group would look like the following:

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

### Things to watch out for

When working with entities and components, your debugger doesn’t always capture the information you want. Since entities are just an id in a table, LLDB has a hard time converting them into their component values when debugging.

You should be able to dereference component handles in the GDB or LLDB console (e.g. `expr component_handle.get()`), but it doesn't always work. If you need the debug info, dereference the handle in the body of the function so the debugger unpacks the info for you.

```c++
auto handle = entity.component<C>();
auto &c = *handle.get(); // c will show up nicely in the debugger.
```

Building this project
---------------------

### Cinder

If you don't already have Cinder installed, clone and build Cinder on your machine. Note that we clone recursively in order to initialize submodules.

```
git clone https://github.com/cinder/Cinder.git --recursive
cd Cinder/xcode
./fullbuild.sh
```

### Samples

Clone this repository into the `blocks/` directory of a recent version of the Cinder master branch. Note that we clone recursively in order to initialize submodules.

```
cd Cinder/blocks/
git clone https://github.com/sosolimited/Entity-Component-Samples.git --recursive
```

Open up one of the samples (in the `samples/` directory) in XCode and you should be good to go.

- Component Swapping
    - Demonstrates adding and removing components from an Entity to change its behavior at runtime.
    - Use number keys 1-3 to add and remove different components.
- Entity Creation
    - Demonstrates the basics of Entity creation, Component definition, and control through Systems.
    - Click and drag to create new entities.
- Star Clusters
    - Demonstrates creation of a scene graph and various approaches to traversing the graph for rendering.
    - Satellites in orbit around central star.
- Gravity Wells
    - Objects fly through the world and are pulled toward attractors.
- TemplateProject
    - Demonstrates the Tinderbox template’s functionality.
    - Visualizes the status of a number of Expires components.

### Project template

This repository includes a cinderblock project template. If you create a new project from the template using TinderBox, you will have a simple working ECS application.
