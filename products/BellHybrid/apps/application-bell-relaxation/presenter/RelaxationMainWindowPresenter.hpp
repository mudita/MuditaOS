// Copyright (c) 2017-2023, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#pragma once

#include "model/RelaxationSongsModel.hpp"
#include <apps-common/BasePresenter.hpp>
#include <module-db/Interface/MultimediaFilesRecord.hpp>
#include <vector>

namespace app::music
{
    class AbstractSongsRepository;
}
namespace app::relaxation
{

    class RelaxationMainWindowContract
    {
      public:
        class View
        {
          public:
            virtual ~View() = default;

            virtual void updateViewState()                                                             = 0;
            virtual void handleError()                                                                 = 0;
        };

        class Presenter : public BasePresenter<RelaxationMainWindowContract::View>
        {

          public:
            virtual void createData(RelaxationSongsModel::OnActivateCallback activateCallback) = 0;
            virtual void updateViewState()                                                     = 0;
            virtual std::shared_ptr<RelaxationSongsModel> getSongsModel()                      = 0;
        };
    };

    class RelaxationMainWindowPresenter : public RelaxationMainWindowContract::Presenter
    {
      private:
        std::shared_ptr<RelaxationSongsModel> songsModel;
        void createData(RelaxationSongsModel::OnActivateCallback activateCallback) override;
        void updateViewState() override;
        std::shared_ptr<RelaxationSongsModel> getSongsModel() override;

      public:
        explicit RelaxationMainWindowPresenter(std::unique_ptr<RelaxationSongsModel> songsModel);
    };

} // namespace app::relaxation
