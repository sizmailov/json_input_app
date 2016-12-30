#include "json_input_app/InputData.h"

using namespace JSON;

void InputData::parse(const Json& json) {
  auto m = schema.match(json);
  if (!m) {
    throw m.get_error();
  }
  for (auto& x : bindings) {
    x(json);
  }
};

void InputData::print_schema() const { schema.pretty_print(std::cerr); }

Json InputData::serialize() const {
  Json json(Json::Object{});
  for (auto& x : serialize_bindings) {
    json.get_object().emplace(x());
  }
  return std::move(json);
};
