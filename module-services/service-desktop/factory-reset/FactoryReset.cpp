#include "FactoryReset.hpp"
#include "vfs.hpp"
#include "service-db/includes/DBServiceName.hpp"
#include "SystemManager/SystemManager.hpp"

static uint32_t resetCounter = 0;
static int runcount          = 0;
#define TEST 0

void FactoryReset::Run(sys::Service *ownerService)
{
    if (runcount != 0) {
        return;
    }
    runcount = 1;

    resetCounter = 0;

    LOG_INFO("FactoryReset: restoring started");

    /* TOOO: add check if factory folder is not empty */
#if TEST
    std::string maindir = "/sys/main";
    DeleteDirContent(maindir);
    CopyDirContent(purefs::dir::os_factory, maindir);
#else
    LOG_INFO("FactoryReset: closing ServiceDB...");
    std::string dbServiceName = service::name::db;
    sys::SystemManager::DestroyService(dbServiceName, ownerService);
    DeleteDirContent(purefs::dir::eMMC_disk);
    CopyDirContent(purefs::dir::os_factory, purefs::dir::eMMC_disk);
#endif

    LOG_INFO("FactoryReset: restoring finished");

#if TEST
#else
    LOG_INFO("DoFactoryReset: factory reset finished, rebooting...");
    sys::SystemManager::Reboot(ownerService);
#endif
}

void FactoryReset::DeleteDirContent(std::string dir)
{
    std::vector<vfs::DirectoryEntry> dirlist = vfs.listdir(dir.c_str(), "", true);

    for (auto &direntry : dirlist) {
        if ((direntry.fileName.compare(".") != 0) && (direntry.fileName.compare("..") != 0) &&
            (direntry.fileName.compare("...") != 0)) {
            std::string delpath = dir;
            delpath += "/";
            delpath += direntry.fileName.c_str();

            if (direntry.attributes == vfs::FileAttributes::Directory) {
                if (direntry.fileName.compare(PATH_FACTORY) != 0) {
                    LOG_INFO("FactoryReset: recursively removing dir %s...", delpath.c_str());
                    vfs.deltree(delpath.c_str());
                }
            }
            else {
                LOG_INFO("FactoryReset: removing file %s...", delpath.c_str());
                vfs.remove(delpath.c_str());
            }
        }
    }
}

/* TODO: add maximum recurse depth limit, is there a limit for the FreeRTOS FAT or Linux? */
void FactoryReset::CopyDirContent(std::string sourcedir, std::string targetdir)
{
    std::vector<vfs::DirectoryEntry> dirlist = vfs.listdir(sourcedir.c_str(), "", true);

    for (auto &direntry : dirlist) {
        if ((direntry.fileName.compare(".") != 0) && (direntry.fileName.compare("..") != 0) &&
            (direntry.fileName.compare("...") != 0)) {
            resetCounter++;

            std::string sourcepath = sourcedir;
            sourcepath += "/";
            sourcepath += direntry.fileName.c_str();

            std::string targetpath = targetdir;
            targetpath += "/";
            targetpath += direntry.fileName.c_str();

            if (direntry.attributes == vfs::FileAttributes::Directory) {
                if (direntry.fileName.compare(PATH_FACTORY) != 0) {
                    LOG_INFO("FactoryReset: %lu, restoring dir %s into %s...",
                             resetCounter,
                             sourcepath.c_str(),
                             targetpath.c_str());

                    vfs.mkdir(targetpath.c_str());

                    CopyDirContent(sourcepath, targetpath);
                }
            }
            else {
                LOG_INFO("FactoryReset: %lu, restoring file %s into %s...",
                         resetCounter,
                         sourcepath.c_str(),
                         targetpath.c_str());
                CopyFile(sourcepath, targetpath);
            }
        }
    }
}

void FactoryReset::CopyFile(std::string sourcefile, std::string targetfile)
{
    vfs::FILE *sf = vfs.fopen(sourcefile.c_str(), "r");
    vfs::FILE *tf = vfs.fopen(targetfile.c_str(), "w");

    std::unique_ptr<unsigned char[]> buffer(new unsigned char[purefs::buffer::tar_buf]);

    uint32_t loopcount = (vfs.filelength(sf) / purefs::buffer::tar_buf) + 1u;
    uint32_t readsize  = purefs::buffer::tar_buf;

    for (uint32_t i = 0u; i < loopcount; i++) {
        if (i + 1u == loopcount) {
            readsize = vfs.filelength(sf) % purefs::buffer::tar_buf;
        }

        vfs.fread(buffer.get(), 1, readsize, sf);

        vfs.fwrite(buffer.get(), 1, readsize, tf);
    }

    vfs.fclose(sf);
    vfs.fclose(tf);
}
