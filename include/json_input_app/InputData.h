#pragma once

#include "concise_json_schema/JsonSerializerStd.h"
#include "concise_json_schema/Schema.h"
#include <functional>
#include "Logger.h"

namespace JSON {

struct InputData {
  using prop = Schema::ObjectSchema::Property;

  InputData() : schema(Schema::ObjectSchema{}) {}
  InputData(const InputData&) = delete;
  InputData(InputData&&) = delete;
  InputData& operator=(const InputData&) = delete;
  InputData& operator=(InputData&&) = delete;
  virtual ~InputData() = default;

  virtual void parse(const Json& json);
  virtual void print_schema() const;
  virtual Json serialize() const;

  template <typename T, typename = std::enable_if_t<std::is_base_of<InputData, T>::value>>
  void bind(T& member, std::string name, estd::optional<Json> default_value = {}) {
    schema.get_object().add_property(name, prop(member.schema, default_value, !!default_value));
    bindings.emplace_back([&member, name, default_value](const Json& json) {
      LOG_SCOPE_VERBOSE(name);
      const Json& jvalue = json.count(name) ? json(name) : default_value.value();
      JSON::deserialize(jvalue, member);
    });
    serialize_bindings.emplace_back(
        [&member, name]() { return std::make_pair(name, JSON::serialize(member)); });
  }

  template <typename T>
  void bind(T& member, std::string name, Schema s, estd::optional<Json> default_value = {}) {
    schema.get_object().add_property(name, prop(s, default_value, !!default_value));
    bindings.emplace_back([&member, name, default_value](const Json& json) {
      LOG_SCOPE_VERBOSE(name);
      const Json& jvalue = json.count(name) ? json(name) : default_value.value();
      JSON::deserialize(jvalue, member);
    });
    serialize_bindings.emplace_back(
        [&member, name]() { return std::make_pair(name, JSON::serialize(member)); });
  }

  /* some properties might be constructed indirectly via supplied @param deserializer */
  void bind(std::string name, Schema s, std::function<void(const Json&)>&& deserializer,
            estd::optional<Json> default_value = {}) {
    schema.get_object().add_property(name, prop(s, default_value, !!default_value));
    bindings.emplace_back([ name, deserializer = std::move(deserializer), default_value ](const Json& json) {
      LOG_SCOPE_VERBOSE(name);
      const Json& jvalue = json.count(name) ? json(name) : default_value.value();
      deserializer(jvalue);
    });
    serialize_bindings.emplace_back([name]() { return std::make_pair(name, Json{}); });
  }

  Schema schema;

 private:
  std::vector<std::function<void(const Json&)>> bindings;
  std::vector<std::function<std::pair<std::string, Json>()>> serialize_bindings;
};
}

namespace JSON{
namespace io{

template<typename T>
struct JsonSerializer<T,typename std::enable_if_t<std::is_base_of<InputData,T>::value>> {
  static void deserialize(const Json& json, T& t){
    t.parse(json);
  }
  static Json serialize(const T& t){
    return t.serialize();
  }
};

}
}
