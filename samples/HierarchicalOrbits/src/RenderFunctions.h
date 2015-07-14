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
/// When render order isn't important (as is the general case with opaque geometry),
/// we can just go through and draw everything directly.
/// We can use OpenGL's depth buffer to handle geometry occlusion, since nothing has transparency.
///
/// Note that we could easily batch these draw circle calls without changing the underlying entities.
///
void renderAllEntitiesAsCircles(entityx::EntityManager &entities);

///
void renderCircles(entityx::EntityManager &entities);

/// For rendering 2d sprites, or anything with transparency, we can't rely on the depth buffer.
/// Instead, we need to depth-sort our geometry so that the frontmost transparent thing is drawn last.
void renderCirclesDepthSorted(entityx::EntityManager &entities);

///
/// Often, you want to implicitly draw children in front, and children in order.
/// Like buttons with a background and a text label.
/// Scene graphs provide a natural way to model these issues and make sure drawn shapes stay together.
///
void renderCirclesWithGraph(entityx::EntityManager &entities);

///
/// Sometimes scene graphs don't accurately model how you might want to draw something.
/// Two entities might have background and foreground representations, and both backgrounds should be behind both foregrounds.
/// Other times you might want to render a child behind its parent. Like a background that moves with a shape.
/// Or you may want something to jump to the foreground without reparenting things.
/// Layers are a natural way to express render order that doesn't descend from a parent.
/// Here, we preserve hierarchy within each layer, so child order still matters to render order.
///
void renderEntitiesWithLayers(entityx::EntityManager &entities);

} // namespace soso
