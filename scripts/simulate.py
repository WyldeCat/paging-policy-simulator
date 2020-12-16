
import os
import argparse
import subprocess

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
        os.exit(1)

    # TODO visualize results

if __name__ == "__main__":
    main()
