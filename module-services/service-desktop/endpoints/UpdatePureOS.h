#pragma once

#include "module-utils/microtar/src/microtar.h"
#include "module-vfs/vfs.hpp"

#ifndef TARGET_Linux
#include "ff_stdio.h"
#define ff_rename(a, b) ff_rename(a, b, true)
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <filesystem>
int stdioGET_ERRNO();
#define FF_FILE        FILE
#define ff_fopen       fopen
#define ff_feof        feof
#define ff_fgets       fgets
#define ff_fwrite      fwrite
#define ff_fclose      fclose
#define ff_rename      rename
#define ff_stat        stat
#define FF_Stat_t      struct stat
#define ff_deltree     std::filesystem::remove_all
#define ff_mkdir(path) mkdir(path, S_IRWXU)
#endif

#include "json/json11.hpp"
#include <string.h>
#include <sstream>
#include <vector>
#include <random>
#include <time/time_conversion.hpp>

class ServiceDesktop;
namespace fs = std::filesystem;
namespace updateos
{
    namespace dir
    {
        const inline fs::path updates = purefs::dir::eMMC_disk / "updates";
        const inline fs::path tmp     = updates / "tmp";
    } // namespace dir

    namespace file
    {
        const inline std::string checksums = "checksums.txt";
        const inline std::string sql_mig   = "sqlmig.json";

    } // namespace file

    namespace extension
    {
        const inline std::string update = ".tar";
    }

    const inline int prefix_len = 8;

    enum class UpdateError
    {
        NoError,
        CantCreateTempDir,
        CantCreateUpdatesDir,
        CantRemoveUniqueTmpDir,
        CantCreateUniqueTmpDir,
        CantCreateExtractedFile,
        CantOpenChecksumsFile,
        VerifyChecksumsFailure,
        CantOpenUpdateFile,
        CantDeletePreviousOS,
        CantRenameCurrentToPrevious,
        CantRenameTempToCurrent
    };

}; // namespace updateos

struct FileInfo
{
    std::string fileName;
    std::size_t fileSize;
    unsigned long fileCRC32;

#ifndef TARGET_Linux
    FileInfo(FF_FindData_t *other) : fileName(other->pcFileName), fileSize(other->ulFileSize), fileCRC32(0)
    {}
#endif

    FileInfo(mtar_header_t &h, unsigned long crc32) : fileSize(h.size), fileCRC32(crc32)
    {
        if (h.name[0] == '.' && h.name[1] == '/') {
            // microtar relative paths, strip the leading ./away
            fileName = std::string(h.name).substr(2);
        }
        else {
            fileName = std::string(h.name);
        }
    }

    json11::Json to_json() const
    {
        return (json11::Json::object{
            {"name", fileName}, {"size", std::to_string(fileSize)}, {"crc32", std::to_string(fileCRC32)}});
    }
};

class UpdatePureOS
{
  public:
    UpdatePureOS(ServiceDesktop *ownerService);

    updateos::UpdateError runUpdate(const fs::path fileName);
    updateos::UpdateError prepareTempDirForUpdate();
    updateos::UpdateError unpackUpdate();
    updateos::UpdateError verifyChecksums();
    updateos::UpdateError prepareRoot();
    updateos::UpdateError updateBootINI();

    void getChecksumInfo(const std::string infoLine, std::string &filePath, unsigned long *fileCRC32Long);
    unsigned long getExtractedFileCRC32(const std::string filePath);
    const json11::Json getUpdateFileList();
    bool unpackFileToTemp(mtar_header_t &header, unsigned long *crc32);

    void cleanupAfterUpdate();
    bool fileExists(const fs::path pathToCheck);
    bool isDirectory(const fs::path pathToCheck);
    static std::string generateRandomId(size_t length);
    const fs::path getUpdateTmpChild(const fs::path childPath);
    static void printDir(const char *pcDirectoryToScan);

  private:
    fs::path updateTempDirectory;
    std::vector<FileInfo> filesInUpdatePackage;
    mtar_t updateTar;
    ServiceDesktop *owner = nullptr;
};
