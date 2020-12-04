
from page_policy import PagePolicy
from fifo import FIFO

import socket
import sys
import os
import argparse
import subprocess

parser = argparse.ArgumentParser(description = '')
parser.add_argument('--mem', default = 4096, type = int, help = 'memory size(MB)')
parser.add_argument('--target', required=True, type = str, help = 'target program')
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


def main():
    args = parser.parse_args()

    memory_size = args.mem
    memory_size = memory_size * 1024; # KB

    target = args.target

    sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    sock.bind("./socket")
    sock.listen(1)

    print(os.environ["PIN_ROOT"])
    subprocess.run([os.environ["PIN_ROOT"]+"/pin", "-t", "../obj/pinatrace.so","--","/bin/ls"], cwd="./")

    print("waiting connection..!")
    conn, client = sock.accept()
    print("connected!")
    while True:
        n = conn.recv(4)
        if len(n) == 0:
            break

    os.remove("./socket")


if __name__ == "__main__":
    main()
