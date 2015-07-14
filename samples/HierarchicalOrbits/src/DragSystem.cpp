//
//  DragSystem.cpp
//
//  Created by Soso Limited on 7/14/15.
//
//

#include "DragSystem.h"
#include "Draggable.h"
#include "Transform.h"

using namespace soso;
using namespace entityx;
using namespace cinder;
using namespace cinder::app;

void DragSystem::configure(entityx::EventManager &events)
{
	auto window = app::getWindow();
	auto mouse_down = [this] (MouseEvent &event) { mouseDown( event ); };
	auto mouse_drag = [this] (MouseEvent &event) { mouseDrag( event ); };

	_signal_connections.emplace_back( std::make_shared<ci::signals::ScopedConnection>( window->getSignalMouseDown().connect( mouse_down ) ) );
	_signal_connections.emplace_back( std::make_shared<ci::signals::ScopedConnection>( window->getSignalMouseDrag().connect( mouse_drag ) ) );
}

void DragSystem::mouseDown(ci::app::MouseEvent &event)
{
	ComponentHandle<Draggable> drag;
	ComponentHandle<Transform> transform;
	_dragging_entity.invalidate();

	auto p1 = vec3(event.getPos(), 0.0f);
	auto r = _grab_radius;
	for (auto e : _entities.entities_with_components(transform, drag)) {
		auto d = distance(p1, vec3(vec2(transform->worldPoint()), 0.0f));
		if (d < r) {
			_drag_start = p1;
			_entity_start = transform->position;
			_dragging_entity = e;
			r = d;
		}
	}
}

void DragSystem::mouseDrag(ci::app::MouseEvent &event)
{
	if (_dragging_entity) {
		ComponentHandle<Draggable> drag;
		ComponentHandle<Transform> xf;
		_dragging_entity.unpack(drag, xf);

		xf->position = _entity_start + (vec3(event.getPos(), 0.0f) - _drag_start) * vec3(drag->axes, 1.0f);
	}
}
