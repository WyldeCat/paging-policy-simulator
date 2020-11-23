
import sys

# memory referece
class MemRef:
    def __init__(self, ip, op, addr):
        self.ip = ip
        self.op = op
        self.addr = addr

    def __str__(self):
        return "ip : " + self.ip + ", op : " + self.op + ", addr : " + self.addr + "\n"

# list of memory references
class MemRefs:
    def __init__(self):
        self.memrefs = []

    def append(self, memref):
        self.memrefs.append(memref);

    def __str__(self):
        string = ""
        for memref in self.memrefs:
            string += str(memref)
        return string


def read_file(filename):
    with open(filename, 'r') as f:
        lines = f.readlines()
        # delete the last line : #eof
        lines.pop()

        for line in lines:
            line = line.rstrip("\n");
            line_data = line.split(" ");

            ip = line_data[0]
            ip = ip[:-1] # ':' 문자 제거
            op = line_data[1]
            addr = line_data[2]

            #print("ip : " + ip + ", op : " + op + ", addr : " + addr + '\n')
            
            memref = MemRef(ip, op, addr)
            memrefs.append(memref);



# check args
if len(sys.argv) != 2:
    print("please check the usage...... python ./simulator.py [pinatrace.out]");
    sys.exit(1)


# list of memory references
memrefs = MemRefs()

# read file
read_file(sys.argv[1]);

print(memrefs)




