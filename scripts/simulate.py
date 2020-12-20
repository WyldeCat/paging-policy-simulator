import argparse
import subprocess
import os
import time

from visualize import write_graph

parser = argparse.ArgumentParser(description = '')
parser.add_argument('--mem', default = 4096, type = int, help = 'memory size(KB)')
parser.add_argument('--num_buffer', default = 32, type = int, help = 'num buffer')
parser.add_argument('--num_interval', default = 200, type = int, help = 'num interval')
parser.add_argument('--size_buffer', default = 16, type = int, help = 'buffer size(M)')

parser.add_argument('--policy', default = 'LRU', type = str, choices=['FIFO', 'LRU', 'LFU', 'ARC', 'CLOCK_PRO'], help = 'paging policy')
parser.add_argument('--target', required=True, type = str, help = 'target program')

def main():
    args = parser.parse_args()

    _, program = os.path.split(args.target)
    tag = program + "_" + str(args.policy) + "_" + str(args.mem) + "KB"
    output = tag + ".png"
    csv_output = tag + ".csv"
    title = program + "-" + str(args.policy) + "-" + str(args.mem) + "KB"

    option = str(args.mem) + " " + str(args.policy) + " " + str(args.num_buffer) + " " + str(args.size_buffer) + " " + csv_output +  " " + str(args.num_interval)
    cmd = os.environ["PIN_ROOT"] + "/pin -t ../obj/pps.so " + option + " -- " + args.target

    proc = subprocess.Popen("exec " + cmd, cwd="./", shell=True)

    try:
        proc.wait()
    except:
        proc.kill()
        # TODO Fini() must be called

    write_graph(csv_output, output, title)

if __name__ == "__main__":
    main()
