#include "catch.hpp"
#include <stdint.h>
#include <algorithm>

#include <bsp/bluetooth/Bluetooth.hpp>
#include <cstdio>
#include <cstring>
#include <array>

using namespace bsp;
TEST_CASE("TC1") {
    SECTION("Generic")
    {
        auto bt = Bluetopia::getInstance();
        REQUIRE(bt!=nullptr);

        bt->log(BTdev::LogError,"Test! Shouldnt print");
        bt->register_log(vprintf);
        bt->log(BTdev::LogError,"Test prints\n");
        bt->log(BTdev::LogDebug, "Now logs everything\n");
        bt->log(BTdev::LogWarning, "LOG: %u\n",10);

        // cast to char* to aviod warning
        REQUIRE(bt->write_blocking((char*)"LOL", strlen("LOL"))>0);
        std::array<char,120> test_array;
        test_array.fill('u');
        size_t ret = bt->write_blocking(test_array.data(), test_array.size());
        printf("ret: %lu\n",ret);
        REQUIRE(ret != 0);
        REQUIRE(ret == test_array.size());

        // put 1 element
        REQUIRE(bt->in.push('a') == 0);
        // pop 1 element
        REQUIRE(bt->in.pop(test_array.data()) == 0);
        // test popped element
        REQUIRE(test_array[0] == 'a');
        test_array[0] ='z';
        // pop from empty buffer
        REQUIRE(bt->in.pop(test_array.data()) != 0);
        // check pointer passed for valid value (not changed)
        REQUIRE(test_array[0] == 'z');
        // fill while buffer
        const bool guard_met = false;
        for( int i=0; i < bt->in.size; ++i) {
            REQUIRE(bt->in.push('a') == 0);
            if( !guard_met &&
                i >= bt->in.size - bt->in.threshold) {
                bool guard_met = bt->in.threshold_guard();
            }
        }
        // default threshold == 0
        REQUIRE(guard_met == false);
        bt->in.threshold=1;
        // now shall be met
        REQUIRE(bt->in.threshold_guard() == true);
        // buffer is full -> we shouldn't be able to push anymore!
        REQUIRE(bt->in.push('a') != 0);
    }
}
