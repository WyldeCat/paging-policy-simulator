class PagePolicy:
    """ Super Class"""

    memory_size = 0
    page_frame_size = 4  # 4KB
    max_page_count = 0
    hit_counter = 0
    miss_counter = 0
    name = "NONE"

    def __init__(self, memory_size):
        self.memory_size = memory_size
        self.max_page_count = self.memory_size/self.page_frame_size

    def get_page_number(self, memref):
        """hex to binary"""
        addr = int(memref.addr, 16)  # 48 bit
        # print(bin(addr))
        addr = addr >> 12  # block offet 12 bit cut
        return addr

    def add_memtrace(self, memref):
        pass

    def get_hit_counter(self):
        return self.hit_counter

    def get_miss_counter(self):
        return self.miss_counter

    def print_detailed_result():
        pass

    def print_result(self):
        print("-paging simulator result Policy %s" % (self.name))

        print("-hit counter : %d, miss counter : %d\n" %
              (self.hit_counter, self.miss_counter))
        self.print_detailed_result()
