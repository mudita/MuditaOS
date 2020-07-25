#pragma once

#include <module-utils/microtar/src/microtar.hpp>
#include <vfs.hpp>
#include <json/json11.hpp>

class ServiceDesktop;
namespace fs = std::filesystem;
namespace updateos
{
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
        CantRenameTempToCurrent,
        CantUpdateJSON,
        CantSaveJSON
    };

    enum class BootloaderUpdateError
    {
        NoError,
        NoBootloaderFile,
        CantOpenBootloaderFile,
        CantAllocateBuffer,
        CantLoadBootloaderFile
    };

}; // namespace updateos

struct FileInfo
{
    FileInfo(mtar_header_t &h, unsigned long crc32);
    json11::Json to_json() const;

    std::string fileName;
    std::size_t fileSize;
    unsigned long fileCRC32;
};

class UpdatePureOS
{
  public:
    UpdatePureOS(ServiceDesktop *ownerService);

    updateos::UpdateError runUpdate();
    updateos::UpdateError prepareTempDirForUpdate();
    updateos::UpdateError unpackUpdate();
    updateos::UpdateError verifyChecksums();
    updateos::UpdateError prepareRoot();
    updateos::UpdateError updateBootJSON();
    updateos::UpdateError setUpdateFile(fs::path updateFileToUse);
    updateos::UpdateError cleanupAfterUpdate();

    updateos::BootloaderUpdateError writeBootloader(fs::path bootloaderFile);

    void getChecksumInfo(const std::string &infoLine, std::string &filePath, unsigned long *fileCRC32Long);
    unsigned long getExtractedFileCRC32(const std::string &filePath);
    bool unpackFileToTemp(mtar_header_t &header, unsigned long *crc32);
    const fs::path getUpdateTmpChild(const fs::path &childPath);

  private:

    fs::path updateTempDirectory;
    std::vector<FileInfo> filesInUpdatePackage;
    mtar_t updateTar      = {};
    ServiceDesktop *owner = nullptr;
    fs::path updateFile;
};
