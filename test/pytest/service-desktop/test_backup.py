# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
import time
import os
from harness import log
from harness.interface.defs import status


@pytest.mark.service_desktop_test
@pytest.mark.rt1051
@pytest.mark.usefixtures("phone_unlocked")
@pytest.mark.backup
def test_backup(harness):
    body = { "request": True }
    log.debug("backup testing");
    
    # this requests a backup start
    response = harness.endpoint_request("backup", "get", body)
    assert response["body"] != ""
    assert response["body"]["task"] != ""
    task = response["body"]["task"]
    assert response["status"] == status["OK"]
    
    # in response we get a task ID and status 200
    log.debug("backup started, waiting for results")
    
    # start polling for backup status and wait for it to end
    i = 0
    while True:
        i = i+1
        time.sleep(1)  # wait for the endpoint to be ready

        # now that we know the task ID we can poll for it's status
        body = { "request": True, "task": task }
        response = harness.endpoint_request("backup", "get", body)
        
        # backup is still running
        if response["body"]["state"] == "running":
            assert response["status"] == 200
            log.debug("backup is running...")
            
        # backup has stopped, should be OK and finished, status is 303
        # and redirects to a location as per the documentation
        if response["body"]["state"] == "stopped":
            log.debug("backup ended, check results")
            assert response["status"] == 303
            assert response["body"]["location"] != ""
            
            # see if the location is a valid backup path, extensions is .tar
            # and starts with a /
            p = response["body"]["location"]
            p_split = os.path.splitext(p)
            assert p_split[1] == ".tar"
            assert p_split[0][0] == "/"
            
            break
            
        # wait for a moment
        log.debug("sleeping for 1 second")
        time.sleep(1)
        
        # max 30 second timeout
        if i > 30:
            # this is bas, and the test fails here
            log.error("backup timeout reached")
            assert False
            break
