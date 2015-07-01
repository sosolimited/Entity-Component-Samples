# Entity-Component-Sample
Sample implementation of an entity component system using EntityX.

Entity Component Systems are excellent for describing worlds of independent elements with varied behaviors. Within an ECS, objects in the world, `entities`, are just an aggregation of attributes, `components`, that happen to be associated with each other.

So, how we we describe an aggregation of aggregations? We can use a component.

There are lots of things to consider when building up a hierarchy component, but at its most basic, it should enable traversal of the hierarchy and access to each entity along the way. We care about a few more things, like having a reason to traverse the hierarchy and object lifetime management. Our Hierarchy component template provides the following:

1) Ordered traversal of the group of entities.
2) Lifetime management. Since there is no garbage collection in standard c++, we make sure the leaves are cleaned up with the root of the hierarchy.
3) A template type defining the properties that it makes sense to keep in a hierarchical tree (position, transparency).

The HierarchyComponent allows us to group things like we would in a scene graph:

Thing
	- Other
	- Other
		- Child
		- Child
	- Other

We need to be a bit more explicit about grouping things this way:


BehaviorComponent

InterfaceComponent
