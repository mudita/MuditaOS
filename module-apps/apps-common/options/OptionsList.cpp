// Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include "OptionsList.hpp"

namespace gui
{
    template <class ListType>
    OptionsList<ListType>::OptionsList(std::shared_ptr<OptionsModel> model, std::list<Option> options)
        : optionsModel{std::move(model)}, options(std::move(options))
    {}

    template <class ListType>
    void OptionsList<ListType>::createOptions()
    {
        optionsModel->createData(options);
    }

    template <class ListType>
    void OptionsList<ListType>::refreshOptions(std::list<Option> &&optionList)
    {
        options = std::move(optionList);
        optionsList->rebuildList(listview::RebuildType::InPlace);
    }

    template <class ListType>
    void OptionsList<ListType>::refreshOptions(std::list<Option> &&optionList, unsigned int pageIndex)
    {
        options = std::move(optionList);
        optionsList->rebuildList(listview::RebuildType::OnPageElement, pageIndex);
    }

    template <class ListType>
    void OptionsList<ListType>::addOptions(std::list<Option> &&optionList)
    {
        options = std::move(optionList);
        createOptions();

        optionsList->rebuildList();
    }

    template <class ListType>
    void OptionsList<ListType>::changeOptions(std::list<Option> &&optionList)
    {
        clearOptions();
        addOptions(std::move(optionList));
    }

    template <class ListType>
    void OptionsList<ListType>::recreateOptions()
    {
        clearOptions();
        createOptions();
    }

    template <class ListType>
    void OptionsList<ListType>::clearOptions()
    {
        optionsList->clear();
        optionsModel->clearData();
    }

} // namespace gui
