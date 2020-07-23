#include "UpdatePureOS.hpp"
#include "FactoryReset.hpp"

#include <vfs.hpp>
#include <catch2/catch.hpp>
#include <iostream>

class vfs vfs;

TEST_CASE("System Update Tests")
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

TEST_CASE("Factory Reset Test")
{
    vfs.Init();

    std::string sysdir = purefs::dir::eMMC_disk;
    sysdir += "/factory-test/sys";
    std::string factorydir = sysdir + "/factory";
    REQUIRE(FactoryReset::DeleteDirContent(sysdir) == true);
    REQUIRE(FactoryReset::CopyDirContent(factorydir, sysdir) == true);
}
