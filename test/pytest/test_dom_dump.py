import pytest
from harness import log

@pytest.mark.skip("not working ;/")
def test_get_dom(harness):
    body = {"ui": True, "getWindow": True}
    result = harness.endpoint_request("developerMode", "get", body)
    log.info("data {}".format(result))
    assert result['body']
    assert 'Window' in result['body']['dom']
