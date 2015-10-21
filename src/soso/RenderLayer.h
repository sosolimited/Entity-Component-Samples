//
//  RenderLayer.h
//
//  Created by Soso Limited on 7/13/15.
//
//

#pragma once

#include "entityx/Entity.h"

namespace soso {

struct RenderLayer
{
public:
  /// Create a relative render layer component.
  RenderLayer() = default;
  /// Creates an absolute render layer component.
  explicit RenderLayer(int layer)
  : _layer(layer),
    _relative(false)
  {}

  void setRelativeLayer(int layer) { _layer = layer; _relative = true; }
  void setAbsoluteLayer(int layer) { _layer = layer; _relative = false; }

  int layer() const { return _layer; }

  bool relative() const { return _relative; }
  bool absolute() const { return !_relative; }
private:

  int    _layer = 0;
  bool  _relative = true;
};

} // namespace soso
