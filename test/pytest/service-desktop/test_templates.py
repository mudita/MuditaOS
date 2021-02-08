# Copyright (c) 2017-2021, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
from harness.interface.defs import status


@pytest.mark.service_desktop_test
def test_templates(harness):
    # getting the templates count
    body = {"category": "template", "count": True}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]
    count = ret["body"]["count"]

    if count == 0:
        body = {"category": "template", "templateBody": "first template"}
        ret = harness.endpoint_request("messages", "post", body)
        assert ret["status"] == status["OK"]

        body = {"category": "template", "count": True}
        ret = harness.endpoint_request("messages", "get", body)
        assert ret["status"] == status["OK"]
        count = ret["body"]["count"]
        assert count

    # getting all templates
    body = {"category": "template", "limit": count}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]
    assert len(ret["body"][1][1]) == count

    # getting a number of templates
    number_of_requested_templates = 3
    body = {"category": "template", "limit": number_of_requested_templates, "offset": 0}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]

    templates = ret["body"][1][1]  # getting entries
    templates_count = len(templates)
    assert templates_count == number_of_requested_templates

    # getting template by ID
    body = {"category": "template", "templateID": 1}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]

    # adding new template
    body = {"category": "template", "templateBody": "test template"}
    ret = harness.endpoint_request("messages", "post", body)
    assert ret["status"] == status["OK"]

    # getting the templates count again
    body = {"category": "template", "count": True}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]
    assert ret["body"]["count"] == count + 1

    # getting template to remove
    body = {"category": "template", "limit": count + 1}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]

    templateid = 0
    for template in ret["body"][1][1]:
        if template["templateBody"] == "test template":
            templateid = template["templateID"]
            break

    # removing template
    body = {"category": "template", "templateID": templateid}
    ret = harness.endpoint_request("messages", "del", body)
    assert ret["status"] == status["OK"]

    # getting the templates count again
    body = {"category": "template", "count": True}
    ret = harness.endpoint_request("messages", "get", body)
    assert ret["status"] == status["OK"]
    assert ret["body"]["count"] == count
