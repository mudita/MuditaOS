#!/usr/bin/env python
# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

from harness.harness import Harness
from harness.api.developermode import SetEQ

import sys
import copy

harness = Harness.from_detect()

# pass EQ json file as an argument of this script. It can be both absolute or relative path.

file_path = sys.argv[1]
file_data = open(file_path,"r")
file_text = file_data.read()
file_data.close()

file_name = file_path.split('/')[-1]
print("Overriding "+file_name+" ...")
print("File data: \n"+file_text)
cmd = SetEQ(file_name,file_text)
ret = cmd.run(harness)
print("result: " + "OK" if ret.response.status == 200 else "Error!")
