
class FileReader():
    def open(self):
        '''
        opens file to read and parse, open as long as class exists
        '''
        self.f = open("dump-bt.txt");

    def from_file(self):
        '''
        reads from file opened with open and returns 
        [ if is `out` line (pc to bt), line send]
        '''
        res = False;
        line = self.f.readline()
        if "out:" in line:
            res = True
        line = line.replace("in:  ",'')
        line = line.replace("out: ",'')
        line = line.replace('\n','')
        line = [ int(a,16) for a in filter(None,line.split(" ")) ]
        return [ res, line ]

    def lol(self):
        '''
        debug - read all lines in file and see if these are nice
        '''
        while True:
            [ out , line ] = self.from_file()
            if len(line) > 0:
                if out: 
                    print("> {}".format(line))
                else:
                    print("< {}".format(line))
            else:
                return

    def __del__(self):
        self.f.close();


if __name__ == "__main__":
    lol = FileReader()
    lol.open()
    lol.lol()
