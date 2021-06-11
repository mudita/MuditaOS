# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
import time
from harness.interface.defs import status
from harness import log


@pytest.mark.service_desktop_test
@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.restore
def test_restore(harness):
    # this requests the list of available files
    body = { "request": True }
    response = harness.endpoint_request("restore", "get", body)
    
    # should be ok 200
    assert response["status"] == status["OK"]
    log.debug("check if body is an array")
    
    # body should be an array of files
    assert isinstance(response["body"], list) == True
    
    # chose the first entry for test if array is > 0
    if len(response["body"]) > 0:
        restore_task = response["body"][0]
        log.debug("there are possible backup files on target, test restore %s" % (restore_task))
        # this starts a restore process with a file as parameter
        body = { "request":True, "location": restore_task }
        response = harness.endpoint_request("restore", "post", body)
        
        # we can't really test for results here, as the phone should reset
        # in case the restore process lasts longer we should be able to poll
        # but only on rt1051
        assert response["body"]["location"] != ""
        assert response["body"]["state"] != ""
    else:
        log.error("no possisble backup files on phone, run backup first")
        assert False
