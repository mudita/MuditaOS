#pragma once

#include <memory>
#include <module-utils/Utils.hpp>
#include <utility>

namespace bsp::cellular
{
    enum class CellularResultCode : std::uint8_t
    {
        Uninitialized,
        ReceivedAndIdle,
        ReceivedAndFull,
        ReceivedAfterFull,
        ReceivingNotStarted,
        ReceivedNoData,
        TransmittingNotStarted,
        CMUXFrameError,
    };

    constexpr size_t CellularResultStructMaxDataSize = 128;
    constexpr size_t CellularResultStructEmptySize   = 5;

    struct __attribute__((__packed__)) CellularDMAResultStruct
    {
        CellularResultCode resultCode = CellularResultCode::ReceivedNoData;
        size_t dataSize               = 0;
        std::uint8_t data[CellularResultStructMaxDataSize];

        constexpr static size_t getEmptySize()
        {
            return sizeof(CellularDMAResultStruct) - CellularResultStructMaxDataSize;
        }

        constexpr static size_t getMaxSize()
        {
            return getEmptySize() + sizeof(data);
        }

        size_t getSize() const noexcept
        {
            return getEmptySize() + dataSize;
        }
    };

    struct CellularResultStruct
    {
        CellularResultCode resultCode = CellularResultCode::Uninitialized;
        std::vector<uint8_t> data;

        [[nodiscard]] auto serialize() const -> std::unique_ptr<uint8_t[]>
        {
            auto serialized     = std::make_unique< uint8_t[]>(data.size() + sizeof(resultCode));
            serialized.get()[0] = static_cast<unsigned char>(resultCode);

            if (data.size() > 0) {
                memcpy(serialized.get() + 1, data.data(), data.size());
            }

            return serialized;
        }

        auto deserialize(const uint8_t *serialized, const size_t size) -> void
        {
            if (size > 0) {
                resultCode = static_cast<CellularResultCode>(*serialized);
                if (size > 1) {
                    data = {serialized + 1, serialized + size};
                }
            }
        }

        [[nodiscard]] auto getSerializedSize() const noexcept -> size_t
        {
            return (sizeof(resultCode) + data.size());
        }
    };

    class CellularResult
    {
        CellularResultStruct result;

      public:
        CellularResult() : result{CellularResultCode::Uninitialized, {}}
        {}

        explicit CellularResult(CellularDMAResultStruct &result)
            : result{result.resultCode, {result.data, result.data + result.dataSize}}
        {}

        explicit CellularResult(CellularResultStruct result) : result{std::move(result)}
        {}

        explicit CellularResult(const uint8_t *serialized, const size_t size)
        {
            result.deserialize(serialized, size);
        }

        auto getResultCode() const noexcept -> CellularResultCode
        {
            return result.resultCode;
        }

        auto setResultCode(CellularResultCode code) -> void
        {
            result.resultCode = code;
        }

        auto getData() const noexcept -> std::vector<uint8_t> const &
        {
            return result.data;
        }

        auto setData(const std::vector<uint8_t> &data) -> void
        {
            result.data = data;
        }

        auto getDataAsString() const -> std::string
        {
            return std::string{result.data.begin(), result.data.end()};
        }

        auto getStruct() const -> CellularResultStruct const *
        {
            return &result;
        }

        auto getSerialized() const -> std::unique_ptr<uint8_t[]>
        {
            return result.serialize();
        }

        auto getSerializedSize() const noexcept -> size_t
        {
            return result.getSerializedSize();
        }
    };
} // namespace bsp::cellular

inline const char *c_str(bsp::cellular::CellularResultCode result)
{
    switch (result) {
    case bsp::cellular::CellularResultCode::ReceivedAndIdle:
        return "ReceivedAndIdle";
    case bsp::cellular::CellularResultCode::ReceivedAndFull:
        return "ReceivedAndFull";
    case bsp::cellular::CellularResultCode::ReceivedAfterFull:
        return "ReceivedAfterFull";
    case bsp::cellular::CellularResultCode::ReceivingNotStarted:
        return "ReceivingNotStarted";
    case bsp::cellular::CellularResultCode::TransmittingNotStarted:
        return "TransmittingNotStarted";
    default:
        return "Unknown";
    }
}
