from page_policy import PagePolicy


class ARC(PagePolicy):
    """Sub class"""

    name = "ARC"
    # four lists
    # T1 + B1 : L1 cache
    # T2 + B2 : L2 cache
    # T1 + T2 < C
    # L1 + L2 < 2C
    T1 = []  
    T2 = [] 
    B1 = []
    B2 = []

    C = 0 # number of pages
    P = 0 # number of pages

    def update_P():
        B1_size = len(self.B1)
        B2_size = len(self.B2)

        if( B1_size >= B2_size ) 
            P = P+1
        else
            P = P + B2_size/B1_size

    def replace():
        if( T1_size != 0 and (T1_size > P or self.B2.index(page_number) and T1_size == P) ): # TODO : check range
            victim_page_number = self.T1.pop()
            self.B1.insert(0, victim_page_number)
        else 
            victim_page_number = self.T2.pop()
            self.B2.insert(0, victim_page_number)

    def add_memtrace(self, memref):
        page_number = self.get_page_number(memref)
        
        # page is in the cache(T1) : hit
        if page_number in self.T1:
            self.T1.remove(self.T1.page_number)
            self.T2.insert(0, page_number)
        
        # page is in the cache(T1) : hit
        else if page_number in self.T2:
            self.T2.remove(self.T2.page_number)
            self.T2.insert(0, page_number)

        # page is in the ghost_cache(B1) : miss
        else if page _number in self.B1:
            # remove page
            self.B1.remove(self.B1.page_number)
            # add page
            T1_size = len(T1)
            update_P()
            replace()
                            
            self.T2.insert(0, page_number)

        # page is in the ghost_cahce(B2) : miss
        else if page _number in self.B2:

            # page is not in both : miss
        else:

    def print_detailed_result(self):
        # TODO
        pass
