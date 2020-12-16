import argparse
import subprocess
import os

from visualize import write_graph

parser = argparse.ArgumentParser(description = '')
parser.add_argument('--mem', default = 4096, type = int, help = 'memory size(MB)')
parser.add_argument('--policy', default = 'LRU', type = str, choices=['FIFO', 'LRU', 'LFU'], help = 'paging policy')
parser.add_argument('--target', required=True, type = str, help = 'target program')

def main():
    args = parser.parse_args()

    cmd = os.environ["PIN_ROOT"] + "/pin -t ../obj/pps.so " + str(args.mem) + " " + str(args.policy) + " -- " + args.target
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
