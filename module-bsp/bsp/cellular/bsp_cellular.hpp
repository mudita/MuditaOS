
/*
 * @file bsp_cellular.hpp
 * @author Mateusz Piesta (mateusz.piesta@mudita.com)
 * @date 14.06.19
 * @brief
 * @copyright Copyright (C) 2019 mudita.com
 * @details
 */


#ifndef PUREPHONE_BSP_CELLULAR_HPP
#define PUREPHONE_BSP_CELLULAR_HPP

#include <optional>
#include <stdint.h>
#include <stddef.h>
#include <memory>
#include <FreeRTOS.h>
#include <FreeRTOS/include/queue.h>

#include <common_data/EventStore.hpp>

namespace bsp {

    class Cellular {
    public:

        static std::optional<std::unique_ptr<Cellular>> Create(const char* term = "/dev/ttyUSB0", uint32_t portSpeed = 115200);

        Cellular() {}
        virtual ~Cellular() {}


        virtual void PowerUp() = 0;

        virtual void PowerDown() = 0;

        virtual void Restart() = 0;

        virtual uint32_t Wait(uint32_t timeout) = 0;

        virtual ssize_t Read(void *buf, size_t nbytes) = 0;

        virtual ssize_t Write(void *buf, size_t nbytes) = 0;

        virtual void InformModemHostAsleep() = 0;

		virtual void InformModemHostWakeup() = 0;

        virtual void EnterSleep() = 0;

        virtual void ExitSleep() = 0;

        virtual void SetSpeed(uint32_t portSpeed) = 0;

        virtual void SetSendingAllowed(bool state) = 0;
        virtual bool GetSendingAllowed()           = 0;

        virtual void SelectAntenna(uint8_t antenna) = 0;
        virtual uint8_t GetAntenna() = 0;

      protected:
        bool isInitialized = false;
    };

    namespace cellular
    {
        enum IRQsource{
            statusPin,
            trayPin,
        };

        /// initialize SIM queue directed to EventWorker
        int init(xQueueHandle qHandle);

        namespace status{
            enum class value {
                /// from the docs: When the module is turned on normally, the STATUS will present the low state.
                ACTIVE,
                INACTIVE,
            };
            /// returns true if status OK. Open drain pulled up; ground is OK.
            bsp::cellular::status::value getStatus();
            BaseType_t statusIRQhandler();
        }

        namespace sim
        {
            /// handler for SIM tray which is connected to phone, not GSM
            BaseType_t trayIRQ_handler();

            Store::GSM::Tray getTray();
            /// trigger swap pin on gsm so that it would reload sim card in tray
            /// after that +QPIN urc should come
            void hotswap_trigger();
            void sim_sel();
        } // namespace sim
    }     // namespace cellular
};        // namespace bsp

#endif // PUREPHONE_BSP_CELLULAR_HPP
