#include "FactoryReset.hpp"
#include "vfs.hpp"
#include "service-db/includes/DBServiceName.hpp"
#include "SystemManager/SystemManager.hpp"

static int runcount          = 0;
static const int min_factory_dir_size = 4;
/* 4 is just an arbitrary value of min number of entries required in the factory reset directory.
 * Otherwise the factory reset dir might be corrupted.
 */
static const int max_recurse_depth = 120;
/* 120 is just an arbitrary value of max number of recursive calls.
 * If more then error is assumed, not the real depth of directories."
 */

void FactoryReset::Run(sys::Service *ownerService)
{
    if (runcount != 0) {
        return;
    }
    runcount = 1;

    LOG_INFO("FactoryReset: restoring factory state started...");

    std::vector<vfs::DirectoryEntry> dirlist = vfs.listdir(purefs::dir::os_factory.c_str(), "", true);

    if (dirlist.size() >= min_factory_dir_size) {
        LOG_INFO("FactoryReset: closing ServiceDB...");
        std::string dbServiceName = service::name::db;
        sys::SystemManager::DestroyService(dbServiceName, ownerService);

        DeleteDirContent(purefs::dir::eMMC_disk);

        CopyDirContent(purefs::dir::os_factory, purefs::dir::eMMC_disk);

        LOG_INFO("DoFactoryReset: restoring factory state finished, rebooting...");
        sys::SystemManager::Reboot(ownerService);
    }
    else {
        LOG_ERROR("FactoryReset: restoring factory state aborted");
        LOG_ERROR("FactoryReset: directory %s seems corrupted.", purefs::dir::os_factory.c_str());
    }
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

void FactoryReset::CopyDirContent(std::string sourcedir, std::string targetdir)
{
    static int recurseLevel = 0;

    recurseLevel++;

    if (recurseLevel < max_recurse_depth) {
        std::vector<vfs::DirectoryEntry> dirlist = vfs.listdir(sourcedir.c_str(), "", true);

        for (auto &direntry : dirlist) {
            if ((direntry.fileName.compare(".") != 0) && (direntry.fileName.compare("..") != 0) &&
                (direntry.fileName.compare("...") != 0)) {

                std::string sourcepath = sourcedir;
                sourcepath += "/";
                sourcepath += direntry.fileName.c_str();

                std::string targetpath = targetdir;
                targetpath += "/";
                targetpath += direntry.fileName.c_str();

                if ((sourcepath.size() < ffconfigMAX_FILENAME) && (targetpath.size() < ffconfigMAX_FILENAME)) {
                    if (direntry.attributes == vfs::FileAttributes::Directory) {
                        if (targetpath.compare(purefs::dir::os_factory.c_str()) != 0) {
                            LOG_INFO(
                                "FactoryReset: restoring dir  %s into %s...", sourcepath.c_str(), targetpath.c_str());

                            vfs.mkdir(targetpath.c_str());

                            CopyDirContent(sourcepath, targetpath);
                        }
                    }
                    else {
                        LOG_INFO("FactoryReset: restoring file %s into %s...", sourcepath.c_str(), targetpath.c_str());

                        CopyFile(sourcepath, targetpath);
                    }
                }
                else {
                    LOG_ERROR("FactoryReset: path length (source or target) exceeds system limit of %d",
                              ffconfigMAX_FILENAME);
                    LOG_ERROR(
                        "FactoryReset: skipping restore of dir %s into %s", sourcepath.c_str(), targetpath.c_str());
                }
            }
        }
    }
    else {
        LOG_ERROR("FactoryReset: recurse level %d (too high), error assumed, skipping restore of dir %s",
                  recurseLevel,
                  sourcedir.c_str());
    }

    recurseLevel--;
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
