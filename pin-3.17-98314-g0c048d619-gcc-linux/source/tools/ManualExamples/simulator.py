
import sys

# memory referece
class MemRef:
    def __init__(self, ip, op, addr):
        self.ip = ip
        self.op = op
        self.addr = addr

    def __str__(self):
        return "ip : " + self.ip + ", op : " + self.op + ", addr : " + self.addr + "\n"

'''
# list of memory references
class MemRefs:
    memrefs = []

    def append(self, memref):
        self.memrefs.append(memref);

    def __str__(self):
        string = ""
        for memref in self.memrefs:
            string += str(memref)
        return string
'''

class PagePolicy:
    """ Super Class"""

    max_memory_size = 4*1024*1024 #4GB
    page_entry_size = 4 # 4KB
    max_page_entry_count = max_memory_size/page_entry_size
    page_table = []
    memrefs = []
    hit_counter = 0
    miss_counter = 0
    
    def add_memtrace(self, memref):
        pass
        #self.page_table.append(memref)
        
    def get_hit_counter(self):
        return self.hit_counter

    def get_miss_counter(self):
        return self.miss_counter

    def print_info(self):
        print("-page table entry\n")
        for page_entry in self.page_table:
            print(page_entry)
        print("-hit counter : %d, miss counter : %d\n"%(self.hit_counter, self.miss_counter));


class FIFO(PagePolicy):
    """Sub class"""

    def add_memtrace(self, memref):
        pass


def read_memtrace(filename):
    with open(filename, 'r') as f:
        lines = f.readlines()
        # delete the last line : #eof
        lines.pop()

        for line in lines:
            line = line.rstrip("\n");
            line_data = line.split(" ");

            ip = line_data[0]
            ip = ip[:-1] # delete ':'
            op = line_data[1]
            addr = line_data[2]

            print(addr)
            """hex to binary"""
            ob_binary = bin(int(addr, 16))
            print(ob_binary)
            binary = ob_binary[2:]
            print(len(binary))

            memref = MemRef(ip, op, addr)
            fifo.add_memtrace(memref);
            break

        fifo.print_info()


# check args
if len(sys.argv) != 2:
    print("please check the usage...... python ./simulator.py [pinatrace.out]");
    sys.exit(1)



# list of memory references
#memrefs = MemRefs()

fifo = FIFO()

# read file
read_memtrace(sys.argv[1]);

#print(memrefs)




