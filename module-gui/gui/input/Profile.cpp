// Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log/log.hpp>
#include "utf8/UTF8.hpp"
#include "Profile.hpp"
#include <Utils.hpp>
#include <gsl/gsl>

namespace gui
{

    Profile::Profile(const std::filesystem::path &filepath)
    {
        name       = filepath.stem();
        inputChars = createJson(filepath);
    }

    const std::string &Profile::getName() noexcept
    {
        return name;
    }

    const json11::Json &Profile::getJson() const noexcept
    {
        return inputChars;
    }

    const json11::Json Profile::createJson(const std::string &filepath)
    {
        auto fd = std::fopen(filepath.c_str(), "r");

        if (fd == nullptr) {
            LOG_FATAL("Error during opening file %s", filepath.c_str());
            return json11::Json();
        }

        uint32_t fsize = std::filesystem::file_size(filepath);

        auto stream = std::make_unique<char[]>(fsize + 1);

        memset(stream.get(), 0, fsize + 1);

        std::fread(stream.get(), 1, fsize, fd);

        std::string err;
        json11::Json parsedJson = json11::Json::parse(stream.get(), err);

        auto _ = gsl::finally([fd] { std::fclose(fd); });

        if (err.length() != 0) {
            LOG_FATAL("%s", err.c_str());
            return json11::Json();
        }
        return parsedJson;
    }

    uint32_t Profile::getCharKey(bsp::KeyCodes code, uint32_t times)
    {
        std::string ts = inputChars[utils::to_string(static_cast<int>(code))].string_value();
        UTF8 utf       = UTF8(ts);
        if (ts.size() > 0) {
            return utf[times % utf.length()];
        }
        return utf[0];
    }

} /* namespace gui */
