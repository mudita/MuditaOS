#ifndef PUREPHONE_BSP_CELLULAR_HPP
#define PUREPHONE_BSP_CELLULAR_HPP

#include <optional>
#include <stdint.h>
#include <stddef.h>
#include <memory>
#include <FreeRTOS.h>
#include <FreeRTOS/include/queue.h>
#include "drivers/lpuart/DriverLPUART.hpp"
#include <common_data/EventStore.hpp>

namespace bsp {
namespace cellular
    {
    	enum class antenna{
    		lowBand,
			highBand
    	};
    }
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

        virtual void SelectAntenna(bsp::cellular::antenna antenna) = 0;
        virtual bsp::cellular::antenna GetAntenna() = 0;

        [[nodiscard]] auto GetCellularDevice() const noexcept -> std::shared_ptr<devices::Device>;

    protected:
        bool isInitialized = false;
        std::shared_ptr<drivers::DriverLPUART> driverLPUART;
    };
    namespace cellular
        {
        enum IRQsource{
            statusPin,
            trayPin,
			ringIndicatorPin,
        };

        /// initialize SIM queue directed to EventWorker
        int init(xQueueHandle qHandle);

        namespace USB {
            enum class PassthroughState {
                ENABLED,
                DISABLED,
            };

            enum class BootPinState {
                FIRMWARE_UPGRADE,
                NORMAL_BOOT,
            };

            BootPinState getBootPin() ;
            PassthroughState getPassthrough() ;

            void setBootPin(BootPinState bootPin);
            void setPassthrough(PassthroughState pass);
        }

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

        namespace ringIndicator
		{
        	// handling incoming calls and sms - RI pin
        	BaseType_t riIRQ_handler();
		} // namespace RingIndicator

    }     // namespace cellular
};        // namespace bsp

#endif // PUREPHONE_BSP_CELLULAR_HPP
