// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "WindowsPopupRequest.hpp"

namespace gui::popup
{
    Request::Request(gui::popup::ID id, std::unique_ptr<gui::PopupRequestParams> &&params, Blueprint blueprint)
        : id(id), params(std::move(params)), blueprint(std::move(blueprint))
    {
        timeRequested = utils::time::getCurrentTimestamp();
    }

    bool Request::handle()
    {
        return blueprint(id, params);
    }

    bool Request::operator<(const Request &p) const
    {
        return params->getDisposition().priority > p.params->getDisposition().priority;
    }

    const gui::PopupRequestParams &Request::getPopupParams() const
    {
        return *params;
    }
} // namespace gui::popup
