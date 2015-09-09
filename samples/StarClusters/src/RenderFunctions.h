//
//  RenderFunctions.h
//
//  Created by Soso Limited on 7/13/15.
//
//

#pragma once

///
/// @file A collection of example render functions.
/// Each function renders entities as circles, and provides different levels of rendering control.
///

namespace entityx {
	class EntityManager;
}

namespace soso {

///
/// Draws a circle at each entity's world location.
///
void renderAllEntitiesAsCircles(entityx::EntityManager &entities);

///
/// Renders all entities with Circle components as circles.
///
/// When render order isn't important (as is the general case with opaque geometry),
/// we can just go through and draw everything directly.
/// OpenGL's depth buffer and depth testing are used to handle occlusion.
///
void renderCircles(entityx::EntityManager &entities);

///
/// Render all entities with Circle components in back-to-front order.
///
/// For rendering 2d sprites, or anything with transparency, we can't rely on the depth buffer.
/// Instead, we need to depth-sort our geometry so that the frontmost transparent thing is drawn last.
///
void renderCirclesDepthSorted(entityx::EntityManager &entities);

///
/// Traverses each scene graph root and draws its children in order.
///
/// Often, you want to implicitly draw children in front of their parents,
/// and siblings in the order in which they were added to their parent.
/// This lets us think of things like a painter, where the last painted thing is on top.
///
/// When the entities we want to draw are in a hierarchical graph, we can traverse that graph and render as we go.
///
void renderCirclesHierarchically(entityx::EntityManager &entities);

///
/// Traverses each scene graph root and draws its children in order.
/// Additionally, groups draw calls by layer, so higher layers are drawn later regardless of parenting.
/// Children inherit layering from their parents.
///
/// Sometimes scene graphs don't fully or accurately model how you might want to draw something.
/// Two entities might have background and foreground representations, and both backgrounds should be behind both foregrounds.
/// Other times you might want to render a child behind its parent. Like a background that moves with a shape.
/// Or you may want something to jump to the foreground without reparenting things.
/// Layers are a natural way to express render order that doesn't descend from a parent.
/// Here, we preserve hierarchy within each layer, so child order still matters to render order.
///
void renderCirclesByLayer(entityx::EntityManager &entities);

} // namespace soso
