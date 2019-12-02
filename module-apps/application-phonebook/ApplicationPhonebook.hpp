#include "Application.hpp"

namespace app {

inline const std::string name_phonebook = "ApplicationPhonebook";

class ApplicationPhonebook: public app::Application {
public:
	ApplicationPhonebook( std::string name=name_phonebook, std::string parent="", bool startBackgound = false);
	virtual ~ApplicationPhonebook();

	sys::Message_t DataReceivedHandler(sys::DataMessage* msgl,sys::ResponseMessage* resp) override;
	sys::ReturnCodes InitHandler() override;
	sys::ReturnCodes DeinitHandler() override;

    sys::ReturnCodes SwitchPowerModeHandler(const sys::ServicePowerMode mode) override final{return sys::ReturnCodes::Success;}

	void createUserInterface() override;
	void destroyUserInterface() override;
};

}
