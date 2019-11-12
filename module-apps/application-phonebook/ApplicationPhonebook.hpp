#include "Application.hpp"

namespace app
{

class ApplicationPhonebook : public app::Application
{
  public:
    ApplicationPhonebook(std::string name = "ApplicationPhonebook", std::string parent = "",
                         bool startBackgound = false);
    virtual ~ApplicationPhonebook();

    sys::Message_t DataReceivedHandler(sys::DataMessage *msgl, sys::ResponseMessage *resp) override;
    sys::ReturnCodes InitHandler() override;
    sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final
    {
        return sys::ReturnCodes::Success;
    }

    void createUserInterface() override;
    void destroyUserInterface() override;
};

} // namespace app
