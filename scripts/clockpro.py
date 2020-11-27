from page_policy import PagePolicy


class CLOCKPRO(PagePolicy):
    """Sub class"""

    name = "CLOCKPRO"
    # TODO: Implement add_memtrace
    cache = []

    def add_memtrace(self, memref):
        page_number = self.get_page_number(memref)

        # first reference page
        if not page_number in self.cache:
            self.miss_counter += 1
            # if len(cache) < max_page_entry_count: # there is space
            if len(self.cache) < self.max_page_entry_count:  # there is space
                self.cache.append(page_number)  # append ref at end of the list
            else:  # full
                print("cache full")
                self.cache.pop(0)  # evict first element
                self.cache.append(page_number)  # append at end of the list
        else:  # page in the list
            self.hit_counter += 1
            self.cache.pop(self.cache.index(page_number))  # pop the old one
            self.cache.append(page_number)  # append new one

    def print_detailed_result(self):
        # TODO
        pass
