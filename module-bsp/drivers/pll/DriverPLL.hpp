/*
 *  @file DriverPLL.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 09.08.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_DRIVERPLL_HPP
#define PUREPHONE_DRIVERPLL_HPP


#include "../DriverInterface.hpp"

namespace drivers {


    enum class PLLInstances {
        //TODO: M.P add the rest of plls
        Audio,
        Video,
        Ethernet,
        USB_1,
        USB_2
    };

    struct DriverPLLParams {

    };

    class DriverPLL : public DriverInterface<DriverPLL> {
    public:

        static std::shared_ptr<DriverPLL> Create(const PLLInstances inst, const DriverPLLParams &params);

        DriverPLL(const DriverPLLParams& params): parameters(params) {}

        virtual ~DriverPLL() {}

    protected:
        const DriverPLLParams parameters;

    };

}


#endif //PUREPHONE_DRIVERPLL_HPP
