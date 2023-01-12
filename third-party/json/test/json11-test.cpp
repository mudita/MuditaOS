#include <catch2/catch.hpp>
#include "json11.hpp"

TEST_CASE("json11 - custom types test")
{
    std::string err{};
    std::string dumpString{};

    SECTION("signed"){
        SECTION("intmax_t - max")
        {
            intmax_t value = INTMAX_MAX;
            auto json = json11::Json::object{
                     {"test", value}};
            REQUIRE(json["test"] == value);

            dumpString = json11::Json(json).dump();
            auto newJson = json11::Json::parse(dumpString,err);
            REQUIRE(err.empty());
            REQUIRE(newJson == json);
            REQUIRE(newJson["test"] == value);
        }
        SECTION("intmax_t - min")
        {
            intmax_t value = INTMAX_MIN;
            auto json = json11::Json::object{
                     {"test", value}};
            REQUIRE(json["test"] == value);

            dumpString = json11::Json(json).dump();
            auto newJson = json11::Json::parse(dumpString,err);
            REQUIRE(err.empty());
            REQUIRE(newJson == json);
            REQUIRE(newJson["test"] == value);
        }
        SECTION("int64_t - max")
        {
            int64_t value = INT64_MAX;
            auto json = json11::Json::object{
                    {"test", value}};
            REQUIRE(json["test"] == value);

            dumpString = json11::Json(json).dump();
            auto newJson = json11::Json::parse(dumpString,err);
            REQUIRE(err.empty());
            REQUIRE(newJson == json);
            REQUIRE(newJson["test"] == value);
        }
        SECTION("int64_t - min")
        {
            int64_t value = INT64_MIN;
            auto json = json11::Json::object{
                    {"test", value}};
            REQUIRE(json["test"] == value);

            dumpString = json11::Json(json).dump();
            auto newJson = json11::Json::parse(dumpString,err);
            REQUIRE(err.empty());
            REQUIRE(newJson == json);
            REQUIRE(newJson["test"] == value);
        }
        SECTION("int32_t - max")
        {
            int32_t value = INT32_MAX;
            auto json = json11::Json::object{
                    {"test", value}};
            REQUIRE(json["test"] == value);

            dumpString = json11::Json(json).dump();
            auto newJson = json11::Json::parse(dumpString,err);
            REQUIRE(err.empty());
            REQUIRE(newJson == json);
            REQUIRE(newJson["test"] == value);
        }
        SECTION("int32_t - min")
        {
            int32_t value = INT32_MIN;
            auto json = json11::Json::object{
                    {"test", value}};
            REQUIRE(json["test"] == value);

            dumpString = json11::Json(json).dump();
            auto newJson = json11::Json::parse(dumpString,err);
            REQUIRE(err.empty());
            REQUIRE(newJson == json);
            REQUIRE(newJson["test"] == value);
        }
        SECTION("int16_t - min")
        {
            int16_t value = INT16_MAX;
            auto json = json11::Json::object{
                    {"test", value}};
            REQUIRE(json["test"] == value);

            dumpString = json11::Json(json).dump();
            auto newJson = json11::Json::parse(dumpString,err);
            REQUIRE(err.empty());
            REQUIRE(newJson == json);
            REQUIRE(newJson["test"] == value);
        }
        SECTION("int16_t - max")
        {
            int16_t value = INT16_MIN;
            auto json = json11::Json::object{
                    {"test", value}};
            REQUIRE(json["test"] == value);

            dumpString = json11::Json(json).dump();
            auto newJson = json11::Json::parse(dumpString,err);
            REQUIRE(err.empty());
            REQUIRE(newJson == json);
            REQUIRE(newJson["test"] == value);
        }
        SECTION("int8_t - min")
        {
            int8_t value = INT8_MAX;
            auto json = json11::Json::object{
                    {"test", value}};
            REQUIRE(json["test"] == value);

            dumpString = json11::Json(json).dump();
            auto newJson = json11::Json::parse(dumpString,err);
            REQUIRE(err.empty());
            REQUIRE(newJson == json);
            REQUIRE(newJson["test"] == value);
        }
        SECTION("int8_t - max")
        {
            int8_t value = INT8_MIN;
            auto json = json11::Json::object{
                    {"test", value}};
            REQUIRE(json["test"] == value);

            dumpString = json11::Json(json).dump();
            auto newJson = json11::Json::parse(dumpString,err);
            REQUIRE(err.empty());
            REQUIRE(newJson == json);
            REQUIRE(newJson["test"] == value);
        }
    }

    SECTION("unsigned"){
        SECTION("uintmax_t")
        {
            uintmax_t value = UINTMAX_MAX;
            auto json = json11::Json::object{
                     {"test", value}};
            REQUIRE(json["test"] == value);

            dumpString = json11::Json(json).dump();
            auto newJson = json11::Json::parse(dumpString,err);
            REQUIRE(err.empty());
            REQUIRE(newJson == json);
            REQUIRE(newJson["test"] == value);
        }
        SECTION("uint64_t")
        {
            uint64_t value = UINT64_MAX;
            auto json = json11::Json::object{
                    {"test", value}};
            REQUIRE(json["test"] == value);

            dumpString = json11::Json(json).dump();
            auto newJson = json11::Json::parse(dumpString,err);
            REQUIRE(err.empty());
            REQUIRE(newJson == json);
            REQUIRE(newJson["test"] == value);
        }
        SECTION("uint32_t")
        {
            uint32_t value = UINT32_MAX;
            auto json = json11::Json::object{
                    {"test", value}};
            REQUIRE(json["test"] == value);

            dumpString = json11::Json(json).dump();
            auto newJson = json11::Json::parse(dumpString,err);
            REQUIRE(err.empty());
            REQUIRE(newJson == json);
            REQUIRE(newJson["test"] == value);
        }
        SECTION("uint16_t")
        {
            uint16_t value = UINT16_MAX;
            auto json = json11::Json::object{
                    {"test", value}};
            REQUIRE(json["test"] == value);

            dumpString = json11::Json(json).dump();
            auto newJson = json11::Json::parse(dumpString,err);
            REQUIRE(err.empty());
            REQUIRE(newJson == json);
            REQUIRE(newJson["test"] == value);
        }
        SECTION("uint8_t")
        {
            uint8_t value = UINT8_MAX;
            auto json = json11::Json::object{
                   {"test", value}};
            REQUIRE(json["test"] == value);

            dumpString = json11::Json(json).dump();
            auto newJson = json11::Json::parse(dumpString,err);
            REQUIRE(err.empty());
            REQUIRE(newJson == json);
            REQUIRE(newJson["test"] == value);
        }
        SECTION("size_t")
        {
            size_t value = SIZE_MAX;
            auto json = json11::Json::object{
                   {"test", value}};
            REQUIRE(json["test"] == value);

            dumpString = json11::Json(json).dump();
            auto newJson = json11::Json::parse(dumpString,err);
            REQUIRE(err.empty());
            REQUIRE(newJson == json);
            REQUIRE(newJson["test"] == value);
        }
    }

}

