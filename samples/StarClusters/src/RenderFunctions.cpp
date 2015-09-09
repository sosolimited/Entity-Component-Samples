//
//  RenderFunctions.cpp
//
//  Created by Soso Limited on 7/13/15.
//
//

#include "RenderFunctions.h"

#include "Transform.h"
#include "Circle.h"
#include "RenderLayer.h"
#include "entityx/Entity.h"
#include "cinder/gl/gl.h"

using namespace soso;
using namespace cinder;

void soso::renderAllEntitiesAsCircles(entityx::EntityManager &entities)
{
	gl::ScopedDepth depth(true, true);

	entityx::ComponentHandle<Transform> transform;
	for (auto __unused e : entities.entities_with_components(transform)) {
		gl::ScopedModelMatrix mat;
		gl::multModelMatrix(transform->worldTransform());
		gl::drawSolidCircle(vec2(0), 12.0f);
	}
}

void soso::renderCircles(entityx::EntityManager &entities)
{
	gl::ScopedDepth depth(true, true);

	entityx::ComponentHandle<Transform> transform;
	entityx::ComponentHandle<Circle>		circle;

	gl::ScopedColor color(Color(1.0f, 1.0f, 1.0f));
	for (auto __unused e : entities.entities_with_components(transform, circle)) {
		gl::ScopedModelMatrix mat;
		auto q = inverse(normalize(quat_cast(transform->worldTransform()))); // billboard the shapes; mostly works
		gl::multModelMatrix(transform->worldTransform() * glm::mat4_cast(q));
		gl::color(circle->color);

		gl::drawSolidCircle(vec2(0), circle->radius);
	}
}

void soso::renderCirclesDepthSorted(entityx::EntityManager &entities)
{
	struct RenderInfo {
		ci::vec3	position;
		float			radius = 1.0f;
		ci::Color color;

	};
	std::vector<RenderInfo> circles;

	auto insertion_point = [&circles] (float depth) {
			auto iter = circles.begin();
			while (iter != circles.end() && depth < iter->position.z) {
				++iter;
			}
			return iter;
		};

	entityx::ComponentHandle<Transform> transform;
	entityx::ComponentHandle<Circle>		circle;

	for (auto __unused e : entities.entities_with_components(transform, circle)) {
		auto pos = vec3(transform->worldTransform() * vec4(0, 0, 0, 1));
		circles.insert(insertion_point(pos.z), RenderInfo{pos, circle->radius, circle->color});
	}

	gl::ScopedColor color(Color(1.0f, 1.0f, 1.0f));
	for (auto &c : circles) {
		gl::ScopedModelMatrix mat;
		gl::translate(c.position);
		gl::color(c.color);

		gl::drawSolidCircle(vec2(0), c.radius);
	}

}

void soso::renderCirclesHierarchically(entityx::EntityManager &entities)
{
	entityx::ComponentHandle<Transform> transform;
	entityx::ComponentHandle<Circle>		circle;
	auto xf = mat4(1);

	const auto billboard_xf = [] (const Transform &transform) {
		auto q = inverse(normalize(quat_cast(transform.localTransform())));
		return transform.localTransform() * glm::mat4_cast(q);
	};

	using function = std::function<void (Transform::Handle)>;
	function draw_recursively = [&billboard_xf, &draw_recursively] (Transform::Handle transform) {
			gl::ScopedModelMatrix mat;
			gl::multModelMatrix(billboard_xf(*transform.get()));

			auto circle = transform->entity().component<Circle>();
			if (circle)
			{
				gl::color(circle->color);
				gl::drawSolidCircle(vec2(0), circle->radius);
			}

			for (auto &child: transform->children())
			{
				draw_recursively(child);
			}
	};

	gl::ScopedColor				color(Color::white());
	gl::ScopedDepth				depth(false);
	for (auto __unused e : entities.entities_with_components(transform, circle)) {
    if (transform->isRoot())
		{
			draw_recursively(transform);
		}
	}
}

void soso::renderCirclesByLayer(entityx::EntityManager &entities)
{
	struct DataPoint
	{
		mat4	transform;
		Color	color;
		float	radius;
	};

	using RenderData = std::vector<DataPoint>;
	using RenderDataRef = std::unique_ptr<RenderData>;
	struct RenderDataLayer {
		RenderDataLayer(int layer)
		: layer(layer)
		{}
		int						layer = 0;
		RenderDataRef render_data = std::make_unique<RenderData>();
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
	using function = std::function<void (Transform::Handle, int)>;
	function gather_recursively = [&gather_recursively, &get_layer] (Transform::Handle transform, int layer) {

		auto rlc = entityx::ComponentHandle<soso::RenderLayer>();
		auto circle = entityx::ComponentHandle<Circle>();
		auto e = transform->entity();
		e.unpack(rlc, circle);

		if (rlc)
		{
			layer += rlc->layer_adjustment;
		}

		if (circle)
		{
			get_layer(layer).push_back({ transform->worldTransform(), circle->color, circle->radius });
		}

		for (auto &c : transform->children())
		{
			gather_recursively(c, layer);
		}
	};

	entityx::ComponentHandle<Transform> transform;
	for (auto __unused e : entities.entities_with_components(transform))
	{
		// gather trees for rendering
    if (transform->isRoot())
		{
			gather_recursively(transform, 0);
		}
	}

	// In case we encountered layers out of order
	std::sort(layers.begin(), layers.end(), [] (const RenderDataLayer &lhs, const RenderDataLayer &rhs) {
		return lhs.layer < rhs.layer;
	});

	gl::ScopedModelMatrix mat;
	gl::ScopedColor color(Color::white());

	for (auto &layer : layers)
	{
		for (auto &data : *layer.render_data)
		{
			gl::setModelMatrix(data.transform);
			gl::color(data.color);
			gl::drawSolidCircle(vec2(0), data.radius);
		}
	}
}
