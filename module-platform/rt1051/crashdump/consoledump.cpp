// Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
// For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

#include <log.hpp>

#include "consoledump.hpp"
#include "MIMXRT1051.h"

/// Hard Fault Fault Status Register
struct __attribute__((packed)) hfsr_t
{
    union
    {
        uint32_t all;
        struct
        {
            uint32_t rsrvd0 : 1;
            uint32_t vecttbl : 1;
            uint32_t rsrvd1 : 28;
            uint32_t forced : 1;
            uint32_t debugevt : 1;
        };
    };
};

/// MemManage Fault Status Register
struct __attribute__((packed)) mmfsr_t
{
    union
    {
        uint8_t all;
        struct
        {
            uint8_t iaccviol : 1;
            uint8_t daccviol : 1;
            uint8_t rsrvd2 : 1;
            uint8_t munstkerr : 1;
            uint8_t mstkerr : 1;
            uint8_t mlsperr : 1;
            uint8_t rsrvd6 : 1;
            uint8_t mmarvalid : 1;
        };
    };
};

/// BusFault Status Register
struct __attribute__((packed)) bfsr_t
{
    union
    {
        uint8_t all;
        struct
        {
            uint8_t ibuserr : 1;
            uint8_t preciserr : 1;
            uint8_t impreciserr : 1;
            uint8_t unstkerr : 1;
            uint8_t stkerr : 1;
            uint8_t lsperr : 1;
            uint8_t rsrvd6 : 1;
            uint8_t bfarvalid : 1;
        };
    };
};

/// UsageFault Status Register
struct __attribute__((packed)) ufsr_t
{
    union
    {
        uint16_t all;
        struct
        {
            uint8_t undefinstr : 1;
            uint8_t invstate : 1;
            uint8_t invpc : 1;
            uint8_t nocp : 1;
            uint8_t rsrvd4_7 : 3;
            uint8_t unaligned : 1;
            uint8_t divbyzero : 1;
            uint8_t rsrvd10_15 : 6;
        };
    };
};

/// Configurable Fault Status Register
struct __attribute__((packed)) cfsr_t
{
    union
    {
        uint32_t all;
        struct
        {
            mmfsr_t mmfsr;
            bfsr_t bfsr;
            ufsr_t ufsr;
        };
    };
};

/// Auxiliary Bus Fault Status register
struct __attribute__((packed)) abfsr_t
{
    union
    {
        uint32_t all;
        struct
        {
            uint32_t itcm : 1;
            uint32_t dtcm : 1;
            uint32_t ahbp : 1;
            uint32_t axim : 1;
            uint32_t eppb : 1;
            uint32_t rsrvd5_7 : 3;
            uint32_t aximtype : 2;
            uint32_t rsrvd10_31 : 1;
        };
    };
};

struct __attribute__((packed)) syslog_t
{
    CrashCatcherStackedRegisters stackFrame;
    struct
    {
        hfsr_t hfsr;         /// Hard Fault Fault Status Register
        cfsr_t cfsr;         /// Configurable Fault Status Register
        uint32_t mmfar;      /// MemManage Fault Address Register
        uint32_t bfar;       /// BusFault Address Register
        abfsr_t abfsr;       /// Auxiliary Bus Fault Status register
        uint32_t exc_return; /// EXC_RETURN value for the exception being serviced
    } registers;
};

namespace
{
    void printCrashInfo(const syslog_t &syslog)
    {
        LOG_FATAL("!!! Crash Occured !!!");
        LOG_FATAL("* Stack Frame:");
        LOG_FATAL("R0  = 0x%0" PRIX32, syslog.stackFrame.r0);
        LOG_FATAL("R1  = 0x%0" PRIX32, syslog.stackFrame.r1);
        LOG_FATAL("R2  = 0x%0" PRIX32, syslog.stackFrame.r2);
        LOG_FATAL("R3  = 0x%0" PRIX32, syslog.stackFrame.r3);
        LOG_FATAL("R12 = 0x%0" PRIX32, syslog.stackFrame.r12);
        LOG_FATAL("LR  = 0x%0" PRIX32, syslog.stackFrame.lr);
        LOG_FATAL("PC  = 0x%0" PRIX32, syslog.stackFrame.pc);
        LOG_FATAL("PSR = 0x%0" PRIX32, syslog.stackFrame.psr);

        LOG_FATAL("* Fault Status Registers:");
        LOG_FATAL("SCB->HFSR = 0x%0" PRIX32, syslog.registers.hfsr.all);
        LOG_FATAL("SCB->CFSR = 0x%0" PRIX32, syslog.registers.cfsr.all);
        LOG_FATAL("SCB->MMAR = 0x%0" PRIX32, syslog.registers.mmfar);
        LOG_FATAL("SCB->BFAR = 0x%0" PRIX32, syslog.registers.bfar);
        LOG_FATAL("SCB->ABFSR = 0x%0" PRIX32, syslog.registers.abfsr.all);
        LOG_FATAL("EXC_RETURN = 0x%0" PRIX32, syslog.registers.exc_return);

        LOG_FATAL("* Details of Fault Status Registers:");
        hfsr_t hfsr = syslog.registers.hfsr;
        LOG_FATAL("SCB->HFSR = 0x%0" PRIX32 " - HardFault Status Register", hfsr.all);
        if (hfsr.debugevt)
            LOG_FATAL(" - DEBUGEVT: Hard fault caused by debug event");
        if (hfsr.forced)
            LOG_FATAL(" - FORCED: Hard fault caused by bus/memory management/usage fault");
        if (hfsr.vecttbl)
            LOG_FATAL(" - VECTBL: BusFault on vector table read");

        cfsr_t cfsr = syslog.registers.cfsr;
        LOG_FATAL("SCB->CFSR = 0x%0" PRIX32 " - Configurable Fault Status Register", cfsr.all);
        mmfsr_t mmfsr = cfsr.mmfsr;
        LOG_FATAL("> SCB->MMFSR = 0x%0" PRIX8 " Memory Management Fault Status Register", mmfsr.all);
        if (mmfsr.mmarvalid)
            LOG_FATAL(" - MMARVALID: MMAR is valid");
        if (mmfsr.mlsperr)
            LOG_FATAL(" - MLSPERR: A MemManage fault occurred during floating-point lazy state preservation");
        if (mmfsr.mstkerr)
            LOG_FATAL(" - MSTKERR: MemManage fault on stacking for exception entry");
        if (mmfsr.munstkerr)
            LOG_FATAL(" - MUNSTKERR: MemManage fault on unstacking for a return from exception:");
        if (mmfsr.daccviol)
            LOG_FATAL(" - DACCVIOL: Data access violation");
        if (mmfsr.iaccviol)
            LOG_FATAL(" - IACCVIOL: Instruction access violation");

        bfsr_t bfsr = cfsr.bfsr;
        LOG_FATAL("> SCB->BFSR = 0x%0" PRIX8 " - Bus Fault Status Register ", bfsr.all);
        if (bfsr.bfarvalid)
            LOG_FATAL(" - BFARVALID: BFAR is valid  ");
        if (bfsr.lsperr)
            LOG_FATAL(" - LSPERR: A bus fault occurred during floating-point lazy state preservation");
        if (bfsr.stkerr)
            LOG_FATAL(" - STKERR: Stacking error");
        if (bfsr.unstkerr)
            LOG_FATAL(" - UNSTKERR: Unstacking error");
        if (bfsr.impreciserr)
            LOG_FATAL(" - IMPREISERR: Imprecise data bus error");
        if (bfsr.preciserr)
            LOG_FATAL(" - PRECISERR: Precise data bus error");
        if (bfsr.ibuserr)
            LOG_FATAL(" - IBUSERR: Instruction bus error");

        ufsr_t ufsr = cfsr.ufsr;
        LOG_FATAL("> SCB->UFSR = 0x%0" PRIX16 " - Usage Fault Status Register", ufsr.all);
        if (ufsr.divbyzero)
            LOG_FATAL(" - DIVBYZERO: Divide by zero UsageFault");
        if (ufsr.unaligned)
            LOG_FATAL(" - UNALIGNED: Unaligned access UsageFault");
        if (ufsr.nocp)
            LOG_FATAL(" - NOCP: Attempt to execute a coprocessor instruction");
        if (ufsr.invpc)
            LOG_FATAL(" - INVPC: Invalid PC load UsageFault");
        if (ufsr.invstate)
            LOG_FATAL(" - INVSTATE: Invalid state UsageFault");
        if (ufsr.undefinstr)
            LOG_FATAL(" - UNDEFINSTR: Undefined instruction UsageFault");

        abfsr_t abfsr = syslog.registers.abfsr;
        LOG_FATAL("SCB->ABFSR = 0x%0" PRIX32 " - Auxiliary Bus Fault Status register", abfsr.all);
        if (abfsr.itcm)
            LOG_FATAL(" - ITCM: Asynchronous fault on ITCM interface");
        if (abfsr.dtcm)
            LOG_FATAL(" - DTCM: Asynchronous fault on DTCM interface.");
        if (abfsr.ahbp)
            LOG_FATAL(" - AHBP: Asynchronous fault on AHBP interface");
        if (abfsr.eppb)
            LOG_FATAL(" - EPPB: Asynchronous fault on EPPB interface");
    }

    syslog_t syslogFrom(const CrashCatcherInfo *info)
    {
        syslog_t syslog;

        syslog.stackFrame          = *info->pSP;
        syslog.registers.hfsr.all  = SCB->HFSR;
        syslog.registers.cfsr.all  = SCB->CFSR;
        syslog.registers.mmfar     = SCB->MMFAR;
        syslog.registers.bfar      = SCB->BFAR;
        syslog.registers.abfsr.all = SCB->ABFSR;

        return syslog;
    }
} // namespace

namespace crashdump
{
    void printCrashInfo(const CrashCatcherInfo *info)
    {
        ::printCrashInfo(syslogFrom(info));
    }
} // namespace crashdump
