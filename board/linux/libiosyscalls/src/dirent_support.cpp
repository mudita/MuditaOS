// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
#include "dirent_support.hpp"
#include <cstring>
#include <ff_stdio.h>
#include <errno.h>
#include <new>
#include <dirent.h>


namespace vfsn::linux::internal::diren
{
    DIR_ITER *diropen(int &_errno_, const char *path)
    {
        FF_Stat_t fs;
        if (ff_stat(path, &fs)) {
            _errno_ = stdioGET_ERRNO();
            if (_errno_ == EFAULT)
                _errno_ = ENOENT;
            return nullptr;
        }
        if (!(fs.st_mode & FF_IFDIR)) {
            _errno_ = ENOTDIR;
            return nullptr;
        }
        auto ret = new (std::nothrow) DIR_ITER;
        if (!ret) {
            _errno_ = ENOMEM;
            return nullptr;
        }
        ret->dir_state = nullptr;
        std::strncpy(ret->name_state, path, sizeof(ret->name_state));
        return ret;
    }

    int dirreset(int &_errno_, DIR_ITER *state)
    {
		if( !state ) {
			_errno_ = EINVAL;
			return -1;
		}
        if (state->dir_state) {
            delete reinterpret_cast<FF_FindData_t *>(state->dir_state);
            state->dir_state = nullptr;
            _errno_          = 0;
            return 0;
        }
        else {
            _errno_ = EBADF;
            return -1;
        }
    }
    int dirnext(int &_errno_, DIR_ITER *state)
    {
        int ret;
		if(!state) {
			_errno_ = EINVAL;
			return -1;
		}
        if (!state->dir_state) {
            auto fdd = new (std::nothrow) FF_FindData_t;
            if (!fdd) {
                _errno_ = ENOMEM;
                return -1;
            }
            std::memset(fdd, 0, sizeof(*fdd));
            ret              = ff_findfirst(state->name_state, fdd);
            state->dir_state = fdd;
        }
        else {
            auto fdd = static_cast<FF_FindData_t *>(state->dir_state);
            ret      = ff_findnext(fdd);
        }
        _errno_ = stdioGET_ERRNO();
        if (_errno_ == EFAULT)
            _errno_ = ENOENT;
        return ret;
    }

    int dirclose(int &_errno_, DIR_ITER *state)
    {
        if(!state) {
            _errno_ = 0;
            return 0;
        }
        if (state->dir_state) {
            delete reinterpret_cast<FF_FindData_t *>(state->dir_state);
        }
        delete state;
        _errno_ = 0;
        return 0;
    }
} // namespace vfsn::internal::dirent
