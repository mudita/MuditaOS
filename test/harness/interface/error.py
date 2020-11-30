# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md

from enum import IntEnum


class Error(IntEnum):
    PORT_NOT_FOUND = 1,
    PORT_FILE_NOT_FOUND = 2,
    TEST_FAILED = 3,
    VERSION_MISMATCH = 4,
    OTHER_ERROR = 5


class TestError(Exception):

    def __init__(self, error_code: Error):
        self.error_code = error_code
        self.message = f"Test failed with error code: {error_code.name}"
        super().__init__(self.message)

    def __str__(self):
        return self.message

    def get_error_code(self):
        return int(self.error_code)

