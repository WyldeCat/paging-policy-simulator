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

        if( B1_size >= B2_size ):
            P = P + 1
        else:
            P = P + B2_size/B1_size

    def replace(page_number):
        T1_size = len(T1)
        if( T1_size != 0 and (T1_size > P or self.B2.count(page_number) != 0 and T1_size == P) ):
            # move page : T1 --> B1 
            victim_page_number = self.T1.pop() # delete the LRU page in T2
            self.B1.insert(0, victim_page_number) # move it to MRU position in B1
        else:
            # move page : T2 --> B2 
            victim_page_number = self.T2.pop() # delete the LRU page in T2
            self.B2.insert(0, victim_page_number) # move it to MRU position in B2

    def add_memtrace(self, memref):
        page_number = self.get_page_number(memref)
        
        # case 1) page is in the cache(T1) : hit
        if page_number in self.T1:
            # move page : T1 --> T2
            self.T1.remove(page_number)
            self.T2.insert(0, page_number)
        
        # case 2) page is in the cache(T1) : hit
        elif (page_number in self.T2):
            # move page : T2 --> T2
            self.T2.remove(page_number)
            self.T2.insert(0, page_number)

        # case 3) page is in the ghost_cache(B1) : miss
        elif (page_number in self.B1):
            # adaptation : update P
            increase_P()
            # replace page
            replace()
            # move page : B1 --> T2  
            self.B1.remove(page_number)
            self.T2.insert(0, page_number)

        # case 4) page is in the ghost_cahce(B2) : miss
        elif page_number in self.B2:
            # adaptation : update P
            decrease_P()
            # replace page
            replace()
            # move page : B2 --> T2  
            self.B2.remove(page_number)
            self.T2.insert(0, page_number)

        # case 5) page is not in both : miss
        else:
            T1_size = len(self.T1)
            T2_size = len(self.T2)
            B1_size = len(self.B1)
            B2_size = len(self.B2)
            L1_size = T1_size + B1_size

            # if L1(T1+B1) has exactly c pages
            if( L1_size == C ):
                if( T1_size < C ):
                    # delete LRU page in B1
                    self.B1.remove(page_number)
                    replace()
                else:
                    # B1 is empty
                    self.T1.remove(page_number)

            # if L1(T1+B1) has less than c pages
            elif( L1_size < C ):
                total_size = T1_size + T2_size + B1_size + B2_size;
                if( total_size >= C ):
                    if( total_size == 2*C ):
                        # delete LRU page in B2 
                        self.B2.remove(page_number)
                    replace()

            # move it to MRU position in T1
            self.T1.insert(0, page_number)


    def print_detailed_result(self):
        # TODO
        pass
