# Copyright (c) 2017-2020, Mudita Sp. z.o.o. All rights reserved.
# For licensing, see https://github.com/mudita/MuditaOS/LICENSE.md
import sys
from tests.messages import *
from tests.templates import *
from tests.contacts import *
from tests.update import *
from tests.deviceinfo import *
from tests.factoryReset import *
from tests.backup import *
from tests.calllog import *
from tests.calendarEvents import *
from termcolor import colored

from harness.harness import Harness


def main():
    if len(sys.argv) == 1 or sys.argv[1] is None:
        print('Please pass port name as the parameter: python main.py /dev/ttyACM0 ')
        exit(1)

    test_harness = Harness(sys.argv[1])
    serial = test_harness.get_connection()
    final_result = True
    failed_tests = []
    for test_instance in (DeviceInfoTest(serial), calendarEventsTest(serial), UpdateTest(serial), BackupTest(serial), MessageTest(serial),
                          MessageTemplateTest(serial), ContactTest(serial), CalllogTest(serial),
                          FactoryResetTest(serial)):
        test_name = type(test_instance).__name__
        result = test_instance.run()
        if result == False:
            failed_tests.append(test_name)
        final_result = final_result and result
        print(
            f'[{test_name}] {colored("PASS", "green") if result else colored("FAIL", "red")}')

    print("\nFinal result:", colored("PASS", "green") if final_result else colored("FAIL", "red"))
    print("Failed tests:")
    for test in failed_tests:
        print(colored(test, "red"))


if __name__ == '__main__':
    main()
