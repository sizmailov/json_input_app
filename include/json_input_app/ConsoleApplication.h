#pragma once

#include "Task.h"
#include "Logger.h"

namespace JSON{
class ConsoleApplication {
 public:

  ConsoleApplication(int argc, char** const argv);
  virtual ~ConsoleApplication() = default;

  template <typename T>
  void add_task(std::string name) {
    tasks.emplace(std::make_pair(std::move(name), std::make_unique<T>()));
  }

  virtual int exec();

protected:

  enum class SetupStatus{
    GoodOptions,
    BadOptions,
    NoExecOptions,
  };

  virtual SetupStatus setup();
  const int argc;
  char** const argv;
  std::istream* is;
private:
  std::map<std::string, std::unique_ptr<Task>> tasks;
  Task* task = nullptr;
};
}