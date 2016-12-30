#pragma once
#include "Task.h"

namespace JSON{
class ConsoleApplication {
 public:
  ConsoleApplication();
  virtual ~ConsoleApplication() = default;
  template <typename T>
  void add_task(std::string name) {
    tasks.emplace(std::make_pair(std::move(name), std::make_unique<T>()));
  }
  virtual void setup(int argc, char** argv);
  int exec();

 private:
  int setup_rcode = -1;
  std::istream* is;
  std::map<std::string, std::unique_ptr<Task>> tasks;
  Task* task = nullptr;
};
}