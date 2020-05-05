/*
 * ThreadeModel.hpp
 *
 *  Created on: 13 lis 2019
 *      Author: kuba
 */

#pragma once

#include <vector>
#include <stdint.h>

#include "DatabaseModel.hpp"
#include "Application.hpp"
#include "ListItemProvider.hpp"
#include "Interface/ThreadRecord.hpp"

class ThreadModel : public app::DatabaseModel<ThreadRecord>, public gui::ListItemProvider
{
  public:
    ThreadModel() = delete;
    ThreadModel(app::Application *app);
    virtual ~ThreadModel() = default;

    void requestRecordsCount() override;
    bool updateRecords(std::unique_ptr<std::vector<ThreadRecord>> records,
                       const uint32_t offset,
                       const uint32_t limit,
                       uint32_t count) override;
    void requestRecords(const uint32_t offset, const uint32_t limit) override;

    gui::ListItem *getItem(int index) override;
    int getItemCount() const override
    {
        return recordsCount;
    };
    app::Application *getApplication(void)
    {
        return application;
    };
};
