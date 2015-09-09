//
//  Draggable.h
//
//  Created by Soso Limited on 7/14/15.
//
//

#pragma once

namespace soso {

///
/// A Draggable component indicates the entity is draggable.
/// Controlled by the DragSystem.
///
struct Draggable
{
	Draggable() = default;
	explicit Draggable(const ci::vec2 &axes)
	: _axes(axes)
	{}

	ci::vec2 _axes = ci::vec2(1.0f);
};

} // namespace soso
