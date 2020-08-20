#/usr/bin/env python3

from FileReader import FileReader

why = '''
// This header is created to run commads through bt uart on start to initialize bt chip

'''

if __name__ == "__main__":
    '''
    This code creates header to be loaded in c++ code to get commands which needs to be send on 
    bluetooth initialization ( and responses )
    '''
    header = open("bt-header.hpp", "w")
    header.write("#pragma once")
    header.write(why)
    header.write("#include<list>\n\n")

    todo = FileReader()
    todo.open()

    out_lines = []
    in_lines = []

    while True:
        [out, send] = todo.from_file()
        if send == '' or send == []:
            print("done");
            break

        print("data: {}".format(send))

        if out == True:
            out_lines += [send]
        else:
            in_lines += [send]

    def to_write(what,val):
        ret = "std::list<std::list<unsigned char>> {} = {};\n".format(what, val)
        ret = ret.replace('[','{')
        ret = ret.replace(']','}')
        return ret


    header.write(to_write("bt_out",out_lines))
    header.write(to_write("bt_in",in_lines))
    header.close()
