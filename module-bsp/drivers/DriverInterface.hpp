/*
 *  @file DriverInterface.hpp
 *  @author Mateusz Piesta (mateusz.piesta@mudita.com)
 *  @date 29.07.19
 *  @brief  
 *  @copyright Copyright (C) 2019 mudita.com
 *  @details
 */



#ifndef PUREPHONE_DRIVERINTERFACE_HPP
#define PUREPHONE_DRIVERINTERFACE_HPP

#include <memory>
#include "critical.hpp"

namespace drivers {

    template<typename T>
    class DriverInterface {

    public:
        template<typename I, typename P>
        static std::shared_ptr<T> Create(const I instance, const P &params) {

            cpp_freertos::CriticalSection::Enter();
            std::shared_ptr<T> inst = singleton.lock();

            if (!inst) {
#if defined(TARGET_RT1051)
                inst = T::Create(instance, params);
#elif defined(TARGET_Linux)
#else
#error "Unsupported target"
#endif

                singleton = inst;
            }

            cpp_freertos::CriticalSection::Exit();

            return inst;
        }

    protected:
        static std::weak_ptr<T> singleton;
    };

    template<typename I>
    std::weak_ptr<I> DriverInterface<I>::singleton;
}


#endif //PUREPHONE_DRIVERINTERFACE_HPP
