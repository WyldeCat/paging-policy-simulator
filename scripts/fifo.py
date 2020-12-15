from page_policy import PagePolicy


class FIFO(PagePolicy):
    """Sub class"""

    name = "FIFO"
    cache = []

    def add_memtrace(self, memref):
        page_number = self.get_page_number(memref)

        if page_number not in self.cache: # page is not in the list
            self.miss_counter += 1
            if len(self.cache) < self.max_page_count:  # there is space
                self.cache.append(page_number)  # append ref at end of the list
            else:  # full
                self.cache.pop(0)  # evict first element
                self.cache.append(page_number)  # append at end of the list
        else:  # page is in the list
            self.hit_counter += 1
            self.cache.pop(self.cache.index(page_number))  # pop the old one
            self.cache.append(page_number)  # append new one

    def print_detailed_result(self):
        # TODO
        pass
