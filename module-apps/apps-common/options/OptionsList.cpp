// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OptionsList.hpp"

namespace gui
{
    OptionsList::OptionsList(std::shared_ptr<OptionsModel> model, std::list<Option> options)
        : optionsModel{std::move(model)}, options(std::move(options))
    {}

    void OptionsList::createOptions()
    {
        optionsModel->createData(options);
    }

    void OptionsList::refreshOptions(std::list<Option> &&optionList)
    {
        options = std::move(optionList);
        optionsList->rebuildList(listview::RebuildType::InPlace);
    }

    void OptionsList::refreshOptions(std::list<Option> &&optionList, unsigned int pageIndex)
    {
        options = std::move(optionList);
        optionsList->rebuildList(listview::RebuildType::OnPageElement, pageIndex);
    }

    void OptionsList::addOptions(std::list<Option> &&optionList)
    {
        options = std::move(optionList);
        createOptions();

        optionsList->rebuildList();
    }

    void OptionsList::changeOptions(std::list<Option> &&optionList)
    {
        clearOptions();
        addOptions(std::move(optionList));
    }

    void OptionsList::recreateOptions()
    {
        clearOptions();
        createOptions();
    }

    void OptionsList::clearOptions()
    {
        optionsList->clear();
        optionsModel->clearData();
    }
} // namespace gui
