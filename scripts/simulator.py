
from page_policy import PagePolicy
from fifo import FIFO
from lru import LRU
from lfu import LFU

import socket
import sys
import os
import argparse
import subprocess

parser = argparse.ArgumentParser(description = '')
parser.add_argument('--mem', default = 4096, type = int, help = 'memory size(MB)')
parser.add_argument('--policy', default = 'FIFO', type = str, choices=['FIFO', 'LRU', 'LFU'], help = 'paging policy')
parser.add_argument('--target', required=True, type = str, help = 'target program')
parser.add_argument('--trace_out', type = str, help = 'record traces if this arg is given')

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
    memory_size = memory_size * 1024 # KB

    if args.policy == 'FIFO':
        policy = FIFO(memory_size)
    elif args.policy == 'LRU':
        policy = LRU(memory_size)
    elif args.policy == 'LFU':
        policy = LFU(memory_size)

    target = args.target

    trace_out = None
    if args.trace_out:
        trace_out = open(args.trace_out, 'w')

    if os.path.exists("./socket"):
        os.remove("./socket")

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
            policy.add_memtrace(MemRef(ip, type, addr))

            if trace_out != None:
                record = str(type) + ", " + hex(addr) + "\n"
                trace_out.write(record)

    except ConnectionClose:
        print("ConnectionClosed!")
        policy.print_result()
        # TODO print result

    if trace_out != None:
        trace_out.close()

    os.remove("./socket")

if __name__ == "__main__":
    main()
