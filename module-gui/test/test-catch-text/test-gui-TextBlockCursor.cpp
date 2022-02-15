// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <widgets/text/core/TextBlock.hpp>
#include <mock/buildTextDocument.hpp>

#include <catch2/catch.hpp>

TEST_CASE("Text BlockCursor Ctor/Dtor ")
{
    using namespace gui;

    SECTION("default is npos")
    {
        auto cursor = BlockCursor(nullptr, 0, 0, nullptr);
        REQUIRE(cursor.atBegin() == false);
        REQUIRE(cursor.atEnd() == false);
        REQUIRE(cursor.getPosition() == text::npos);
        REQUIRE(cursor.getBlockNumber() == text::npos);
        REQUIRE(cursor.operator bool() == false);
    }

    SECTION("default with empty document - is npos")
    {
        auto [empty_doc, font] = mockup::buildEmptyTestDocument();
        auto cursor            = BlockCursor(&empty_doc, 0, 0, nullptr);
        REQUIRE(cursor.atBegin() == false);
        REQUIRE(cursor.atEnd() == false);
        REQUIRE(cursor.getPosition() == text::npos);
        REQUIRE(cursor.getBlockNumber() == text::npos);
        REQUIRE(cursor.operator bool() == false);
    }

    SECTION("default with non empty document, from 0,0")
    {
        auto [doc, font] = mockup::buildTestDocument();
        auto cursor      = BlockCursor(&doc, 0, 0, nullptr);
        REQUIRE(cursor.atBegin() == true);
        REQUIRE(cursor.atEnd() == false);
        REQUIRE(cursor.getPosition() != text::npos);
        REQUIRE(cursor.getBlockNumber() != text::npos);
        REQUIRE(cursor.operator bool() == true);
    }

    SECTION("default with non empty document, form end -1")
    {
        REQUIRE(text::npos == std::numeric_limits<unsigned int>().max());
        auto [doc, font] = mockup::buildTestDocument();
        auto cursor      = BlockCursor(&doc, text::npos - 1, text::npos - 1, nullptr);
        REQUIRE(cursor.atBegin() == false);
        REQUIRE(cursor.atEnd() == true);
        REQUIRE(cursor.getPosition() != text::npos);
        REQUIRE(cursor.getBlockNumber() != text::npos);
        REQUIRE(cursor.operator bool() == true);
    }

    SECTION("default with non empty document, in the middle")
    {
        auto [doc, font] = mockup::buildTestDocument();
        auto block_nr    = doc.getBlocks().size() / 2;
        REQUIRE(block_nr > 0);
        auto block_len = std::next(doc.getBlocks().begin(), block_nr)->length();
        REQUIRE(block_len > 0);
        auto cursor = BlockCursor(&doc, block_len / 2, block_nr, nullptr);
        REQUIRE(cursor.atBegin() == false);
        REQUIRE(cursor.atEnd() == false);
        REQUIRE(cursor.getPosition() != text::npos);
        REQUIRE(cursor.getBlockNumber() != text::npos);
        REQUIRE(cursor.operator bool() == true);
    }

    SECTION("default with non empty document, in the end of doc")
    {
        auto [doc, font] = mockup::buildTestDocument();
        auto block_nr    = doc.getBlocks().size() - 1;
        REQUIRE(block_nr > 0);
        auto block_len = std::next(doc.getBlocks().begin(), block_nr)->length();
        REQUIRE(block_len == 0);
        auto cursor = BlockCursor(&doc, block_len, block_nr, nullptr);
        REQUIRE(cursor.atBegin() == false);
        REQUIRE(cursor.atEnd() == true);
        REQUIRE(cursor.getPosition() != text::npos);
        REQUIRE(cursor.getBlockNumber() != text::npos);
        REQUIRE(cursor.operator bool() == true);
    }
}

TEST_CASE("TextDocument <-> BlockCursor fencing tests")
{
    using namespace gui;
    auto [document, testfont] = mockup::buildTestDocument();
    REQUIRE(testfont != nullptr);
    auto &blocks = document.getBlocks();
    auto offset  = blocks.front().length();

    SECTION("begin - last char")
    {
        auto cursor = document.getBlockCursor(offset - 1);
        REQUIRE(cursor.getPosition() != text::npos);
        auto block_text = std::string(blocks.front().getText()); // cant do so on our utf..
        auto test_text  = std::string(block_text.begin() + offset - 1, block_text.end());
        auto text       = document.getText(cursor);
        REQUIRE(text == test_text);
        REQUIRE(cursor->getFormat()->getFont() == testfont);
    }

    SECTION("true fence hit - just next block")
    {
        auto cursor = document.getBlockCursor(offset);
        REQUIRE(cursor.getPosition() != text::npos);
        auto text = document.getText(cursor);

        REQUIRE(text == std::string(std::next(blocks.begin())->getText()));
        REQUIRE(cursor->getFormat()->getFont() == testfont);
    }

    SECTION("1st block + 1 -> 2nd block - 1st char")
    {
        auto cursor = document.getBlockCursor(offset + 1);
        REQUIRE(cursor.getPosition() != text::npos);
        auto block_text = std::string(std::next(blocks.begin())->getText());
        auto test_text  = std::string(block_text.begin() + 1, block_text.end());
        auto text       = document.getText(cursor);
        REQUIRE(text == test_text);
        REQUIRE(cursor->getFormat()->getFont() == testfont);
    }
    SECTION("fence fence hit - fence of textBlocks and block with newline")
    {
        auto cursor = document.getBlockCursor(document.getText().length());
        REQUIRE(cursor.getPosition() == 0);
        auto text = document.getText(cursor);
        REQUIRE(text == "");
    }

    SECTION("fence fence hit - fence of textBlocks and block without newline")
    {
        auto [document, testfont] = mockup::buildTestDocument(gui::TextBlock::End::None);
        auto cursor               = document.getBlockCursor(document.getText().length());
        REQUIRE(cursor.getPosition() == text::npos);
        auto text = document.getText(cursor);
        REQUIRE(text == "");
    }
}

TEST_CASE("TextDocument <-> BlockCursor operators: +, ++, -, -- tests")
{
    using namespace gui;

    /// tests below could probably be generated with some parametrized section testing suite option as all the tests are
    /// super similar

    SECTION("no document - (in/de)crementing npos does nothin")
    {
        auto cursor = BlockCursor(nullptr, 0, 0, nullptr);
        ++cursor;
        REQUIRE(cursor.atBegin() == false);
        REQUIRE(cursor.atEnd() == false);
        REQUIRE(cursor.getPosition() == text::npos);
        REQUIRE(cursor.getBlockNumber() == text::npos);
        REQUIRE(cursor.operator bool() == false);
        --cursor;
        REQUIRE(cursor.atBegin() == false);
        REQUIRE(cursor.atEnd() == false);
        REQUIRE(cursor.getPosition() == text::npos);
        REQUIRE(cursor.getBlockNumber() == text::npos);
        REQUIRE(cursor.operator bool() == false);
    }

    SECTION("empty document - (in/de)crementing npos does nothing")
    {
        auto [empty_doc, font] = mockup::buildEmptyTestDocument();
        auto cursor            = BlockCursor(&empty_doc, 0, 0, nullptr);
        ++cursor;
        REQUIRE(cursor.atBegin() == false);
        REQUIRE(cursor.atEnd() == false);
        REQUIRE(cursor.getPosition() == text::npos);
        REQUIRE(cursor.getBlockNumber() == text::npos);
        REQUIRE(cursor.operator bool() == false);
        --cursor;
        REQUIRE(cursor.atBegin() == false);
        REQUIRE(cursor.atEnd() == false);
        REQUIRE(cursor.getPosition() == text::npos);
        REQUIRE(cursor.getBlockNumber() == text::npos);
        REQUIRE(cursor.operator bool() == false);
    }

    SECTION("begin of block +  && ++ till the end of block && -- till the begining")
    {
        auto super_large_offset = 1000;
        auto [doc, font]        = mockup::buildTestDocument();
        auto cursor             = BlockCursor(&doc, 0, 0, nullptr);
        REQUIRE(super_large_offset > static_cast<int>(doc.getText().length()));
        ++cursor;
        REQUIRE(cursor.atBegin() == false);
        REQUIRE(cursor.atEnd() == false);
        REQUIRE(cursor.getPosition() != text::npos);
        REQUIRE(cursor.getBlockNumber() != text::npos);
        REQUIRE(cursor.operator bool() == true);
        cursor += super_large_offset;
        REQUIRE(cursor.atBegin() == false);
        REQUIRE(cursor.atEnd() == true);
        REQUIRE(cursor.getPosition() != text::npos);
        REQUIRE(cursor.getBlockNumber() != text::npos);
        REQUIRE(cursor.operator bool() == true);
        cursor -= super_large_offset;
        REQUIRE(cursor.atBegin() == true);
        REQUIRE(cursor.atEnd() == false);
        REQUIRE(cursor.getPosition() != text::npos);
        REQUIRE(cursor.getBlockNumber() != text::npos);
        REQUIRE(cursor.operator bool() == true);
    }

    SECTION("begin of block -  && --")
    {
        auto super_large_offset = 1000;
        auto [doc, font]        = mockup::buildTestDocument();
        auto cursor             = BlockCursor(&doc, 0, 0, nullptr);
        REQUIRE(super_large_offset > static_cast<int>(doc.getText().length()));
        REQUIRE(cursor.atBegin() == true);
        REQUIRE(cursor.atEnd() == false);
        REQUIRE(cursor.getPosition() != text::npos);
        REQUIRE(cursor.getBlockNumber() != text::npos);
        REQUIRE(cursor.operator bool() == true);
        --cursor;
        REQUIRE(cursor.atBegin() == true);
        REQUIRE(cursor.atEnd() == false);
        REQUIRE(cursor.getPosition() != text::npos);
        REQUIRE(cursor.getBlockNumber() != text::npos);
        REQUIRE(cursor.operator bool() == true);
        cursor -= super_large_offset;
        REQUIRE(cursor.atBegin() == true);
        REQUIRE(cursor.atEnd() == false);
        REQUIRE(cursor.getPosition() != text::npos);
        REQUIRE(cursor.getBlockNumber() != text::npos);
        REQUIRE(cursor.operator bool() == true);
    }

    SECTION("end document +  && ++")
    {
        auto [doc, font]       = mockup::buildTestDocument();
        auto end_position      = doc.getText().length();
        auto end_block         = doc.getBlocks().back().length() - 1;
        auto cursor            = BlockCursor(&doc, end_position, end_block, nullptr);
        auto any_offset_really = 1000;
        REQUIRE(cursor.atBegin() == false);
        REQUIRE(cursor.atEnd() == true);
        REQUIRE(cursor.getPosition() != text::npos);
        REQUIRE(cursor.getBlockNumber() != text::npos);
        REQUIRE(cursor.operator bool() == true);
        ++cursor;
        REQUIRE(cursor.atBegin() == false);
        REQUIRE(cursor.atEnd() == true);
        REQUIRE(cursor.getPosition() != text::npos);
        REQUIRE(cursor.getBlockNumber() != text::npos);
        REQUIRE(cursor.operator bool() == true);
        cursor += any_offset_really;
        REQUIRE(cursor.atBegin() == false);
        REQUIRE(cursor.atEnd() == true);
        REQUIRE(cursor.getPosition() != text::npos);
        REQUIRE(cursor.getBlockNumber() != text::npos);
        REQUIRE(cursor.operator bool() == true);
    }

    SECTION("block (in/de)cremention")
    {
        auto [doc, font]                     = mockup::buildTestDocument();
        auto cursor                          = BlockCursor(&doc, 0, 0, nullptr);
        unsigned int first_block_size_offset = doc.getBlocks().front().length() - 1;
        unsigned int check_offset            = 2;
        unsigned int block_move_penalty      = 1; // +1 because with first_block_size_offset we are still in 1st block
        REQUIRE(first_block_size_offset > check_offset);
        REQUIRE(first_block_size_offset < doc.getText().length());

        REQUIRE(cursor.atBegin() == true);
        REQUIRE(cursor.atEnd() == false);
        REQUIRE(cursor.getPosition() != text::npos);
        REQUIRE(cursor.getBlockNumber() != text::npos);
        REQUIRE(cursor.getBlockNumber() == 0);
        REQUIRE(cursor.operator bool() == true);
        cursor += first_block_size_offset + block_move_penalty;
        REQUIRE(cursor.getBlockNumber() == 1);
        REQUIRE(cursor.getPosition() == 0);

        cursor -= check_offset;
        REQUIRE(cursor.getBlockNumber() == 0);
        REQUIRE(cursor.getPosition() == first_block_size_offset - check_offset + block_move_penalty);

        for (unsigned int i = 0; i < check_offset; ++i) {
            ++cursor;
        }
        REQUIRE(cursor.getBlockNumber() == 1);
        REQUIRE(cursor.getPosition() == 0);
    }
}

TEST_CASE("add Char to empty")
{
    using namespace gui;
    {
        auto [doc, font] = mockup::buildEmptyTestDocument();
        auto cursor      = BlockCursor(&doc, 0, 0, nullptr);
        cursor.addChar('a');
        REQUIRE(doc.getText() == "a");
        REQUIRE(doc.getText().length() == 1);
    }
}

TEST_CASE("add Char")
{
    using namespace gui;
    auto [doc, font] = mockup::buildOnelineTestDocument("one line");
    auto cursor      = BlockCursor(&doc, 0, 0, nullptr);

    SECTION(" to second block -in the middle")
    {
        auto blocks    = doc.getBlocks();
        auto test_text = blocks.front().getText();
        test_text.insertCode('x', test_text.length() / 2);
        cursor += blocks.front().length() / 2;
        cursor.addChar('x');
        REQUIRE(doc.getBlocks().front().getText() == test_text);
    }

    SECTION("at the end of block")
    {
        auto blocks    = doc.getBlocks();
        auto test_text = blocks.front().getText();
        test_text.insertCode('x', test_text.length());
        cursor += blocks.front().length();
        cursor.addChar('x');
        REQUIRE(doc.getBlocks().front().getText() == test_text);
    }
}

TEST_CASE("remove Char in empty")
{
    using namespace gui;
    auto [doc, font] = mockup::buildEmptyTestDocument();
    auto cursor      = BlockCursor(&doc, 0, 0, nullptr);
    cursor.removeChar();
    REQUIRE(doc.getText() == "");
    REQUIRE(doc.getText().length() == 0);
}

TEST_CASE("remove Char")
{
    using namespace gui;
    auto [doc, font] = mockup::buildTestDocument();
    auto cursor      = BlockCursor(&doc, 0, 0, nullptr);

    SECTION("text remove")
    {
        UTF8 text = "a";
        text.removeChar(0);
        REQUIRE(text == "");
    }

    SECTION("remove first char")
    {
        auto no_of_chars_prior_to_remove = doc.getBlocks().begin()->length();
        ++cursor; // move cursor to point on first character
        REQUIRE(cursor.removeChar());
        REQUIRE(doc.getBlocks().begin()->length() == no_of_chars_prior_to_remove - 1);
    }

    SECTION("remove last char")
    {
        auto no_of_chars_prior_to_remove = doc.getBlocks().begin()->length();
        // move to end of first block
        cursor += no_of_chars_prior_to_remove - 1;
        REQUIRE(cursor.removeChar());
        REQUIRE(doc.getBlocks().begin()->length() == no_of_chars_prior_to_remove - 1);
    }

    SECTION("empty whole first block & remove first block ( with newline)")
    {
        auto block_count_start = doc.getBlocks().size();
        auto how_many          = doc.getBlocks().begin()->length();
        // move to end of first block
        cursor += how_many;
        for (unsigned int i = 0; i <= how_many + 1; ++i) {
            REQUIRE(cursor.removeChar());
            --cursor;
        }
        REQUIRE(block_count_start > 0);
        REQUIRE(doc.getBlocks().size() < block_count_start);
        REQUIRE(cursor.getBlockNumber() == 0);
    }

    SECTION("empty whole second block & remove second block")
    {
        auto start_block_nr    = cursor.getBlockNumber();
        auto block_count_start = doc.getBlocks().size();
        auto how_many          = std::next(doc.getBlocks().begin())->length();
        // move to end of second block +1 is for newline
        cursor += doc.getBlocks().front().length() + how_many - 1;
        for (unsigned int i = 0; i <= how_many + 1; ++i) {
            REQUIRE(cursor.removeChar());
            --cursor;
        }
        REQUIRE(block_count_start > 0);
        REQUIRE(doc.getBlocks().size() < block_count_start);
        REQUIRE(start_block_nr == cursor.getBlockNumber());
    }

    SECTION("block with just newline - remove newline block")
    {
        auto doc = mockup::buildJustNewlineTestDocument();
        auto cur = BlockCursor(&doc, 0, 0, nullptr);
        REQUIRE(!doc.isEmpty());
        cur.removeChar();
        REQUIRE(doc.isEmpty());
    }
}

TEST_CASE("add newline")
{
    using namespace gui;
    std::string text  = "some long text";
    auto [doc, font]  = mockup::buildOnelineTestDocument(text);
    auto cursor       = BlockCursor(&doc, 0, 0, nullptr);
    auto no_of_blocks = doc.getBlocks().size();

    cursor += text.find(" ");
    cursor.addChar('\n');
    REQUIRE(no_of_blocks < doc.getBlocks().size());
}

TEST_CASE("add newline at the end")
{
    using namespace gui;
    std::string text = "some long text.";
    auto [doc, font] = mockup::buildOnelineTestDocument(text);
    auto cursor      = BlockCursor(&doc, 0, 0, nullptr);

    cursor += text.size();
    cursor.addChar('\n');
    ++cursor;
    cursor.addChar('s');

    REQUIRE(doc.getBlocks().size() == 2);
    REQUIRE(doc.getBlocks().front().getText() == "some long text.\n");
    REQUIRE(doc.getBlocks().back().getText() == "s");
}

TEST_CASE("atEOL()")
{
    using namespace gui;
    std::string text = "some long text";
    auto [doc, font] = mockup::buildOnelineTestDocument(text);
    auto cursor      = BlockCursor(&doc, 0, 0, nullptr);

    REQUIRE(cursor.atEnd() == false);
    cursor += text.length();
    REQUIRE(cursor.atEnd() == true);
    cursor += text.length();
    REQUIRE(cursor.atEnd() == true);
}

TEST_CASE("operator-> for Text block with text")
{
    using namespace gui;
    std::string text = "some long text";
    auto [doc, font] = mockup::buildOnelineTestDocument(text);
    auto cursor      = BlockCursor(&doc, 0, 0, nullptr);

    // one block text len will be same for len of whole doc
    REQUIRE(cursor->length() == doc.getText().length());
}

TEST_CASE("operator* for TextBlock")
{
    using namespace gui;
    std::string text = "some long text";
    auto [doc, font] = mockup::buildOnelineTestDocument(text);
    auto cursor      = BlockCursor(&doc, 0, 0, nullptr);

    REQUIRE((*cursor).length() == doc.getText().length());
}

TEST_CASE("check sentence beginning")
{
    using namespace gui;
    auto [doc, font] = mockup::buildTestDocument();
    auto cursor      = BlockCursor(&doc, 0, 0, nullptr);

    auto addStringToBlock = [](const std::string &input, gui::BlockCursor &cursor) {
        for (unsigned int i = 0; i < input.length(); ++i) {
            cursor.addChar(input[i]);
            ++cursor;
        }
    };

    auto removeNSignsInBlock = [](unsigned int n, gui::BlockCursor &cursor) {
        for (unsigned int i = 0; i < n; ++i) {
            cursor.removeChar();
            --cursor;
        }
    };

    // Empty document sentence beginning.
    REQUIRE(cursor.checkSentenceBeginning());

    // Add empty spaces at document beginning - still sentence beginning
    addStringToBlock("  ", cursor);
    REQUIRE(cursor.checkSentenceBeginning());

    // Add one sign and check sentence beginning
    addStringToBlock("P", cursor);
    REQUIRE(!cursor.checkSentenceBeginning());

    // Remove one sign to check sentence beginning
    removeNSignsInBlock(1, cursor);
    REQUIRE(cursor.checkSentenceBeginning());

    // Add string ending with dot
    addStringToBlock("Test.", cursor);
    // No white space so not sentence beginning
    REQUIRE(!cursor.checkSentenceBeginning());

    // Add empty space. New sentence should be detected
    addStringToBlock(" ", cursor);
    REQUIRE(cursor.checkSentenceBeginning());

    // Remove space.
    removeNSignsInBlock(1, cursor);
    REQUIRE(!cursor.checkSentenceBeginning());

    // Add string ending with dot and white space
    addStringToBlock("Test. ", cursor);
    REQUIRE(cursor.checkSentenceBeginning());

    // Add extra sign
    addStringToBlock("P", cursor);
    REQUIRE(!cursor.checkSentenceBeginning());

    // Check number of blocks
    REQUIRE(cursor.getBlockNumber() == 0);

    // Add newline - new block and new sentence
    addStringToBlock("\n", cursor);
    // Check number of blocks
    REQUIRE(cursor.getBlockNumber() == 1);
    REQUIRE(cursor.checkSentenceBeginning());

    // Add string ending with dot
    addStringToBlock("Test.", cursor);
    // No white space so not sentence beginning
    REQUIRE(!cursor.checkSentenceBeginning());
}
