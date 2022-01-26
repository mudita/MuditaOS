// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <catch2/catch.hpp>
#include <bsp/cellular/CellularResult.hpp>

TEST_CASE("CellularResult")
{
    SECTION("CellularDMAResultStruct")
    {
        bsp::cellular::CellularDMAResultStruct cellularDmaResultStruct;

        auto maxSize = bsp::cellular::CellularResultStructMaxDataSize + sizeof(cellularDmaResultStruct.resultCode) +
                       sizeof(cellularDmaResultStruct.dataSize);

        REQUIRE(cellularDmaResultStruct.getMaxSize() == maxSize);
        REQUIRE(cellularDmaResultStruct.resultCode == bsp::cellular::CellularResultCode::ReceivedNoData);
        REQUIRE(cellularDmaResultStruct.getEmptySize() == maxSize - bsp::cellular::CellularResultStructMaxDataSize);
        REQUIRE(cellularDmaResultStruct.getSize() == cellularDmaResultStruct.getEmptySize());
    }

    SECTION("Create CellularResultStruct")
    {
        bsp::cellular::CellularResultStruct cellularResultStruct;

        REQUIRE(cellularResultStruct.resultCode == bsp::cellular::CellularResultCode::Uninitialized);
        REQUIRE(cellularResultStruct.data.empty());
    }

    SECTION("Serialize empty CellularResultStruct")
    {
        bsp::cellular::CellularResultStruct cellularResultStruct;

        auto serialized       = cellularResultStruct.serialize();
        auto serializedResult = serialized.get()[0];

        delete[] serialized.release();

        REQUIRE(cellularResultStruct.getSerializedSize() == sizeof(cellularResultStruct.resultCode));
        REQUIRE(serializedResult == static_cast<unsigned char>(bsp::cellular::CellularResultCode::Uninitialized));
    }

    SECTION("Deserialize empty buffer")
    {
        bsp::cellular::CellularResultStruct cellularResultStruct;

        uint8_t serialized[0] = {};
        cellularResultStruct.deserialize(serialized, 0);

        REQUIRE(cellularResultStruct.resultCode == bsp::cellular::CellularResultCode::Uninitialized);
        REQUIRE(cellularResultStruct.data.empty());
    }

    SECTION("Serialize and deserialize CellularResultStruct")
    {
        bsp::cellular::CellularResultStruct cellularResultStruct;
        cellularResultStruct.resultCode = bsp::cellular::CellularResultCode::ReceivedAndFull;
        cellularResultStruct.data       = {1, 2, 3, 4, 5, 10};

        auto serialized = cellularResultStruct.serialize();

        bsp::cellular::CellularResultStruct cellularResultStructDeserialized;
        cellularResultStructDeserialized.deserialize(serialized.get(), cellularResultStruct.getSerializedSize());

        delete[] serialized.release();

        REQUIRE(cellularResultStructDeserialized.resultCode == cellularResultStruct.resultCode);
        REQUIRE(cellularResultStructDeserialized.data == cellularResultStruct.data);
    }
}
