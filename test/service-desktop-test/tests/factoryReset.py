from defs import *
from test import *


class FactoryResetTest:
    def run(self):
        msg, result_msg = prepare_message(endpoint["factory"], method["post"], status["OK"], {"factoryRequest": True},
                                          {'factoryRequest': True})
        test = Test(msg, result_msg)
        ret, result = test.execute()
        return ret
