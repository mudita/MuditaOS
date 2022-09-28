// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <gtest/gtest.h>
#include <apps-common/widgets/TimeFixedWidget.hpp>
#include <apps-common/widgets/BellBaseLayout.hpp>
#include <products/BellHybrid/apps/application-bell-background-sounds/data/BGSoundsStyle.hpp>
#include <gui/core/FontManager.hpp>
#include <mock/InitializedFontManager.hpp>

namespace
{

    void checkDisplayedText(std::string &&text, const gui::DigitsContainer &container)
    {
        std::for_each(std::crbegin(container.digits),
                      std::crend(container.digits),
                      [text = std::move(text)](gui::Label *label) mutable {
                          if (text.empty()) {
                              EXPECT_STREQ((label->getText()).c_str(), "");
                          }
                          else {
                              EXPECT_STREQ(label->getText().c_str(), &text.back());
                              text.pop_back();
                          }
                      });

        EXPECT_TRUE(text.empty());
    }

    template <size_t N>
    auto initContainer(gui::HBox &box) -> gui::DigitsContainer
    {
        auto container = gui::DigitsContainer{N};
        for (auto &digit : container.digits) {
            digit = new gui::Label(&box, 0, 0, 0, 0);
        }
        return container;
    }

} // namespace

TEST(DigitsContainerTest, setMinutesBox)
{
    using namespace std::string_literals;
    mockup::fontManager();

    std::unordered_map<int, std::string> inputAndExpectedOutput{{123, "123"s},
                                                                {999, "999"s},
                                                                {9999, "999"s},
                                                                {1233433, "433"s},
                                                                {99, "99"s},
                                                                {10, "10"s},
                                                                {1, "01"s},
                                                                {0, "00"s}};
    gui::HBox box{};
    auto container = initContainer<3>(box);

    for (auto &param : inputAndExpectedOutput) {
        container.setMinutesBox(param.first, gui::DimensionsParams{});
        checkDisplayedText(std::move(param.second), container);
    }
}

TEST(DigitsContainerTest, setSecondsBox)
{
    using namespace std::string_literals;
    mockup::fontManager();

    std::unordered_map<int, std::string> inputAndExpectedOutput{
        {123, "23"s}, {999, "99"s}, {9999, "99"s}, {1233433, "33"s}, {99, "99"s}, {10, "10"s}, {1, "01"s}, {0, "00"s}};
    gui::HBox box{};
    auto container = initContainer<2>(box);

    for (auto &param : inputAndExpectedOutput) {
        container.setSecondsBox(param.first, gui::DimensionsParams{});
        checkDisplayedText(std::move(param.second), container);
    }
}
