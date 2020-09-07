#include <vfs.hpp>
#include <time/time_conversion.hpp>
#include <random>
#include <ticks.hpp>
#include <source/version.hpp>

std::string vfs::generateRandomId(size_t length = 0)
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

void vfs::computeCRC32(vfs::FILE *file, unsigned long *outCrc32)
{
    if (outCrc32 == nullptr)
        return;

    std::unique_ptr<unsigned char[]> buf(new unsigned char[purefs::buffer::crc_buf]);
    size_t bufLen;

    *outCrc32 = 0;

    while (!::vfs.eof(file)) {
        bufLen = ::vfs.fread(buf.get(), 1, purefs::buffer::crc_buf, file);
        if (bufLen <= 0)
            break;

        *outCrc32 = Crc32_ComputeBuf(*outCrc32, buf.get(), bufLen);
    }
}

bool vfs::verifyCRC(const std::string filePath, const unsigned long crc32)
{
    unsigned long crc32Read;
    auto lamb = [](vfs::FILE *stream) { ::vfs.fclose(stream); };

    std::unique_ptr<vfs::FILE, decltype(lamb)> fp(::vfs.fopen(filePath.c_str(), "r"), lamb);

    if (fp.get() != nullptr) {
        computeCRC32(fp.get(), &crc32Read);
        LOG_INFO(
            "verifyCRC computed crc32 for %s is %08" PRIX32, filePath.c_str(), static_cast<std::uint32_t>(crc32Read));
        return (crc32Read == crc32);
    }
    LOG_ERROR("verifyCRC can't open %s", filePath.c_str());
    return (false);
}

bool vfs::verifyCRC(const fs::path filePath)
{
    auto lamb = [](vfs::FILE *stream) { ::vfs.fclose(stream); };
    std::unique_ptr<char[]> crcBuf(new char[purefs::buffer::crc_char_size]);
    size_t readSize;
    fs::path crcFilePath(filePath);
    crcFilePath += purefs::extension::crc32;

    std::unique_ptr<vfs::FILE, decltype(lamb)> fp(::vfs.fopen(crcFilePath.c_str(), "r"), lamb);

    if (fp.get() != nullptr) {
        if ((readSize = ::vfs.fread(crcBuf.get(), 1, purefs::buffer::crc_char_size, fp.get())) !=
            (purefs::buffer::crc_char_size)) {
            LOG_ERROR("verifyCRC fread on %s returned different size then %d [%zu]",
                      crcFilePath.c_str(),
                      purefs::buffer::crc_char_size,
                      readSize);
            return false;
        }

        const unsigned long crc32Read = strtoull(crcBuf.get(), nullptr, purefs::buffer::crc_radix);

        LOG_INFO("verifyCRC read %s string:\"%s\" hex:%08lX", crcFilePath.c_str(), crcBuf.get(), crc32Read);
        return verifyCRC(filePath, crc32Read);
    }
    LOG_ERROR("verifyCRC can't open %s", crcFilePath.c_str());
    return false;
}

std::string vfs::loadFileAsString(const fs::path &fileToLoad)
{
    auto lamb = [](vfs::FILE *stream) { ::vfs.fclose(stream); };
    std::unique_ptr<char[]> readBuf(new char[purefs::buffer::tar_buf]);
    std::unique_ptr<vfs::FILE, decltype(lamb)> fp(fopen(fileToLoad.c_str(), "r"), lamb);
    std::string contents;
    size_t readSize;

    if (fp.get() != nullptr) {
        while (!eof(fp.get())) {
            readSize = fread(readBuf.get(), 1, purefs::buffer::tar_buf, fp.get());
            contents.append(static_cast<const char *>(readBuf.get()), readSize);
        }
    }

    return contents;
}

bool vfs::replaceWithString(const fs::path &fileToModify, const std::string &stringToWrite)
{
    auto lamb = [](vfs::FILE *stream) { ::vfs.fclose(stream); };
    std::unique_ptr<vfs::FILE, decltype(lamb)> fp(fopen(fileToModify.c_str(), "w"), lamb);

    if (fp.get() != nullptr) {
        size_t dataWritten = fprintf(fp.get(), stringToWrite.c_str());
        return dataWritten == stringToWrite.length();
    }
    else {
        return false;
    }
}

const fs::path vfs::getCurrentBootJSON()
{
    if (verifyCRC(purefs::file::boot_json)) {
        return relativeToRoot(purefs::file::boot_json);
    }

    LOG_INFO("vfs::getCurrentBootJSON crc check failed on %s", purefs::file::boot_json.c_str());
    return relativeToRoot(purefs::file::boot_json);
}

bool vfs::loadBootConfig(const fs::path &bootJsonPath)
{
    std::string parseErrors  = "";
    std::string jsonContents = loadFileAsString(bootJsonPath);

    LOG_INFO("vfs::getOSRootFromJSON parsing %s", bootJsonPath.c_str());
    LOG_INFO("vfs::getOSRootFromJSON \"%s\"", jsonContents.c_str());

    bootConfig.boot_json_parsed = json11::Json::parse(jsonContents, parseErrors);

    if (parseErrors == "") {
        bootConfig.os_type  = bootConfig.boot_json_parsed[purefs::json::main][purefs::json::os_type].string_value();
        bootConfig.os_image = bootConfig.boot_json_parsed[purefs::json::main][purefs::json::os_image].string_value();
        bootConfig.os_root_path = purefs::dir::eMMC_disk / bootConfig.os_type;
        bootConfig.boot_json    = bootJsonPath;
        bootConfig.bootloader_verion =
            bootConfig.boot_json_parsed[purefs::json::bootloader][purefs::json::os_version].string_value();
        bootConfig.timestamp  = utils::time::Timestamp().str("%c");
        bootConfig.os_version = std::string(VERSION);

        LOG_INFO("boot_config: %s", bootConfig.to_json().dump().c_str());
        return true;
    }
    else {
        bootConfig.os_type      = PATH_CURRENT;
        bootConfig.os_image     = purefs::file::boot_bin;
        bootConfig.os_root_path = purefs::dir::eMMC_disk / bootConfig.os_type;
        bootConfig.boot_json    = bootJsonPath;
        bootConfig.timestamp    = utils::time::Timestamp().str("%c");
        bootConfig.os_version   = std::string(VERSION);

        LOG_WARN("vfs::getOSRootFromJSON failed to parse %s: \"%s\"", bootJsonPath.c_str(), parseErrors.c_str());
        return false;
    }
}

bool vfs::updateFileCRC32(const fs::path &file)
{
    unsigned long fileCRC32 = 0;
    auto lamb               = [](vfs::FILE *stream) { ::vfs.fclose(stream); };

    std::unique_ptr<vfs::FILE, decltype(lamb)> fp(fopen(file.c_str(), "r"), lamb);

    if (fp.get() != nullptr) {
        std::unique_ptr<char[]> crc32Buf(new char[purefs::buffer::crc_char_size]);
        int written = 0;
        computeCRC32(fp.get(), &fileCRC32);
        LOG_INFO("updateFileCRC32 writing new crc32 %08" PRIX32 " for %s",
                 static_cast<std::uint32_t>(fileCRC32),
                 file.c_str());
        if (fileCRC32 != 0) {
            if ((written = sprintf(crc32Buf.get(), "%08" PRIX32, fileCRC32)) != (purefs::buffer::crc_char_size - 1)) {
                LOG_INFO("updateFileCRC32 can't prepare string for crc32, sprintf returned %d instead of %d",
                         written,
                         purefs::buffer::crc_char_size - 1);
                return false;
            }
            fs::path fileCRC32Path = file;
            fileCRC32Path += purefs::extension::crc32;

            std::unique_ptr<vfs::FILE, decltype(lamb)> fpCRC32(fopen(fileCRC32Path.c_str(), "w"), lamb);

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

void vfs::updateTimestamp()
{
    bootConfig.timestamp = utils::time::Timestamp().str("%c");
    LOG_INFO("vfs::updateTimestamp \"%s\"", bootConfig.to_json().dump().c_str());

    if (replaceWithString(bootConfig.boot_json, bootConfig.to_json().dump())) {
        updateFileCRC32(bootConfig.boot_json);
    }
}

json11::Json purefs::BootConfig::to_json() const
{
    return json11::Json::object{
        {purefs::json::main,
         json11::Json::object{{purefs::json::os_image, os_image},
                              {purefs::json::os_type, os_type},
                              {purefs::json::os_version, os_version},
                              {purefs::json::timestamp, timestamp}}},

        {purefs::json::git_info,
         json11::Json::object{{purefs::json::os_git_tag, std::string(GIT_TAG)},
                              {purefs::json::os_git_revision, std::string(GIT_REV)},
                              {purefs::json::os_git_branch, std::string(GIT_BRANCH)}}},
        {purefs::json::bootloader, json11::Json::object{{purefs::json::os_version, bootloader_verion}}}};
}

// Method to compare two version strings
//   v1 <  v2  -> -1
//   v1 == v2  ->  0
int purefs::BootConfig::version_compare(const std::string &v1, const std::string &v2)
{
    size_t i = 0, j = 0;
    while (i < v1.length() || j < v2.length()) {
        int acc1 = 0, acc2 = 0;

        while (i < v1.length() && v1[i] != '.') {
            acc1 = acc1 * 10 + (v1[i] - '0');
            i++;
        }
        while (j < v2.length() && v2[j] != '.') {
            acc2 = acc2 * 10 + (v2[j] - '0');
            j++;
        }

        if (acc1 < acc2)
            return -1;
        if (acc1 > acc2)
            return +1;

        ++i;
        ++j;
    }
    return 0;
}
