#include <iostream>
#include "json_input_app/ConsoleApplication.h"

using namespace JSON;

struct SimpleTask : public Task {
  int n;

  SimpleTask(){
    bind(n,"n","int"_schema);
  }

  virtual int run() override {
    for (int i=0;i<n;i++){
      std::cout << "hello world" << i << std::endl;
    }
    return 0;
  }
};

int main(int argc, char** argv){
  ConsoleApplication app;
  app.add_task<SimpleTask>("combo");    // <-- put more tasks here
  app.setup(argc,argv);
  return app.exec();
};
