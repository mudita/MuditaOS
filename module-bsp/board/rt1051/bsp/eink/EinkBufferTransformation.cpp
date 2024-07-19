// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "EinkBufferTransformation.hpp"
#include "EinkDimensions.hpp"
#include "EinkBinarizationLuts.hpp"
#include <cstdint>

namespace
{
    /**
     * @brief This LUT is used for conversion of the 4bpp input grayscale pixel to the 1bpp output pixel
     */
    constexpr std::uint8_t einkMaskLut1Bpp[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1};

    /**
     * @brief This LUT is used for conversion of the 4bpp input grayscale pixel to the 2bpp output pixel
     */
    constexpr std::uint8_t einkMaskLut2Bpp[16] = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3};
} // namespace

namespace bsp::eink
{
    __attribute__((optimize("O1"))) std::uint8_t *transformFrameCoordinateSystem1Bpp(const std::uint8_t *dataIn,
                                                                                     std::uint16_t windowWidthPx,
                                                                                     std::uint16_t windowHeightPx,
                                                                                     std::uint8_t *dataOut,
                                                                                     EinkDisplayColorMode invertColors)
    {
        // In 1bpp mode there are 8 pixels in the byte
        constexpr std::uint8_t pixelsInByte = 8;

        auto outArray = dataOut;

        for (std::int32_t inputCol = windowWidthPx - 1; inputCol >= 0; --inputCol) {
            for (std::int32_t inputRow = windowHeightPx - 1; inputRow >= 7; inputRow -= pixelsInByte) {
                // HACK: Did not create the loop for accessing pixels and merging them in the single byte for better
                // performance.
                // Wanted to avoid unneeded loop count increasing and jump operations which for large amount of data
                // take considerable amount of time.
                const std::uint32_t index = inputRow * BOARD_EINK_DISPLAY_RES_X + inputCol;

                // Use the LUT to convert the input pixel from 4bpp to 1bpp
                const auto firstPixel   = einkMaskLut1Bpp[dataIn[index - 0 * BOARD_EINK_DISPLAY_RES_X]];
                const auto secondPixel  = einkMaskLut1Bpp[dataIn[index - 1 * BOARD_EINK_DISPLAY_RES_X]];
                const auto thirdPixel   = einkMaskLut1Bpp[dataIn[index - 2 * BOARD_EINK_DISPLAY_RES_X]];
                const auto fourthPixel  = einkMaskLut1Bpp[dataIn[index - 3 * BOARD_EINK_DISPLAY_RES_X]];
                const auto fifthPixel   = einkMaskLut1Bpp[dataIn[index - 4 * BOARD_EINK_DISPLAY_RES_X]];
                const auto sixthPixel   = einkMaskLut1Bpp[dataIn[index - 5 * BOARD_EINK_DISPLAY_RES_X]];
                const auto seventhPixel = einkMaskLut1Bpp[dataIn[index - 6 * BOARD_EINK_DISPLAY_RES_X]];
                const auto eightPixel   = einkMaskLut1Bpp[dataIn[index - 7 * BOARD_EINK_DISPLAY_RES_X]];

                // Put the pixels in order: Most left positioned pixel at the most significant side of byte
                std::uint8_t pixels = (firstPixel << 7) | (secondPixel << 6) | (thirdPixel << 5) | (fourthPixel << 4) |
                                      (fifthPixel << 3) | (sixthPixel << 2) | (seventhPixel << 1) | (eightPixel << 0);

                if (invertColors == EinkDisplayColorMode::Inverted) {
                    pixels = ~pixels;
                }

                *outArray = pixels;
                ++outArray;
            }
        }

        return dataOut;
    }

    __attribute__((optimize("O1"))) std::uint8_t *transformFrameCoordinateSystem2Bpp(const std::uint8_t *dataIn,
                                                                                     std::uint16_t windowWidthPx,
                                                                                     std::uint16_t windowHeightPx,
                                                                                     std::uint8_t *dataOut,
                                                                                     EinkDisplayColorMode invertColors)
    {
        // In 2bpp mode there are 4 pixels in the byte
        constexpr std::uint8_t pixelsInByte = 4;

        auto outArray = reinterpret_cast<std::uint16_t *>(dataOut);

        for (std::int32_t inputCol = windowWidthPx - 1; inputCol >= 0; --inputCol) {
            for (std::int32_t inputRow = windowHeightPx - 1; inputRow >= 7; inputRow -= pixelsInByte) {
                // HACK: Did not create the loop for accessing pixels and merging them in the single byte for better
                // performance.
                // Wanted to avoid unneeded loop count increasing and jump operations which for large amount of data
                // take considerable amount of time.
                const std::uint32_t index = inputRow * BOARD_EINK_DISPLAY_RES_X + inputCol;

                // Use the LUT to convert the input pixel from 4bpp to 2bpp and put 4 pixels in single byte
                std::uint8_t temp = (einkMaskLut2Bpp[dataIn[index - 0 * BOARD_EINK_DISPLAY_RES_X]] << 6);
                temp |= (einkMaskLut2Bpp[dataIn[index - 1 * BOARD_EINK_DISPLAY_RES_X]] << 4);
                temp |= (einkMaskLut2Bpp[dataIn[index - 2 * BOARD_EINK_DISPLAY_RES_X]] << 2);
                temp |= (einkMaskLut2Bpp[dataIn[index - 3 * BOARD_EINK_DISPLAY_RES_X]] << 0);

                // Push the 4 pixels into the proper place in the uint16_t
                std::uint16_t pixels = (temp << 0);

                // Use the LUT to convert the input pixel from 4bpp to 2bpp and put 4 pixels in single byte
                temp = (einkMaskLut2Bpp[dataIn[index - 4 * BOARD_EINK_DISPLAY_RES_X]] << 6);
                temp |= (einkMaskLut2Bpp[dataIn[index - 5 * BOARD_EINK_DISPLAY_RES_X]] << 4);
                temp |= (einkMaskLut2Bpp[dataIn[index - 6 * BOARD_EINK_DISPLAY_RES_X]] << 2);
                temp |= (einkMaskLut2Bpp[dataIn[index - 7 * BOARD_EINK_DISPLAY_RES_X]] << 0);

                // Push the 4 pixels into the proper place in the uint16_t
                pixels |= (temp << 8);

                if (invertColors == EinkDisplayColorMode::Inverted) {
                    pixels = ~pixels;
                }

                *outArray = pixels;
                ++outArray;
            }
        }

        return dataOut;
    }

    __attribute__((optimize("O1"))) std::uint8_t *transformFrameCoordinateSystem3Bpp(const std::uint8_t *dataIn,
                                                                                     std::uint16_t windowWidthPx,
                                                                                     std::uint16_t windowHeightPx,
                                                                                     std::uint8_t *dataOut,
                                                                                     EinkDisplayColorMode invertColors)
    {
        // The 4bpp is coded the same way as the 3bpp
        return transformFrameCoordinateSystem4Bpp(dataIn, windowWidthPx, windowHeightPx, dataOut, invertColors);
    }

    __attribute__((optimize("O1"))) std::uint8_t *transformAnimationFrameCoordinateSystem1Bpp(
        const std::uint8_t *dataIn,
        std::uint16_t windowWidthPx,
        std::uint16_t windowHeightPx,
        std::uint8_t *dataOut,
        EinkDisplayColorMode invertColors)
    {
        // In 1bpp mode there are 8 pixels in the byte
        constexpr std::uint8_t pixelsInByte = 8;

        auto outArray = dataOut;

        for (std::int32_t inputCol = windowWidthPx - 1; inputCol >= 0; --inputCol) {
            for (std::int32_t inputRow = windowHeightPx - 1; inputRow >= 7; inputRow -= pixelsInByte) {
                // HACK: Did not create the loop for accessing pixels and merging them in the single byte for better
                // performance.
                // Wanted to avoid unneeded loop count increasing and jump operations which for large amount of
                // data take considerable amount of time.

                const std::uint32_t index = inputRow * BOARD_EINK_DISPLAY_RES_X + inputCol;

                // Use the LUT to convert the input pixel from 4bpp to 1bpp
                const auto firstPixel   = einkMaskLut1Bpp[dataIn[index - 0 * BOARD_EINK_DISPLAY_RES_X]];
                const auto secondPixel  = einkMaskLut1Bpp[dataIn[index - 1 * BOARD_EINK_DISPLAY_RES_X]];
                const auto thirdPixel   = einkMaskLut1Bpp[dataIn[index - 2 * BOARD_EINK_DISPLAY_RES_X]];
                const auto fourthPixel  = einkMaskLut1Bpp[dataIn[index - 3 * BOARD_EINK_DISPLAY_RES_X]];
                const auto fifthPixel   = einkMaskLut1Bpp[dataIn[index - 4 * BOARD_EINK_DISPLAY_RES_X]];
                const auto sixthPixel   = einkMaskLut1Bpp[dataIn[index - 5 * BOARD_EINK_DISPLAY_RES_X]];
                const auto seventhPixel = einkMaskLut1Bpp[dataIn[index - 6 * BOARD_EINK_DISPLAY_RES_X]];
                const auto eightPixel   = einkMaskLut1Bpp[dataIn[index - 7 * BOARD_EINK_DISPLAY_RES_X]];

                // Put the pixels in order: Most left positioned pixel at the most significant side of byte
                std::uint8_t pixels = (firstPixel << 7) | (secondPixel << 6) | (thirdPixel << 5) | (fourthPixel << 4) |
                                      (fifthPixel << 3) | (sixthPixel << 2) | (seventhPixel << 1) | (eightPixel << 0);

                if (invertColors == EinkDisplayColorMode::Inverted) {
                    pixels = ~pixels;
                }

                *outArray = pixels;
                ++outArray;
            }
        }

        return dataOut;
    }

    __attribute__((optimize("O1"))) std::uint8_t *transformAnimationFrameCoordinateSystem2Bpp(
        const std::uint8_t *dataIn,
        std::uint16_t windowWidthPx,
        std::uint16_t windowHeightPx,
        std::uint8_t *dataOut,
        EinkDisplayColorMode invertColors)
    {
        // In 2bpp mode there are 4 pixels in the byte
        constexpr std::uint8_t pixelsInByte = 4;

        auto outArray = reinterpret_cast<std::uint16_t *>(dataOut);

        for (std::int32_t inputCol = windowWidthPx - 1; inputCol >= 0; --inputCol) {
            for (std::int32_t inputRow = windowHeightPx - 1; inputRow >= 7; inputRow -= pixelsInByte) {
                // HACK: Did not create the loop for accessing pixels and merging them in the single byte for better
                // performance.
                // Wanted to avoid unneeded loop count increasing and jump operations which for large amount of data
                // take considerable amount of time.
                const std::uint32_t index = inputRow * BOARD_EINK_DISPLAY_RES_X + inputCol;

                // Use the LUT to convert the input pixel from 4bpp to 2bpp and put 4 pixels in single byte
                std::uint8_t temp = (einkMaskLut2Bpp[dataIn[index - 0 * BOARD_EINK_DISPLAY_RES_X]] << 6);
                temp |= (einkMaskLut2Bpp[dataIn[index - 1 * BOARD_EINK_DISPLAY_RES_X]] << 4);
                temp |= (einkMaskLut2Bpp[dataIn[index - 2 * BOARD_EINK_DISPLAY_RES_X]] << 2);
                temp |= (einkMaskLut2Bpp[dataIn[index - 3 * BOARD_EINK_DISPLAY_RES_X]] << 0);

                // Use the LUT to binarize the 2bpp pixels value and push them into the proper place in the uint16_t
                std::uint16_t pixels = einkBinarizationLUT2bpp[temp] << 0;

                // Use the LUT to convert the input pixel from 4bpp to 2bpp and put 4 pixels in single byte
                temp = (einkMaskLut2Bpp[dataIn[index - 4 * BOARD_EINK_DISPLAY_RES_X]] << 6);
                temp |= (einkMaskLut2Bpp[dataIn[index - 5 * BOARD_EINK_DISPLAY_RES_X]] << 4);
                temp |= (einkMaskLut2Bpp[dataIn[index - 6 * BOARD_EINK_DISPLAY_RES_X]] << 2);
                temp |= (einkMaskLut2Bpp[dataIn[index - 7 * BOARD_EINK_DISPLAY_RES_X]] << 0);

                // Use the LUT to binarize the 2bpp pixels value and push them into the proper place in the uint16_t
                pixels |= (einkBinarizationLUT2bpp[temp] << 8);
                if (invertColors == EinkDisplayColorMode::Inverted) {
                    pixels = ~pixels;
                }

                *outArray = pixels;
                ++outArray;
            }
        }

        return dataOut;
    }

    __attribute__((optimize("O1"))) std::uint8_t *transformAnimationFrameCoordinateSystem3Bpp(
        const std::uint8_t *dataIn,
        std::uint16_t windowWidthPx,
        std::uint16_t windowHeightPx,
        std::uint8_t *dataOut,
        EinkDisplayColorMode invertColors)
    {
        // The 4bpp is coded the same way as the 3bpp
        return transformFrameCoordinateSystem4Bpp(dataIn, windowWidthPx, windowHeightPx, dataOut, invertColors);
    }

    __attribute__((optimize("O1"))) std::uint8_t *transformFrameCoordinateSystem4Bpp(const std::uint8_t *dataIn,
                                                                                     std::uint16_t windowWidthPx,
                                                                                     std::uint16_t windowHeightPx,
                                                                                     std::uint8_t *dataOut,
                                                                                     EinkDisplayColorMode invertColors)
    {
        // In 3bpp and 4bpp modes there are 2 pixels in the byte. Using 8bpp to process the whole uint32_t at once for
        // faster execution
        constexpr std::uint8_t pixelsInByte = 8;

        auto outArray = reinterpret_cast<std::uint32_t *>(dataOut);

        for (std::int32_t inputCol = windowWidthPx - 1; inputCol >= 0; --inputCol) {
            for (std::int32_t inputRow = windowHeightPx - 1; inputRow >= 7; inputRow -= pixelsInByte) {
                // HACK: Did not create the loop for accessing pixels and merging them in the single byte for better
                // performance.
                // Wanted to avoid unneeded loop count increasing and jump operations which for large amount of data
                // take considerable amount of time. Using 8 pixels at a time for better performance
                const std::uint32_t index = inputRow * BOARD_EINK_DISPLAY_RES_X + inputCol;

                const auto firstPixelPair =
                    (dataIn[index - 0 * BOARD_EINK_DISPLAY_RES_X] << 4) | dataIn[index - 1 * BOARD_EINK_DISPLAY_RES_X];
                const auto secondPixelPair =
                    (dataIn[index - 2 * BOARD_EINK_DISPLAY_RES_X] << 4) | dataIn[index - 3 * BOARD_EINK_DISPLAY_RES_X];
                const auto thirdPixelPair =
                    (dataIn[index - 4 * BOARD_EINK_DISPLAY_RES_X] << 4) | dataIn[index - 5 * BOARD_EINK_DISPLAY_RES_X];
                const auto fourthPixelPair =
                    (dataIn[index - 6 * BOARD_EINK_DISPLAY_RES_X] << 4) | dataIn[index - 7 * BOARD_EINK_DISPLAY_RES_X];

                // Put the pixels in the uint32_t for faster processing
                std::uint32_t pixels =
                    (firstPixelPair << 0) | (secondPixelPair << 8) | (thirdPixelPair << 16) | (fourthPixelPair << 24);

                if (invertColors == EinkDisplayColorMode::Inverted) {
                    pixels = ~pixels;
                }

                // Put the pixels in order: Most left positioned pixel at the most significant side of byte
                *outArray = pixels;
                ++outArray;
            }
        }

        return dataOut;
    }

    __attribute__((optimize("O1"))) std::uint8_t *transformFrameCoordinateSystemNoRotation4Bpp(
        const std::uint8_t *dataIn,
        std::uint16_t windowWidthPx,
        std::uint16_t windowHeightPx,
        std::uint8_t *dataOut,
        EinkDisplayColorMode invertColors)
    {
        // In 3bpp and 4bpp modes there are 2 pixels in the byte. Using 8bpp to process the whole uint32_t at once for
        // faster execution
        const std::uint8_t pixelsInByte = 8;

        auto outArray = reinterpret_cast<std::uint32_t *>(dataOut);

        for (std::int32_t inputRow = 0; inputRow < windowHeightPx; ++inputRow) {
            for (std::int32_t inputCol = windowWidthPx - 7; inputCol >= 0; inputCol -= pixelsInByte) {
                // HACK: Did not create the loop for accessing pixels and merging them in the single byte for better
                // performance.
                // Wanted to avoid unneeded loop count increasing and jump operations which for large amount of data
                // take considerable amount of time. Using 8 pixels at a time for better performance
                const std::uint32_t index = inputRow * BOARD_EINK_DISPLAY_RES_X + inputCol;

                // Get 4x 2 adjacent pixels to process them as uint32_t for better execution timings
                const auto firstPixelPair  = (dataIn[index]) | (dataIn[index + 1] << 4);
                const auto secondPixelPair = (dataIn[index + 2]) | (dataIn[index + 3] << 4);
                const auto thirdPixelPair  = (dataIn[index + 4]) | (dataIn[index + 5] << 4);
                const auto fourthPixelPair = (dataIn[index + 6]) | (dataIn[index + 7] << 4);

                // Put the pixels in the uint32_t for faster processing
                std::uint32_t pixels =
                    (firstPixelPair << 24) | (secondPixelPair << 16) | (thirdPixelPair << 8) | (fourthPixelPair);

                if (invertColors == EinkDisplayColorMode::Inverted) {
                    pixels = ~pixels;
                }

                // Put the pixels in order: Most left positioned pixel at the most significant side of byte
                *outArray = pixels;
                ++outArray;
            }
        }

        return dataOut;
    }
} // namespace bsp::eink
