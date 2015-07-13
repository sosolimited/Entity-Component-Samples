//
//  RenderFunctions.cpp
//
//  Created by Soso Limited on 7/13/15.
//
//

#include "RenderFunctions.h"

#include "Transform.h"
#include "RenderLayer.h"
#include "entityx/Entity.h"
#include "cinder/gl/gl.h"

using namespace soso;
using namespace cinder;

///
/// When render order isn't important (as is the general case with opaque geometry),
/// we can just go through and draw everything directly.
/// Note that we could easily batch these draw circle calls without changing the underlying entities.
///
void soso::renderAllEntitiesAsCircles(entityx::EntityManager &entities)
{
	entityx::ComponentHandle<Transform> transform;
	for (auto __unused e : entities.entities_with_components(transform)) {
		gl::ScopedModelMatrix mat;
		gl::multModelMatrix(transform->worldTransform());
		gl::drawSolidCircle(vec2(0), 12.0f);

		app::console() << "Position: " << transform->position << ", transform: " << (transform->worldTransform() * vec4(0, 0, 0, 1)) << std::endl;
	}
}

///
/// Often, you want to implicitly draw children in front, and children in order.
/// Like buttons with a background and a text label.
/// Scene graphs provide a natural way to model these issues and make sure drawn shapes stay together.
///
void soso::renderEntitiesWithGraph(entityx::EntityManager &entities)
{
	std::vector<ci::mat4> render_data;

	entityx::ComponentHandle<Transform> transform;
	for (auto __unused e : entities.entities_with_components(transform)) {
		// gather trees for rendering
    if (transform->isRoot()) {
			render_data.push_back(transform->worldTransform());
			transform->descend([&render_data] (const Transform &parent, const Transform &child) {
				render_data.push_back(child.worldTransform());
			});
		}
	}

	gl::ScopedModelMatrix mat;
	for (auto &m : render_data) {
		gl::setModelMatrix(m);
		gl::drawSolidCircle(vec2(0), 12.0f);
	}
}

///
/// Sometimes scene graphs don't accurately model how you might want to draw something.
/// Two entities might have background and foreground representations, and both backgrounds should be behind both foregrounds.
/// Other times you might want to render a child behind its parent. Like a background that moves with a shape.
/// Or you may want something to jump to the foreground without reparenting things.
/// Layers are a natural way to express render order that doesn't descend from a parent.
/// Here, we preserve hierarchy within each layer, so child order still matters to render order.
///
void soso::renderEntitiesWithLayers(entityx::EntityManager &entities)
{
	using RenderData = std::vector<ci::mat4>;
	using RenderDataRef = std::unique_ptr<RenderData>;
	struct RenderDataLayer {
		RenderDataLayer(int layer)
		: layer(layer)
		{}
		int						layer = 0;
		RenderDataRef render_data;
	};

	std::vector<RenderDataLayer> layers;
	auto get_layer = [&layers] (int layer) -> RenderData& {
		for (auto &l : layers) {
			if (l.layer == layer) {
				return *l.render_data;
			}
		}
		layers.emplace_back(RenderDataLayer(layer));
		return *layers.back().render_data;
	};

	// Recursive function to properly capture render layer changes.
	std::function<void (Transform::Handle, int)> gather_recursively = [&gather_recursively, &get_layer] (Transform::Handle transform, int layer) {

		entityx::ComponentHandle<soso::RenderLayer> rlc;
		auto e = transform->entity();
		e.unpack(rlc);

		if (rlc) {
			layer += rlc->layer_adjustment;
		}

		get_layer(layer).push_back(transform->worldTransform());

		for (auto &c : transform->children()) {
			gather_recursively(c, layer);
		}
	};

	entityx::ComponentHandle<Transform> transform;
	for (auto __unused e : entities.entities_with_components(transform)) {
		// gather trees for rendering
    if (transform->isRoot()) {
			gather_recursively(transform, 0);
		}
	}

	std::sort(layers.begin(), layers.end(), [] (const RenderDataLayer &lhs, const RenderDataLayer &rhs) {
		return lhs.layer < rhs.layer;
	});

	gl::ScopedModelMatrix mat;
	for (auto &layer : layers) {
		for (auto &m : *layer.render_data) {
			gl::setModelMatrix(m);
			gl::drawSolidCircle(vec2(0), 12.0f);
		}
	}
}
