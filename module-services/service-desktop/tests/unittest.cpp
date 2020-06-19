#include "catch.hpp"
#include "vfs.hpp"
#include <UpdatePureOS.h>
#include <stdint.h>
#include <algorithm>
#include <iostream>

class vfs vfs;

TEST_CASE("Test case 1")
{
    vfs.Init();
    UpdatePureOS updateOS(nullptr);

    updateos::UpdateError err = updateOS.prepareTempDirForUpdate();
    REQUIRE(err == updateos::UpdateError::NoError);

    updateOS.setUpdateFile("pureos-unittest.tar");

    err = updateOS.unpackUpdate();
    REQUIRE(err == updateos::UpdateError::NoError);

    err = updateOS.verifyChecksums();
    REQUIRE(err == updateos::UpdateError::NoError);

    err = updateOS.prepareRoot();
    REQUIRE(err == updateos::UpdateError::NoError);
}