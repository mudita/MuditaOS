// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "gtest/gtest.h"

#include "gui/dom/Item2JsonSerializer.hpp"

#include "Item.hpp"
#include "Label.hpp"
#include "Text.hpp"
#include "FontManager.hpp"

#include <purefs/filesystem_paths.hpp>

class Item2JsonSerializerTester : public ::testing::Test
{
  protected:
    static constexpr auto testTextValue1 = "Some text 1";
    static constexpr auto testTextValue2 = "Some text 2";
    Item2JsonSerializerTester()
    {
        auto &fm = gui::FontManager::getInstance();
        fm.init(purefs::dir::getAssetsDirPath());

        auto text = new gui::Text(nullptr, 0, 0, 0, 0);
        text->setText(testTextValue1);

        root.addWidget(text);
        root.addWidget(new gui::Label(nullptr, 0, 0, 0, 0, testTextValue2));

        serializer.traverse(root);
        std::stringstream ss;
        serializer.dump(ss);
        serializedItem = ss.str();
    }
    gui::Item2JsonSerializer serializer;

  public:
    gui::Item root;
    std::string serializedItem;
};

TEST_F(Item2JsonSerializerTester, ChildrenCountTest)
{
    constexpr auto expectedRootChildrenCountPhrase  = "\"ChildrenCount\": 2";
    constexpr auto expectedChildChildrenCountPhrase = "\"ChildrenCount\": 0";

    const auto rootPhrasePos = serializedItem.find(expectedRootChildrenCountPhrase);
    ASSERT_NE(rootPhrasePos, std::string::npos);

    const auto firstChildPhrasePos = serializedItem.find(expectedChildChildrenCountPhrase);
    ASSERT_NE(rootPhrasePos, std::string::npos);

    const auto secondChildPhrasePos = serializedItem.find(expectedChildChildrenCountPhrase, firstChildPhrasePos);
    ASSERT_NE(secondChildPhrasePos, std::string::npos);
}

TEST_F(Item2JsonSerializerTester, TextValueTest)
{
    ASSERT_NE(serializedItem.find(testTextValue1), std::string::npos);
    ASSERT_NE(serializedItem.find(testTextValue2), std::string::npos);
}
