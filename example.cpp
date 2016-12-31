#include <iostream>
#include "json_input_app/ConsoleApplication.h"
#include <cmath>

using namespace JSON;

struct SimpleTask : public Task {
  int n;
  std::vector<double> a;

  SimpleTask(){
    bind(n,"n","int(0..)"_schema);
    bind(a,"a","[double]"_schema);
    schema.prepend_docstring("calculates a^n");
  }

  virtual int run() override {
    LOG_INFO("In run function");
    for (auto&x: a){
      LOG_DEBUG("Cycle iteration");
      std::cout <<x<<"^"<<n<<" = "<< std::pow(x,n) << std::endl;

    }
    return 0;
  }
};

int main(int argc, char** argv){
  ConsoleApplication app(argc,argv);
  app.add_task<SimpleTask>("power");    // <-- put more tasks here
  return app.exec();
};
