from harness import log


def test_get_dom(harness):
    body = {"ui": True, "getWindow": True}
    result = harness.endpoint_request("developerMode", "get", body)
    log.info("data {}".format(result))
    assert result['body']
    assert 'Window' in result['body']['dom']
