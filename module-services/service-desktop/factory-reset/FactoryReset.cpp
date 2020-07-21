#include "FactoryReset.hpp"
#include "vfs.hpp"
#include "service-db/includes/DBServiceName.hpp"
#include "SystemManager/SystemManager.hpp"

static int runcount                   = 0;
static int recurseLevel               = 0;
static const int min_factory_dir_size = 4;
/* 4 is just an arbitrary value of min number of entries required in the factory reset directory.
 * Otherwise the factory reset dir might be corrupted.
 */
static const int max_recurse_depth = 120;
/* 120 is just an arbitrary value of max number of recursive calls.
 * If more then error is assumed, not the real depth of directories."
 */

bool FactoryReset::Run(sys::Service *ownerService)
{
    if (runcount != 0) {
        return false;
    }
    runcount = 1;

    LOG_INFO("FactoryReset: restoring factory state started...");

    recurseLevel = 0;

    std::vector<vfs::DirectoryEntry> dirlist = vfs.listdir(purefs::dir::os_factory.c_str(), "", true);

    if (dirlist.size() < min_factory_dir_size) {
        LOG_ERROR("FactoryReset: restoring factory state aborted");
        LOG_ERROR("FactoryReset: directory %s seems corrupted.", purefs::dir::os_factory.c_str());
        return false;
    }

    LOG_INFO("FactoryReset: closing ServiceDB...");
    std::string dbServiceName = service::name::db;
    (void)sys::SystemManager::DestroyService(dbServiceName, ownerService);

    if (DeleteDirContent(purefs::dir::eMMC_disk) != true) {
        LOG_ERROR("FactoryReset: restoring factory state aborted");
        return false;
    }

    if (CopyDirContent(purefs::dir::os_factory, purefs::dir::eMMC_disk) != true) {
        LOG_ERROR("FactoryReset: restoring factory state aborted");
        return false;
    }

    LOG_INFO("DoFactoryReset: restoring factory state finished, rebooting...");
    sys::SystemManager::Reboot(ownerService);
    return true;
}

bool FactoryReset::DeleteDirContent(std::string dir)
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
                    LOG_INFO("FactoryReset: recursively deleting dir %s...", delpath.c_str());
                    if (vfs.deltree(delpath.c_str()) != 0) {
                        LOG_ERROR("FactoryReset: error deleting dir %s, aborting...", delpath.c_str());
                        return false;
                    }
                }
            }
            else {
                LOG_INFO("FactoryReset: deleting file %s...", delpath.c_str());
                if (vfs.remove(delpath.c_str()) != 0) {
                    LOG_ERROR("FactoryReset: error deleting file %s, aborting...", delpath.c_str());
                    return false;
                }
            }
        }
    }

    return true;
}

bool FactoryReset::CopyDirContent(std::string sourcedir, std::string targetdir)
{
    bool ret = true;

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

                            if (vfs.mkdir(targetpath.c_str()) != 0) {
                                LOG_ERROR("FactoryReset: create dir %s failed", targetpath.c_str());
                                ret = false;
                                break;
                            }

                            if (CopyDirContent(sourcepath, targetpath) != true) {
                                ret = false;
                                break;
                            }
                        }
                    }
                    else {
                        LOG_INFO("FactoryReset: restoring file %s into %s...", sourcepath.c_str(), targetpath.c_str());

                        if (CopyFile(sourcepath, targetpath) != true) {
                            ret = false;
                            break;
                        }
                    }
                }
                else {
                    LOG_ERROR("FactoryReset: path length (source or target) exceeds system limit of %d",
                              ffconfigMAX_FILENAME);
                    LOG_ERROR(
                        "FactoryReset: skipping restore of dir %s into %s", sourcepath.c_str(), targetpath.c_str());
                    ret = false;
                    break;
                }
            }
        }
    }
    else {
        LOG_ERROR("FactoryReset: recurse level %d (too high), error assumed, skipping restore of dir %s",
                  recurseLevel,
                  sourcedir.c_str());
        ret = false;
    }

    recurseLevel--;

    return ret;
}

bool FactoryReset::CopyFile(std::string sourcefile, std::string targetfile)
{
    bool ret = true;

    vfs::FILE *sf = vfs.fopen(sourcefile.c_str(), "r");

    if (sf != nullptr) {
        vfs::FILE *tf = vfs.fopen(targetfile.c_str(), "w");

        if (tf != nullptr) {
            std::unique_ptr<unsigned char[]> buffer(new unsigned char[purefs::buffer::copy_buf]);

            if (buffer.get() != nullptr) {
                uint32_t loopcount = (vfs.filelength(sf) / purefs::buffer::copy_buf) + 1u;
                uint32_t readsize  = purefs::buffer::copy_buf;

                for (uint32_t i = 0u; i < loopcount; i++) {
                    if (i + 1u == loopcount) {
                        readsize = vfs.filelength(sf) % purefs::buffer::copy_buf;
                    }

                    if (vfs.fread(buffer.get(), 1, readsize, sf) != readsize) {
                        LOG_ERROR("FactoryReset: read from sourcefile %s failed", sourcefile.c_str());
                        ret = false;
                        break;
                    }

                    if (vfs.fwrite(buffer.get(), 1, readsize, tf) != readsize) {
                        LOG_ERROR("FactoryReset: write to targetfile %s failed", targetfile.c_str());
                        ret = false;
                        break;
                    }
                }
            }
            else {
                LOG_ERROR("FactoryReset: unable to open copy buffer");
                ret = false;
            }

            vfs.fclose(tf);
        }
        else {
            LOG_ERROR("FactoryReset: unable to open target file %s", targetfile.c_str());
            ret = false;
        }

        vfs.fclose(sf);
    }
    else {
        LOG_ERROR("FactoryReset: unable to open source file %s", sourcefile.c_str());
        ret = false;
    }

    return ret;
}
