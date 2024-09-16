// Copyright (c) 2017-2024, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/blob/master/LICENSE.md

#pragma once

#include <widgets/text/core/TextDocument.hpp>

namespace mockup
{

    auto buildEmptyTestDocument() -> std::tuple<gui::TextDocument, gui::RawFont *>;

    auto buildTestDocument(gui::TextBlock::End lineEnd = gui::TextBlock::End::Newline)
        -> std::tuple<gui::TextDocument, gui::RawFont *>;

    auto buildOnelineTestDocument(std::string text) -> std::tuple<gui::TextDocument, gui::RawFont *>;

    auto buildMultilineTestDocument(std::list<std::string> source) -> std::tuple<gui::TextDocument, gui::RawFont *>;

    auto buildJustNewlineTestDocument() -> gui::TextDocument;
}; // namespace mockup
