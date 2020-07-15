#pragma once

#include "TextDocument.hpp"

namespace mockup
{

    auto buildEmptyTestDocument() -> std::tuple<gui::TextDocument, gui::RawFont *>;

    auto buildTestDocument() -> std::tuple<gui::TextDocument, gui::RawFont *>;

    auto buildOnelineTestDocument(std::string text) -> std::tuple<gui::TextDocument, gui::RawFont *>;

    auto buildMultilineTestDocument(std::list<std::string> source) -> std::tuple<gui::TextDocument, gui::RawFont *>;

    auto buildJustNewlineTestDocument() -> gui::TextDocument;
}; // namespace mockup
