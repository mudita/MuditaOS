# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

import time
from harness import log
from harness.interface.defs import key_codes

def get_calllog_count(harness):
    body = {"count": True}
    return harness.endpoint_request("calllog", "get", body)["body"]["count"]

def select_call_button(harness, call_duration):
    # call
    log.info("Call duration = {}s, calling...".format(call_duration))
    harness.connection.send_key_code(key_codes["fnLeft"])
    time.sleep(call_duration)
    # hang up
    log.info("Hanging up...")
    harness.connection.send_key_code(key_codes["fnRight"])
