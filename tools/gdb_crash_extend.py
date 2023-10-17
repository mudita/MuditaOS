# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

from __future__ import print_function

from gdb.printing import PrettyPrinter, register_pretty_printer
import gdb
import uuid

class MMFSR_bits:
    IACCVIOL = 0
    DACCVIOL = 1
    MUNSTKERR = 3
    MSTKERR = 4
    MMARVALID = 7

class CrashDump(gdb.Command):
    """Prints the ListNode from our example in a nice format!"""

    def __init__(self):
        super(CrashDump, self).__init__(
            "crash_dump", gdb.COMMAND_USER
        )

    def _crash_dump(self, val=None):
        """
        """
        value=gdb.parse_and_eval("*0xE000ED28");
        # print(type(value), gdb.parse_and_eval(value))
        print(value)
        if value & 1 << MMFSR_bits.MMARVALID:
            print("MMAR holds proper address")
        if value & 1 << MMFSR_bits.MSTKERR:
            print("stacking for an exception entry has caused one or more access violations.")
        if value & 1 << MMFSR_bits.MUNSTKERR:
            print("unstack for an exception return has caused one or more access violations.")
        if value & 1 << MMFSR_bits.DACCVIOL:
            print("the processor attempted a load or store at a location that does not permit the operation.")
        if value & 1 << MMFSR_bits.IACCVIOL:
            print("the processor attempted an instruction fetch from a location that does not permit execution.")

        # TODO check psp vs msp by lr young bits
        psp=gdb.parse_and_eval("$psp")
        frame=gdb.parse_and_eval("*(syslog_exception_stack_frame_t*)$psp")
        print("Privileged Stack Pointer: {}".format(frame))

        if frame["psr"] & (1<<9):
            print("On exception entry CPU aligns new SP to 8 bytes so sometimes it needs to push additional 4 bytes to make it happen.")
            psp+=4
        else:
            print("normal stack pointer")

        gdb.execute("set $r0={}".format(frame["r0"]))
        gdb.execute("set $r1={}".format(frame["r1"]))
        gdb.execute("set $r2={}".format(frame["r2"]))
        gdb.execute("set $r3={}".format(frame["r3"]))
        gdb.execute("set $r12={}".format(frame["r12"]))
        gdb.execute("set $lr={}".format(frame["lr"]))
        gdb.execute("set $pc={}".format(frame["pc"]))
        gdb.execute("set $psr={}".format(frame["psr"]))

        gdb.execute("set $sp={}".format(psp))

        gdb.execute("bt")
        return val

    def complete(self, text, word):
        # We expect the argument passed to be a symbol so fallback to the
        # internal tab-completion handler for symbols
        return gdb.COMPLETE_SYMBOL

    def invoke(self, args, from_tty):
        print("Args Passed: %s" % args)
        print(self._crash_dump())

CrashDump()
