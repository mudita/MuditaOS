#pragma once

#include "TextDocument.hpp"

namespace mockup
{

    auto buildEmptyTestDocument() -> std::tuple<gui::TextDocument, gui::Font *>;

    auto buildTestDocument() -> std::tuple<gui::TextDocument, gui::Font *>;

    auto buildOnelineTestDocument(std::string text) -> std::tuple<gui::TextDocument, gui::Font *>;

    auto buildMultilineTestDocument(std::list<std::string> source) -> std::tuple<gui::TextDocument, gui::Font *>;

    auto buildJustNewlineTestDocument() -> gui::TextDocument;
}; // namespace mockup
