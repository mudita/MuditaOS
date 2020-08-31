#include "CheckBoxWithLabelAndModel.hpp"

namespace gui
{

    CheckBoxWithLabelAndModel::CheckBoxWithLabelAndModel(app::Application *application,
                                                         const std::string &description,
                                                         bool checkIsOnLeftBarSide,
                                                         NewEditEventModel *model)
        : CheckBoxWithLabelItem(application, description, checkIsOnLeftBarSide), model(model)
    {
        app = application;
        assert(app != nullptr);
        applyCallbacks();
    }

    void CheckBoxWithLabelAndModel::applyCallbacks()
    {
        focusChangedCallback = [&](Item &item) {
            if (focus) {
                setFocusItem(checkBox);
            }
            else {
                setFocusItem(nullptr);
            }
            return true;
        };

        inputCallback = [&](gui::Item &item, const gui::InputEvent &event) {
            if (event.state != gui::InputEvent::State::keyReleasedShort) {
                return false;
            }
            if (checkBox->isChecked() && event.keyCode == gui::KeyCode::KEY_LF) {
                LOG_DEBUG("reloadDataWithTimeItem");
                model->reloadDataWithTimeItem();
            }
            else if (!checkBox->isChecked() && event.keyCode == gui::KeyCode::KEY_LF) {
                LOG_DEBUG("loadDataWithoutTimeItem");
                model->loadDataWithoutTimeItem();
            }

            if (checkBox->onInput(event)) {
                checkBox->resizeItems();
                app->getCurrentWindow()->bottomBarRestoreFromTemporaryMode();
                return true;
            }

            return false;
        };

        onLoadCallback = [&]() { checkBox->setImageVisible(false); };
    }

} /* namespace gui */
