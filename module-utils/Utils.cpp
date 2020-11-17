// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <time/time_conversion.hpp>
#include <random>
#include <ticks.hpp>
#include "Utils.hpp"

namespace utils
{
    namespace filesystem
    {
        bool replaceWithString(const fs::path &fileToModify, const std::string &stringToWrite)
        {
            auto lamb = [](vfs::FILE *stream) { vfs.fclose(stream); };
            std::unique_ptr<vfs::FILE, decltype(lamb)> fp(vfs.fopen(fileToModify.c_str(), "w"), lamb);

            if (fp.get() != nullptr) {
                size_t dataWritten = vfs.fprintf(fp.get(), stringToWrite.c_str());
                return dataWritten == stringToWrite.length();
            }
            else {
                return false;
            }
        }

        bool updateFileCRC32(const fs::path &file)
        {
            unsigned long fileCRC32 = 0;
            auto lamb               = [](FILE *stream) { fclose(stream); };

            std::unique_ptr<FILE, decltype(lamb)> fp(fopen(file.c_str(), "r"), lamb);

            if (fp.get() != nullptr) {
                auto crc32Buf = std::make_unique<char[]>(purefs::buffer::crc_char_size);
                int written   = 0;
                computeCRC32(fp.get(), &fileCRC32);
                LOG_INFO("updateFileCRC32 writing new crc32 %08" PRIX32 " for %s",
                         static_cast<std::uint32_t>(fileCRC32),
                         file.c_str());
                if (fileCRC32 != 0) {
                    if ((written = sprintf(crc32Buf.get(), "%08" PRIX32, fileCRC32)) !=
                        (purefs::buffer::crc_char_size - 1)) {
                        LOG_INFO("updateFileCRC32 can't prepare string for crc32, sprintf returned %d instead of %d",
                                 written,
                                 purefs::buffer::crc_char_size - 1);
                        return false;
                    }
                    fs::path fileCRC32Path = file;
                    fileCRC32Path += purefs::extension::crc32;

                    std::unique_ptr<FILE, decltype(lamb)> fpCRC32(fopen(fileCRC32Path.c_str(), "w"), lamb);

                    if (fpCRC32.get() != nullptr) {
                        if (fwrite(crc32Buf.get(), 1, purefs::buffer::crc_char_size, fpCRC32.get()) ==
                            purefs::buffer::crc_char_size) {
                            LOG_INFO("updateFileCRC32 wrote \"%s\" in %s", crc32Buf.get(), fileCRC32Path.c_str());
                            return true;
                        }
                        else {
                            LOG_WARN("updateFileCRC32 can't write new crc32");
                            return false;
                        }
                    }
                    else {
                        LOG_WARN("updateFileCRC32 can't open crc32 file for write");
                        return false;
                    }
                }
            }
            else {
                LOG_WARN("updateFileCRC32 can't open file %s for write", file.c_str());
                return false;
            }

            return false;
        }

        void computeCRC32(FILE *file, unsigned long *outCrc32)
        {
            if (outCrc32 == nullptr)
                return;

            std::unique_ptr<unsigned char[]> buf(new unsigned char[purefs::buffer::crc_buf]);
            size_t bufLen;

            *outCrc32 = 0;

            while (!feof(file)) {
                bufLen = fread(buf.get(), 1, purefs::buffer::crc_buf, file);
                if (bufLen <= 0)
                    break;

                *outCrc32 = Crc32_ComputeBuf(*outCrc32, buf.get(), bufLen);
            }
        }

        bool verifyCRC32Value(const std::string &filePath, const unsigned long crc32)
        {
            unsigned long crc32Read;
            auto lamb = [](FILE *stream) { fclose(stream); };

            std::unique_ptr<FILE, decltype(lamb)> fp(fopen(filePath.c_str(), "r"), lamb);

            if (fp.get() != nullptr) {
                utils::filesystem::computeCRC32(fp.get(), &crc32Read);
                LOG_INFO("verifyCRC computed crc32 for %s is %08" PRIX32,
                         filePath.c_str(),
                         static_cast<std::uint32_t>(crc32Read));
                return (crc32Read == crc32);
            }
            LOG_ERROR("verifyCRC can't open %s", filePath.c_str());
            return false;
        }

        bool verifyCRC(const fs::path filePath)
        {
            auto lamb = [](FILE *stream) { fclose(stream); };
            std::unique_ptr<char[]> crcBuf(new char[purefs::buffer::crc_char_size]);
            size_t readSize;
            fs::path crcFilePath(filePath);
            crcFilePath += purefs::extension::crc32;

            std::unique_ptr<FILE, decltype(lamb)> fp(fopen(crcFilePath.c_str(), "r"), lamb);

            if (fp.get() != nullptr) {
                if ((readSize = fread(crcBuf.get(), 1, purefs::buffer::crc_char_size, fp.get())) !=
                    (purefs::buffer::crc_char_size)) {
                    LOG_ERROR("verifyCRC fread on %s returned different size then %d [%zu]",
                              crcFilePath.c_str(),
                              purefs::buffer::crc_char_size,
                              readSize);
                    return false;
                }

                const unsigned long crc32Read = strtoull(crcBuf.get(), nullptr, purefs::buffer::crc_radix);

                LOG_INFO("verifyCRC read %s string:\"%s\" hex:%08lX", crcFilePath.c_str(), crcBuf.get(), crc32Read);
                return verifyCRC32Value(filePath, crc32Read);
            }
            LOG_ERROR("verifyCRC can't open %s", crcFilePath.c_str());
            return false;
        }

        std::string generateRandomId(uint32_t length)
        {
            const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

            std::random_device random_device;
            std::mt19937 generator(random_device());
            generator.seed(utils::time::Timestamp().getTime());
            std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

            std::string random_string;

            for (std::size_t i = 0; i < length; ++i) {
                random_string += CHARACTERS[distribution(generator)];
            }

            return random_string;
        }

        std::string loadFileAsString(const fs::path &fileToLoad)
        {
            LOG_DEBUG("loadFileAsString %s", fileToLoad.c_str());
            auto lamb = [](FILE *stream) { fclose(stream); };
            std::unique_ptr<char[]> readBuf(new char[purefs::buffer::tar_buf]);
            std::unique_ptr<FILE, decltype(lamb)> fp(fopen(fileToLoad.c_str(), "r"), lamb);
            std::string contents;
            size_t readSize;

            if (fp.get() != nullptr) {
                while (!feof(fp.get())) {
                    readSize = fread(readBuf.get(), 1, purefs::buffer::tar_buf, fp.get());
                    contents.append(static_cast<const char *>(readBuf.get()), readSize);
                }
            }

            return contents;
        }
    } // namespace filesystem
} // namespace utils
