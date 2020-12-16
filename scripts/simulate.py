import argparse
import subprocess
import os

from visualize import write_graph

parser = argparse.ArgumentParser(description = '')
parser.add_argument('--mem', default = 4096, type = int, help = 'memory size(MB)')
parser.add_argument('--num_buffer', default = 2, type = int, help = 'num buffer')
parser.add_argument('--size_buffer', default = 128, type = int, help = 'buffer size(M)')

parser.add_argument('--policy', default = 'LRU', type = str, choices=['FIFO', 'LRU', 'LFU'], help = 'paging policy')
parser.add_argument('--target', required=True, type = str, help = 'target program')

def main():
    args = parser.parse_args()

    option = str(args.mem) + " " + str(args.policy) + " " + str(args.num_buffer) + " " + str(args.size_buffer)
    cmd = os.environ["PIN_ROOT"] + "/pin -t ../obj/pps.so " + option + " -- " + args.target

    proc = subprocess.Popen("exec " + cmd, cwd="./", shell=True)

    try:
        proc.wait()
    except:
        proc.kill()
        # TODO Fini() must be called

    _, program = os.path.split(args.target)
    output = program + "_" + str(args.policy) + "_" + str(args.mem) + "MB" + ".png"
    title = program + "-" + str(args.policy) + "-" + str(args.mem) + "MB"

    write_graph("out.csv", output, title)

if __name__ == "__main__":
    main()
