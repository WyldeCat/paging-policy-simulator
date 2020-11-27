from page_policy import PagePolicy


class LRU(PagePolicy):
    """Sub class"""

    name = "LRU"
    # TODO: Implement add_memtrace
    present_pages = []
    hit_counter = 0
    miss_counter = 0

    def add_memtrace(self, memref):
        page_number = self.get_page_number(memref)

        # if the page is not present
        if page_number not in self.present_pages:
            self.miss_counter += 1
            # if not full
            if len(self.present_pages) < self.max_page_entry_count:
                self.present_pages.append(page_number)
            # if full
            else:
                self.present_pages.remove(self.present_pages[0])
                self.present_pages.append(page_number)

        # if the page is present
        else:
            self.hit_counter += 1
            # move the page to the back
            self.present_pages.remove(page_number)
            self.present_pages.append(page_number)

    def print_detailed_result(self):
        # TODO
        pass
