#!/bin/python3
import gdb

class PrintTaskCrashed(gdb.Command):
    "Print task TCB contents"

    def __init__ (self):
        super (PrintTaskCrashed, self).__init__ ("printTask",
                             gdb.COMMAND_SUPPORT,
                             gdb.COMPLETE_NONE, True)

    def invoke (self, arg, from_tty):
        print("Current task address:")
        val = gdb.parse_and_eval("pxCurrentTCB")
        refval = val.dereference()
        # print(refval)
        print("    Current task name: {}".format(refval["pcTaskName"].string()))
        stackTop = int(refval["pxTopOfStack"])
        stackNow = int(refval["pxEndOfStack"])
        stackEnd= int(refval["pxStack"])
        print("    stack: 0x{:02X}    0x{:02X}    0x{:02X}".format(stackTop, stackEnd, stackNow))
        print("    stack use: {} of: {}".format((stackNow - stackTop), (stackTop - stackEnd)))

        if arg == "dump":
            print("Stack dump: ")
            for el in range(stackTop, stackNow):
                try:
                    # print("    0x{:02X}".format(el))
                    memory_content = gdb.execute("x 0x{:02X}".format(el), from_tty, True).split('\t')[1][:-1]
                    # print(memory_content)
                    intval = int(memory_content,16)
                    if intval  > 0x8000000 and intval < 0x8D000000:
                        print("    {}".format(gdb.execute("x {}".format(memory_content), from_tty, True)[:-1]))
                except (Exception,gdb.MemoryError) as ex:
                    pass

PrintTaskCrashed()
