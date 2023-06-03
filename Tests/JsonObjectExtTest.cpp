#include <gtest/gtest.h>
#include <iostream>
#include <string_view>
#include <tao/json/type.hpp>

#include "SevenBit/Conf/Details/JsonObjectExt.hpp"
#include "SevenBit/Conf/Exceptions.hpp"
#include "SevenBit/Conf/Json.hpp"

using namespace sb::cf::json;

class JsonObjectExtTest : public testing::Test
{
  protected:
    static void TearUpTestSuite() {}

    JsonObjectExtTest() {}

    void SetUp() override {}

    void TearDown() override {}

    ~JsonObjectExtTest() {}

    static void TearDownTestSuite() {}
};

TEST_F(JsonObjectExtTest, ShouldFindInner)
{
    sb::cf::JsonObject json = {{"str", "hello"},
                               {"number", 123},
                               {"array", sb::cf::JsonArray{{{"key", "value"}}}},
                               {"inner",
                                {{"str", "hello1"},
                                 {"number", 1231},
                                 {"inner",
                                  {
                                      {"str", "hello2"},
                                      {"number", 1232},
                                  }}}}};

    EXPECT_EQ(*sb::cf::details::JsonObjectExt::findInner(json, "inner:inner:str"), "hello2");
    EXPECT_EQ(*sb::cf::details::JsonObjectExt::findInner(json, "array:0:key"), "value");
    EXPECT_FALSE(sb::cf::details::JsonObjectExt::findInner(json, "array:2:key"));
    EXPECT_FALSE(sb::cf::details::JsonObjectExt::findInner(json, "array:0.123:key"));
    EXPECT_FALSE(sb::cf::details::JsonObjectExt::findInner(json, "array:0 123:key"));
    EXPECT_FALSE(sb::cf::details::JsonObjectExt::findInner(json, "array:0 apok:key"));
    EXPECT_FALSE(sb::cf::details::JsonObjectExt::findInner(json, "array:-1:key"));
    EXPECT_FALSE(sb::cf::details::JsonObjectExt::findInner(json, "array:9notNumber:key"));
    EXPECT_FALSE(sb::cf::details::JsonObjectExt::findInner(json, "inner:inner:nonExisting"));
    EXPECT_FALSE(sb::cf::details::JsonObjectExt::findInner(json, "inner:inner:str:nonExisting"));
    EXPECT_FALSE(sb::cf::details::JsonObjectExt::findInner(json, "nonExisting"));
    EXPECT_FALSE(sb::cf::details::JsonObjectExt::findInner(json, "inner::inner:str"));
}

TEST_F(JsonObjectExtTest, ShouldGetOrCreateInner)
{
    sb::cf::JsonObject json = {{"str", "hello"},
                               {"number", 123},
                               {"inner",
                                {{"str", "hello1"},
                                 {"number", 1231},
                                 {"inner",
                                  {
                                      {"str", "hello2"},
                                      {"number", 1232},
                                  }}}}};

    sb::cf::details::JsonObjectExt::getOrCreateInner(json, "inner:inner:str") = "hello3";
    sb::cf::details::JsonObjectExt::getOrCreateInner(json, "inner:inner:inner:str") = "hello5";

    sb::cf::JsonObject expectedJson = {{"str", "hello"},
                                       {"number", 123},
                                       {"inner",
                                        {{"str", "hello1"},
                                         {"number", 1231},
                                         {"inner",
                                          {{"str", "hello3"},
                                           {"number", 1232},
                                           {"inner",
                                            {
                                                {"str", "hello5"},
                                            }}}}}}};

    EXPECT_EQ(json, expectedJson);
}

TEST_F(JsonObjectExtTest, ShouldGetOrCreateInnerArrayElement)
{
    sb::cf::JsonObject json = {{"str", "hello"}};

    sb::cf::details::JsonObjectExt::getOrCreateInner(json, "array:3:object") = "value";

    sb::cf::JsonObject expected = {
        {"str", "hello"},
        {"array",
         sb::cf::JsonArray{sb::cf::JsonValue{}, sb::cf::JsonValue{}, sb::cf::JsonValue{}, {{"object", "value"}}}},
    };

    EXPECT_EQ(json, expected);
}

TEST_F(JsonObjectExtTest, ShouldGetOrCreateExistingArrayElement)
{
    sb::cf::JsonObject json = {{"str", "hello"}, {"array", sb::cf::JsonArray{1234, {{"second", "element"}}}}};

    sb::cf::details::JsonObjectExt::getOrCreateInner(json, "array:3:object") = "value";

    sb::cf::JsonObject expected = {
        {"str", "hello"},
        {"array", sb::cf::JsonArray{1234, {{"second", "element"}}, sb::cf::JsonValue{}, {{"object", "value"}}}},
    };

    EXPECT_EQ(json, expected);
}

TEST_F(JsonObjectExtTest, ShouldGetOrCreateInnerWrongArrayElement)
{
    sb::cf::JsonObject json = {{"str", "hello"}};

    sb::cf::details::JsonObjectExt::getOrCreateInner(json, "array:-3:object") = "value";

    sb::cf::JsonObject expected = {{"str", "hello"}, {"array", {{"-3", {{"object", "value"}}}}}};

    EXPECT_EQ(json, expected);
}

TEST_F(JsonObjectExtTest, ShouldFailGetOrCreateInner)
{
    sb::cf::JsonObject json = {{"str", "hello"},
                               {"number", 123},
                               {"inner",
                                {{"str", "hello1"},
                                 {"number", 1231},
                                 {"inner",
                                  {
                                      {"str", "hello2"},
                                      {"number", 1232},
                                  }}}}};

    EXPECT_ANY_THROW(sb::cf::details::JsonObjectExt::getOrCreateInner(json, "inner:inner:str:fail"));
}

TEST_F(JsonObjectExtTest, ShouldFailGetOrCreateInnerEmpty)
{
    sb::cf::JsonObject json = {{"str", "hello"}};

    EXPECT_ANY_THROW(sb::cf::details::JsonObjectExt::getOrCreateInner(json, std::vector<std::string_view>{}));
}

TEST_F(JsonObjectExtTest, SouldDeepMergeEmptyJsonValue)
{
    sb::cf::JsonValue json;

    sb::cf::JsonValue jsonOverride = {{"str", "helloOv"}};

    sb::cf::details::JsonObjectExt::deepMerge(json, std::move(jsonOverride));

    sb::cf::JsonValue expected = {{"str", "helloOv"}};

    EXPECT_EQ(json, expected);
}

TEST_F(JsonObjectExtTest, SouldDeepMergeJsonValue)
{

    sb::cf::JsonValue json = {{"str", "hello"},
                              {"number", 123},
                              {"array", sb::cf::JsonArray{1, 2, 3, 4, 5, 6}},
                              {"default", "default"},
                              {"inner",
                               {
                                   {"str", "hello1"},
                                   {"number", 1231},

                               }}};

    sb::cf::JsonValue jsonOverride = {{"str", "helloOv"},
                                      {"default", sb::cf::json::uninitialized},
                                      {"array", sb::cf::JsonArray{3, {{"value", "value"}}, 1}},
                                      {"inner",
                                       {{"number", 12313},
                                        {"inner",
                                         {
                                             {"str", "hello2Ov"},
                                             {"number", 12323},
                                         }}}}};

    sb::cf::details::JsonObjectExt::deepMerge(json, std::move(jsonOverride));

    sb::cf::JsonObject expectedJson = {{"str", "helloOv"},
                                       {"number", 123},
                                       {"array", sb::cf::JsonArray{3, {{"value", "value"}}, 1, 4, 5, 6}},
                                       {"default", "default"},
                                       {"inner",
                                        {{"str", "hello1"},
                                         {"number", 12313},
                                         {"inner",
                                          {
                                              {"str", "hello2Ov"},
                                              {"number", 12323},
                                          }}}}};

    EXPECT_EQ(json, expectedJson);
}

TEST_F(JsonObjectExtTest, SouldDeepMergeJsonArray)
{

    sb::cf::JsonArray json{{{"str", "hello"}},
                           {{"number", 123}},
                           sb::cf::JsonValue{},
                           {{"inner",
                             {
                                 {"str", "hello1"},
                                 {"number", 1231},

                             }}}};

    sb::cf::JsonArray jsonOverride{
        {{"str", "hello22"}},
        sb::cf::JsonValue{},
        {{"number", 123}},
    };

    sb::cf::details::JsonObjectExt::deepMerge(json, std::move(jsonOverride));

    sb::cf::JsonArray expectedJson{{{"str", "hello22"}},
                                   {{"number", 123}},
                                   {{"number", 123}},
                                   {{"inner",
                                     {
                                         {"str", "hello1"},
                                         {"number", 1231},

                                     }}}};

    EXPECT_EQ(json, expectedJson);
}