
from page_policy import PagePolicy
from fifo import FIFO

import sys

# memory referece


class MemRef:
    def __init__(self, ip, op, addr):
        self.ip = ip
        self.op = op
        self.addr = addr

    def __str__(self):
        return "ip : " + self.ip + ", op : " + self.op + ", addr : " + self.addr + "\n"


def read_memtrace(filename):
    with open(filename, 'r') as f:
        lines = f.readlines()
        # delete the last line : #eof
        lines.pop()

        for line in lines:
            line = line.rstrip("\n")
            line_data = line.split(" ")

            ip = line_data[0]
            ip = ip[:-1]  # delete ':'
            op = line_data[1]
            addr = line_data[2]

            memref = MemRef(ip, op, addr)

            # send memory reference to simulator
            fifo.add_memtrace(memref)


# check args
if len(sys.argv) != 2:
    print("Please check the usage...... python ./simulator.py [pinatrace.out]")
    sys.exit(1)


memory_size = input("input memory size (GB): ")
memory_size = memory_size*1024*1024; # KB

# FIFO simulator
fifo = FIFO(memory_size)

# read file
read_memtrace(sys.argv[1])

# print FIFO simulator results
fifo.print_result()
