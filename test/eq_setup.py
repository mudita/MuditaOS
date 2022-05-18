#!/usr/bin/env python
# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

from harness.harness import Harness
from harness.interface.defs import Endpoint, Method

import sys

harness = Harness.from_detect()

# pass EQ json file as an argument of this script. It can be both absolute or relative path.

file_path = sys.argv[1]
file_data = open(file_path,"r")
file_text = file_data.read()
file_data.close()

file_name = file_path.split('/')[-1]
print("Overriding "+file_name+" ...")
print("File data: \n"+file_text)
ret = harness.endpoint_request("developerMode", "put", {"EQ": "test", "fileName": file_name, "fileData":file_text})
result = ret["status"]
print("result: " + "OK" if result == 200 else "Error!")
