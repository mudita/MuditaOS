# Copyright (c) 2017-2022, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import pytest
from harness.interface.defs import status


class TemplatesTester:
    def __init__(self, harness):
        self.harness = harness
        self.template_body = "TEST TEMPLATE BODY"
        self.templates_page_size = 4

    def __get_count(self):
        body = {"category": "template", "count": True}
        ret = self.harness.endpoint_request("messages", "get", body)
        assert ret["status"] == status["OK"]
        return ret["body"]["count"]

    def __add_template(self):
        body = {"category": "template", "templateBody": self.template_body}
        ret = self.harness.endpoint_request("messages", "post", body)
        assert ret["status"] == status["OK"]
        assert type(ret["body"]["templateID"]) == int
        return ret

    def __get_templates(self, limit, offset):
        body = {"category": "template", "limit": limit, "offset": offset}
        return self.harness.endpoint_request("messages", "get", body)

    def __remove_template(self, template_id):
        body = {"category": "template", "templateID": template_id}
        ret = self.harness.endpoint_request("messages", "del", body)
        assert ret["status"] == status["NoContent"]

    def __get_all_templates(self):
        body = {"category": "template", "count": True}
        ret = self.harness.endpoint_request("messages", "get", body)

        assert ret["status"] == status["OK"]

        total_count = ret["body"]["count"]
        offset = 0

        templates = []

        while True:
            ret = self.__get_templates(total_count, offset)
            templates += ret["body"]["entries"]

            assert ret["status"] == status["OK"]
            assert ret["body"]["totalCount"] == total_count

            if "nextPage" in ret["body"]:

                assert "limit" in ret["body"]["nextPage"]

                assert "offset" in ret["body"]["nextPage"]
                offset = ret["body"]["nextPage"]["offset"]

            else:
                break

        return templates

    def __remove_added_templates(self, templates):
        for template in templates:
            if template["templateBody"] == self.template_body:
                self.__remove_template(template["templateID"])

    def test_getting_template_by_id(self):
        template_id = 1
        body = {"category": "template", "templateID": template_id}
        ret = self.harness.endpoint_request("messages", "get", body)

        assert ret["status"] == status["OK"]
        assert ret["body"]["templateID"] == template_id

    def test_getting_templates_without_pagination(self):
        count = self.__get_count()

        number_of_requested_templates = self.templates_page_size - 1
        ret = self.__get_templates(number_of_requested_templates, 0)

        assert ret["status"] == status["OK"]
        assert ret["body"]["totalCount"] == count
        assert len(ret["body"]["entries"]) == number_of_requested_templates
        for template in ret["body"]["entries"]:
            assert type(template["lastUsedAt"]) == int
            assert type(template["templateBody"]) == str
            assert type(template["templateID"]) == int

    def test_changing_template_body(self):
        test_passed = False
        initial_count = self.__get_count()

        self.__add_template()

        templates = self.__get_all_templates()

        for template in templates:

            if template["templateBody"] == self.template_body:

                # Change template
                new_template_body = "NEW TEMPLATE BODY TEST"
                body = {"category": "template", "templateID": template["templateID"], "templateBody": new_template_body}
                ret = self.harness.endpoint_request("messages", "put", body)
                assert ret["status"] == status["NoContent"]

                # and then remove it to clean env
                self.__remove_template(template["templateID"])
                test_passed = True
                break

        # templates count at the end of the test should match initial templates count
        total_count = self.__get_count()
        assert total_count == initial_count
        assert test_passed == True

    def test_getting_templates_with_pagination(self):
        initial_count = self.__get_count()

        self.__add_template()

        total_count = self.__get_count()
        assert total_count == initial_count + 1

        # adding new templates to make the total count be above templates page size
        expected_templates_count = self.templates_page_size * 3 + 1
        num_of_templates_to_insert = expected_templates_count - total_count

        while 0 < num_of_templates_to_insert:
            self.__add_template()
            num_of_templates_to_insert -= 1

        total_count = self.__get_count()
        assert total_count == expected_templates_count

        templates = self.__get_all_templates()

        self.__remove_added_templates(templates)

        # templates count at the end of the test should match initial templates count
        total_count = self.__get_count()
        assert total_count == initial_count


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_get_template_by_id(harness):
    templates_tester = TemplatesTester(harness)
    templates_tester.test_getting_template_by_id()


@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_get_templates_without_pagination(harness):
    templates_tester = TemplatesTester(harness)
    templates_tester.test_getting_templates_without_pagination()

@pytest.mark.rt1051
@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_change_template(harness):
    templates_tester = TemplatesTester(harness)
    templates_tester.test_changing_template_body()

@pytest.mark.rt1051
@pytest.mark.service_desktop_test
@pytest.mark.usefixtures("phone_unlocked")
def test_get_templates_with_pagination(harness):
    templates_tester = TemplatesTester(harness)
    templates_tester.test_getting_templates_with_pagination()
