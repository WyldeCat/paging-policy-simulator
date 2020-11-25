
import sys

# memory referece
class MemRef:
    def __init__(self, ip, op, addr):
        self.ip = ip
        self.op = op
        self.addr = addr

    def __str__(self):
        return "ip : " + self.ip + ", op : " + self.op + ", addr : " + self.addr + "\n"

class PagePolicy:
    """ Super Class"""

    max_memory_size = 4*1024*1024 #4GB
    page_entry_size = 4 # 4KB
    max_page_entry_count = max_memory_size/page_entry_size
    cache = []
    hit_counter = 0
    miss_counter = 0

    def get_page_number(self, memref):
        """hex to binary"""
        addr = int(memref.addr, 16) # 48 bit
        #print(bin(addr))
        addr = addr >> 12 # block offet 12 bit cut
        addr = bin(addr)
        #print(addr)  
        page_number = int(addr, 2) # 20bit = page_number
        #print(page_number)
        return page_number

    def add_memtrace(self, memref):
        pass
        
    def get_hit_counter(self):
        return self.hit_counter

    def get_miss_counter(self):
        return self.miss_counter

    def print_result(self):
        print("-paging simulator result\n")
        for ref in self.cache:
            print(ref)
        print("-hit counter : %d, miss counter : %d\n"%(self.hit_counter, self.miss_counter));


class FIFO(PagePolicy):
    """Sub class"""

    def add_memtrace(self, memref):
        page_number = self.get_page_number(memref)

        # first reference page
        if not page_number in self.cache:
            self.miss_counter += 1
            #if len(cache) < max_page_entry_count: # there is space
            if len(self.cache) < self.max_page_entry_count: # there is space
                self.cache.append(page_number) # append ref at end of the list
            else: # full
                print("cache full")
                self.cache.pop(0) # evict first element
                self.cache.append(page_number) # append at end of the list
        else: # page in the list
            self.hit_counter += 1
            self.cache.pop(self.cache.index(page_number)) # pop the old one
            self.cache.append(page_number) # append new one


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
    
            memref = MemRef(ip, op, addr)

            # send memory reference to simulator
            fifo.add_memtrace(memref);


# check args
if len(sys.argv) != 2:
    print("please check the usage...... python ./simulator.py [pinatrace.out]");
    sys.exit(1)


# FIFO simulator
fifo = FIFO()

# read file
read_memtrace(sys.argv[1]);

#print FIFO simulator results
fifo.print_result()





