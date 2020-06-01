#include <vfs.hpp>
#include <time/time_conversion.hpp>
#include <random>
#include <ticks.hpp>

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
