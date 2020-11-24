#!/usr/bin/env python3
'''
Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

detect static globals in code

@package globalFinder
'''

import argparse
import magic
import os
import re
import subprocess
import sys

from enum import Enum
from queue import Queue
from threading import Thread

class ArchType(Enum):
    X86 = 1
    ARM = 2

class Finder():
    "find potential places where global can be"
    def __init__(self, path):
        '''path - path to elf'''
        self.elfPath=path
        self.arch = None
        self.armRe = re.compile("ELF 32-bit .*ARM.*")
        self.x86Re = re.compile("ELF (64|32)-bit .*x86.*")
        self.objdump = None
        self.nm = None
        self.staticSymbol = "_Z41__static_initialization_and_destruction_0ii"
        self.staticRe = re.compile("([0-9a-fA-F]+) ([0-9a-fA-F]+) .*" + self.staticSymbol + ".*")
        self.foundSections=[]
        self.queue = Queue()
        self.resultsQueue = Queue()
        self.results={}
    def checkArch(self):
        '''Detect elf architecture'''
        magicString = magic.from_file(self.elfPath)
        if self.x86Re.match(magicString):
            self.arch = ArchType.X86
            self.objdump = "objdump"
            self.nm = "nm"
        elif self.armRe.match(magicString):
            self.arch = ArchType.ARM
            self.objdump = "arm-none-eabi-objdump"
            self.nm = "arm-none-eabi-nm"
        else:
            self.arch = None
    def findStatic(self):
        '''find sections with static initialization symbol'''
        print("searching for statics")
        out = subprocess.run( [self.nm, "--print-size", self.elfPath], capture_output=True, text=True)
        lines = out.stdout.split('\n')
        for line in lines:
            match = self.staticRe.match(line)
            if match:
                startAddress = int(match.group(1), 16)
                sectionSize  = int(match.group(2), 16)
                endAddres = startAddress + sectionSize
                self.queue.put(("0x%x" % startAddress, "0x%x" % endAddres))
                self.foundSections.append(line)
    def countResults(self):
        '''count found items'''
        while not self.resultsQueue.empty():
            path, lineNo = self.resultsQueue.get()
            realpath = os.path.realpath(path)
            key=realpath+":"+lineNo
            if key not in self.results:
                self.results[key] = 1
            else:
                self.results[key] = self.results[key] + 1



class StaticChecker(Thread):

    def __init__(self, finder):
        Thread.__init__(self)
        self.finder = finder
        self.hppRegexp = re.compile("(.*\.hpp):([0-9]+)")

    def run(self):
        while True:
            print("-thread start:", self.native_id, self.finder.queue.qsize())
            if not self.finder.queue.empty():
                startAddres, stopAddress = self.finder.queue.get()
                try:
                    out = subprocess.run( [ self.finder.objdump, "--start-address="+startAddres, "--stop-address="+stopAddress, "-d", "-l", self.finder.elfPath], capture_output=True, text=True)
                    for line in out.stdout.split('\n'):
                        reMatch=self.hppRegexp.match(line)
                        if reMatch:
                            self.finder.resultsQueue.put((reMatch.group(1), reMatch.group(2)))
                            print(" ", reMatch.group(1), reMatch.group(2))
                finally:
                    self.finder.queue.task_done()
            else:
                return


def main():
    parser = argparse.ArgumentParser(description="Find globals in code")
    parser.add_argument('elfPath', help="Path to elf file" )
    args = parser.parse_args()
    finder = Finder(args.elfPath)
    finder.checkArch()
    finder.findStatic()
    cpuCount = os.cpu_count()
    if cpuCount is None:
        cpuCount=2
    for x in range(cpuCount):
        staticChecker = StaticChecker(finder)
        staticChecker.daemon=True
        staticChecker.start()
    finder.queue.join()
    finder.countResults()
    if len(finder.results) >= 1:
        print(finder.results)
        sys.exit(1)




if __name__ == "__main__":
    main()
