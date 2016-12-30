#pragma once
#include "InputData.h"

namespace JSON{
struct Task : public virtual ::JSON::InputData {
  virtual ~Task() = default;
  virtual int run() = 0;
};
}
