
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

class ConnectionClose(Exception): pass

def main():
    args = parser.parse_args()

    memory_size = args.mem
    memory_size = memory_size * 1024; # KB
    fifo = FIFO(memory_size)

    target = args.target

    sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    sock.bind("./socket")
    sock.listen(1)

    subprocess.Popen([os.environ["PIN_ROOT"] + "/pin", "-t",
        "../obj/pinatrace.so", "--", target], cwd="./")

    conn, _ = sock.accept()

    try:
        while True:
            def recv_long():
                ret = conn.recv(8)
                if len(ret) == 0:
                    raise ConnectionClose
                return int.from_bytes(ret, byteorder='little')

            type = recv_long()
            ip = recv_long()
            addr = recv_long()
            fifo.add_memtrace(MemRef(ip, type, addr))

    except ConnectionClose:
        print("ConnectionClosed!")
        fifo.print_result()
        # TODO print result

    os.remove("./socket")

if __name__ == "__main__":
    main()
