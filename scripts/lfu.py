from page_policy import PagePolicy


class LFU(PagePolicy):
    """Sub class"""

    name = "LFU"
    freq_for_pages = dict()

    def add_memtrace(self, memref):
        page_number = self.get_page_number(memref)

        # if page is not present
        if page_number not in self.freq_for_pages:
            self.miss_counter += 1
            # if not full
            if len(self.freq_for_pages) < self.max_page_entry_count:
                self.freq_for_pages[page_number] = 1

            else:  # full
                least_used_page = min(self.freq_for_pages, self.freq_for_pages.get)
                del self.freq_for_pages[least_used_page]
                self.freq_for_pages[page_number] = 1

        else:  # page in the list
            self.hit_counter += 1
            self.freq_for_pages[page_number] += 1

    def print_detailed_result(self):
        # TODO
        pass
