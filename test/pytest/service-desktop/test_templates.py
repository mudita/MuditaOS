# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
from harness.interface.defs import status


@pytest.mark.service_desktop_test
def test_messages(harness):
    # getting the templates count
    body = {"template": True, "count": True}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]
    count = ret["body"]["count"]

    if count == 0:
        body = {"template": True, "text": "first template"}
        ret = harness.endpoint_request("messages", "put", body)
        assert ret["status"] == status["OK"]

        body = {"template": True, "count": True}
        ret = harness.endpoint_request("messages", "get", body)
        assert ret["status"] == status["OK"]
        count = ret["body"]["count"]
        assert count

    # getting all templates
    body = {"template": True, "count": count}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]
    assert len(ret["body"]) == count

    # adding new template
    body = {"template": True, "text": "test template"}
    ret = harness.endpoint_request("messages", "put", body)
    assert ret["status"] == status["OK"]

    # getting the templates count again
    body = {"template": True, "count": True}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]
    assert ret["body"]["count"] == count + 1

    # getting template to remove
    body = {"template": True, "count": count + 1}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]

    for template in ret["body"]:
        if template["text"] == "test template":
            id = template["id"]
            break

    # removing template
    body = {"template": True, "id": id}
    ret = harness.endpoint_request("messages", "del", body)
    assert ret["status"] == status["OK"]

    # getting the templates count again
    body = {"template": True, "count": True}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]
    assert ret["body"]["count"] == count
