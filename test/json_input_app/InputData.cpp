#include <gtest/gtest.h>

#include "json_input_app/InputData.h"


using ::testing::Test;
using namespace JSON;


class InputDataTests : public Test{

};

TEST_F(InputDataTests, deserialization){
  struct Input : public InputData{
    int n1;
    int n2;
    Input(){
      bind(n1,"n1","int"_schema);
      bind(n2,"n2","int"_schema);
    };
  };

  Input input;
  input.n1=0;
  input.n2=0;

  EXPECT_NO_THROW(JSON::deserialize("{\"n1\":1,\"n2\":2}"_json,input));
  EXPECT_EQ(input.n1,1);
  EXPECT_EQ(input.n2,2);
  EXPECT_THROW(JSON::deserialize("{\"n1\":null,\"n2\":2}"_json,input), SchemaMatchResult::MatchError);
  EXPECT_THROW(JSON::deserialize("{\"n1\":1,\"n2\":null}"_json,input), SchemaMatchResult::MatchError);

  Input input2;
  input2.n1 = 0;
  input2.n2 = 0;

  JSON::deserialize(JSON::serialize(input),input2);
  EXPECT_EQ(input.n1,input2.n1);
  EXPECT_EQ(input.n2,input2.n2);
}


TEST_F(InputDataTests, deserialization_2){
  struct Input0 : virtual public InputData{
    int n1;
    Input0(){
      bind(n1,"n1","int"_schema);
    };
  };
  struct Input1 : virtual public InputData{
    int n2;
    Input1(){
      bind(n2,"n2","int"_schema);
    };
  };
  struct Input : public Input1, public Input0{
  };

  Input input;
  input.n1=0;
  input.n2=0;

  EXPECT_NO_THROW(JSON::deserialize("{\"n1\":1,\"n2\":2}"_json,input));
  EXPECT_EQ(input.n1,1);
  EXPECT_EQ(input.n2,2);
  EXPECT_THROW(JSON::deserialize("{\"n1\":null,\"n2\":2}"_json,input), SchemaMatchResult::MatchError);
  EXPECT_THROW(JSON::deserialize("{\"n1\":1,\"n2\":null}"_json,input), SchemaMatchResult::MatchError);

  Input input2;
  input2.n1 = 0;
  input2.n2 = 0;

  JSON::deserialize(JSON::serialize(input),input2);
  EXPECT_EQ(input.n1,input2.n1);
  EXPECT_EQ(input.n2,input2.n2);
}


TEST_F(InputDataTests, deserialization_3){
  struct Input : virtual public InputData{
    int n1;
    int n2;
    Input(){
      bind(n1,"n1","int"_schema);
      bind(n2,"n2","int"_schema);
    };
  };
  struct Aggregate : virtual public InputData{
    Input input;
    Aggregate(){
      bind(input,"input");
    }
  };

  Aggregate a;
  a.input.n1=0;
  a.input.n2=0;

  EXPECT_NO_THROW(JSON::deserialize("{\"input\":{\"n1\":1,\"n2\":2}}"_json,a));
  EXPECT_EQ(a.input.n1,1);
  EXPECT_EQ(a.input.n2,2);
  EXPECT_THROW(JSON::deserialize("{\"input\":{\"n1\":null,\"n2\":2}}"_json,a), SchemaMatchResult::MatchError);
  EXPECT_THROW(JSON::deserialize("{\"input\":{\"n1\":1,\"n2\":null}}"_json,a), SchemaMatchResult::MatchError);

  Aggregate a2;
  a2.input.n1 = 0;
  a2.input.n2 = 0;

  JSON::deserialize(JSON::serialize(a),a2);
  EXPECT_EQ(a.input.n1,a2.input.n1);
  EXPECT_EQ(a.input.n2,a2.input.n2);
}
