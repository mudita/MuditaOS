#include "FactoryReset.hpp"
#include "vfs.hpp"
#include "service-db/includes/DBServiceName.hpp"
#include "SystemManager/SystemManager.hpp"

namespace FactoryReset
{
    static bool CopyFile(std::string sourcefile, std::string targetfile);

    static int recurseDepth             = 0;
    static const int empty_dirlist_size = 2;   /* vfs.listdir lists "." and ".." by default also for empty dir */
    static const int max_recurse_depth  = 120; /* 120 is just an arbitrary value of max number of recursive calls.
                                                * If more then error is assumed, not the real depth of directories."
                                                */
#ifdef TARGET_Linux
    static const int max_filepath_length = 4096;
#else
    static const int max_filepath_length = ffconfigMAX_FILENAME;
#endif

    bool Run(sys::Service *ownerService)
    {
        LOG_INFO("FactoryReset: restoring factory state started...");

        recurseDepth = 0;

        std::vector<vfs::DirectoryEntry> dirlist = vfs.listdir(purefs::dir::os_factory.c_str(), "");

        if (dirlist.size() <= empty_dirlist_size) {
            LOG_ERROR("FactoryReset: restoring factory state aborted");
            LOG_ERROR("FactoryReset: directory %s seems empty.", purefs::dir::os_factory.c_str());
            return false;
        }

        if (ownerService != nullptr) {
            LOG_INFO("FactoryReset: closing ServiceDB...");
            std::string dbServiceName = service::name::db;
            (void)sys::SystemManager::DestroyService(dbServiceName, ownerService);
        }

        if (DeleteDirContent(purefs::dir::eMMC_disk) != true) {
            LOG_ERROR("FactoryReset: restoring factory state aborted");
            return false;
        }

        if (CopyDirContent(purefs::dir::os_factory, purefs::dir::eMMC_disk) != true) {
            LOG_ERROR("FactoryReset: restoring factory state aborted");
            return false;
        }

        LOG_INFO("FactoryReset: restoring factory state finished, rebooting...");
        sys::SystemManager::Reboot(ownerService);
        return true;
    }

    bool DeleteDirContent(std::string dir)
    {
        std::vector<vfs::DirectoryEntry> dirlist = vfs.listdir(dir.c_str(), "");

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

    bool CopyDirContent(std::string sourcedir, std::string targetdir)
    {
        if (recurseDepth >= max_recurse_depth) {
            LOG_ERROR("FactoryReset: recurse level %d (too high), error assumed, skipping restore of dir %s",
                      recurseDepth,
                      sourcedir.c_str());
            return false;
        }

        std::vector<vfs::DirectoryEntry> dirlist = vfs.listdir(sourcedir.c_str(), "");

        for (auto &direntry : dirlist) {
            if ((direntry.fileName.compare(".") == 0) || (direntry.fileName.compare("..") == 0) ||
                (direntry.fileName.compare("...") == 0)) {
                continue;
            }

            std::string sourcepath = sourcedir;
            sourcepath += "/";
            sourcepath += direntry.fileName.c_str();

            std::string targetpath = targetdir;
            targetpath += "/";
            targetpath += direntry.fileName.c_str();

            if ((sourcepath.size() >= max_filepath_length) || (targetpath.size() >= max_filepath_length)) {
                LOG_ERROR("FactoryReset: path length (source or target) exceeds system limit of %d",
                          max_filepath_length);
                LOG_ERROR("FactoryReset: skipping restore of dir %s into %s", sourcepath.c_str(), targetpath.c_str());
                return false;
            }

            if (direntry.attributes == vfs::FileAttributes::Directory) {
                if (targetpath.compare(purefs::dir::os_factory.c_str()) == 0) {
                    continue;
                }

                LOG_INFO("FactoryReset: restoring dir  %s into %s...", sourcepath.c_str(), targetpath.c_str());

                if (vfs.mkdir(targetpath.c_str()) != 0) {
                    LOG_ERROR("FactoryReset: create dir %s failed", targetpath.c_str());
                    return false;
                }

                recurseDepth++;

                if (CopyDirContent(sourcepath, targetpath) != true) {
                    recurseDepth--;
                    return false;
                }

                recurseDepth--;
            }
            else {
                LOG_INFO("FactoryReset: restoring file %s into %s...", sourcepath.c_str(), targetpath.c_str());

                if (CopyFile(sourcepath, targetpath) != true) {
                    return false;
                }
            }
        }

        return true;
    }

    static bool CopyFile(std::string sourcefile, std::string targetfile)
    {
        bool ret  = true;
        auto lamb = [](vfs::FILE *stream) { vfs.fclose(stream); };

        std::unique_ptr<vfs::FILE, decltype(lamb)> sf(vfs.fopen(sourcefile.c_str(), "r"), lamb);
        std::unique_ptr<vfs::FILE, decltype(lamb)> tf(vfs.fopen(targetfile.c_str(), "w"), lamb);

        if ((sf.get() != nullptr) && (tf.get() != nullptr)) {
            std::unique_ptr<unsigned char[]> buffer(new unsigned char[purefs::buffer::copy_buf]);

            if (buffer.get() != nullptr) {
                uint32_t loopcount = (vfs.filelength(sf.get()) / purefs::buffer::copy_buf) + 1u;
                uint32_t readsize  = purefs::buffer::copy_buf;

                for (uint32_t i = 0u; i < loopcount; i++) {
                    if (i + 1u == loopcount) {
                        readsize = vfs.filelength(sf.get()) % purefs::buffer::copy_buf;
                    }

                    if (vfs.fread(buffer.get(), 1, readsize, sf.get()) != readsize) {
                        LOG_ERROR("FactoryReset: read from sourcefile %s failed", sourcefile.c_str());
                        ret = false;
                        break;
                    }

                    if (vfs.fwrite(buffer.get(), 1, readsize, tf.get()) != readsize) {
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
        }
        else {
            LOG_ERROR("FactoryReset: unable to open source or target file");
            ret = false;
        }

        return ret;
    }
} // namespace FactoryReset
