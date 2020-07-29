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
    vfs::FILE *fp = ::vfs.fopen(filePath.c_str(), "r");
    if (fp != NULL) {
        computeCRC32(fp, &crc32Read);
        ::vfs.fclose(fp);
        return (crc32Read == crc32);
    }
    LOG_ERROR("verifyCRC can't open %s", filePath.c_str());
    return (false);
}

bool vfs::verifyCRC(const fs::path filePath)
{
    std::unique_ptr<char[]> crcBuf(new char[purefs::buffer::crc_char_size]);

    fs::path crcFilePath(filePath);
    crcFilePath += purefs::extension::crc32;

    vfs::FILE *fp = ::vfs.fopen(crcFilePath.c_str(), "r");
    if (fp != NULL) {
        if (::vfs.fgets(crcBuf.get(), purefs::buffer::crc_char_size, fp) == NULL) {
            LOG_ERROR("verifyCRC ff_fgets on %s returned NULL", crcFilePath.c_str());
            return (false);
        }

        ::vfs.fclose(fp);
        const unsigned long crc32Read = strtoull(crcBuf.get(), nullptr, purefs::buffer::crc_radix);
        return (verifyCRC(filePath, crc32Read));
    }
    LOG_ERROR("verifyCRC can't open %s", crcFilePath.c_str());
    return (false);
}

std::string vfs::loadFileAsString(const fs::path &fileToLoad)
{
    std::unique_ptr<char[]> readBuf(new char[purefs::buffer::tar_buf]);
    FILE *fp = fopen(fileToLoad.c_str(), "r");
    std::string contents;
    size_t readSize;

    if (fp != NULL) {
        while (!eof(fp)) {
            readSize = fread(readBuf.get(), 1, purefs::buffer::tar_buf, fp);
            contents.append(static_cast<const char *>(readBuf.get()), readSize);
        }

        fclose(fp);
    }

    return contents;
}

bool vfs::replaceWithString(const fs::path &fileToModify, const std::string &stringToWrite)
{
    FILE *fp = fopen(fileToModify.c_str(), "w");
    if (fp != NULL) {
        size_t dataWritten = fprintf(fp, stringToWrite.c_str());
        fclose(fp);
        return dataWritten == stringToWrite.length();
    } else {
        return false;
    }

    return true;
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
    std::string parseErrors = "";

    LOG_INFO("vfs::getOSRootFromJSON parsing %s", bootJsonPath.c_str());
    json11::Json bootJson = json11::Json::parse(loadFileAsString(bootJsonPath), parseErrors);

    if (parseErrors == "") {
        boot_config.os_type           = bootJson[purefs::json::main][purefs::json::os_type].string_value();
        boot_config.os_image          = bootJson[purefs::json::main][purefs::json::os_image].string_value();
        boot_config.os_root_path      = purefs::dir::eMMC_disk / boot_config.os_type;
        boot_config.os_version        = std::string(VERSION);
        boot_config.boot_json         = bootJsonPath;
        boot_config.bootloader_verion = bootJson[purefs::json::bootloader][purefs::json::os_version].string_value();
        LOG_INFO("boot_config: %s", boot_config.to_json().dump().c_str());

        return true;
    }
    else {
        boot_config.os_type      = PATH_CURRENT;
        boot_config.os_root_path = purefs::dir::eMMC_disk / boot_config.os_type;
        boot_config.os_version   = std::string(VERSION);
        boot_config.boot_json    = bootJsonPath;

        LOG_WARN("vfs::getOSRootFromJSON failed to parse %s: \"%s\"", bootJsonPath.c_str(), parseErrors.c_str());

        return false;
    }
}

void vfs::updateTimestamp()
{
    boot_config.timestamp = std::to_string(utils::time::Timestamp().getTime());
    LOG_INFO("vfs::updateTimestamp timestamp: %s %ul", boot_config.timestamp.c_str(), utils::time::Timestamp().getTime());

    replaceWithString(boot_config.boot_json, boot_config.to_json().dump());
}

json11::Json purefs::boot_config_t::to_json() const
{
    return json11::Json::object{
        {purefs::json::main,
         json11::Json::object{{purefs::json::os_image, os_image},
                              {purefs::json::os_type, os_type},
                              {purefs::json::os_version, os_version},
                              {purefs::json::timestamp, timestamp}
        }},

        {purefs::json::git_info,
         json11::Json::object{{purefs::json::os_git_tag, std::string(GIT_TAG)},
                              {purefs::json::os_git_revision, std::string(GIT_REV)},
                              {purefs::json::os_git_branch, std::string(GIT_BRANCH)}}},
        {purefs::json::bootloader, json11::Json::object{{purefs::json::os_version, bootloader_verion}}}};
}
