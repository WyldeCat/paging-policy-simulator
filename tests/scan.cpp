#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main( ) {
  constexpr size_t mem_size = 4096L * 1024L * 1024L;
  constexpr size_t buf_size = mem_size + 4 * 1024;
  constexpr size_t num_pages = buf_size / 4096;

  printf("scan start!\n");

  char *tmp = (char *)malloc(buf_size);
  printf("malloc compl\n");

  memset(tmp, 0, buf_size);
  printf("memset compl\n");

  for (int i = 0; i < 100; i++) {
    printf("i : %d\n", i);
    for (int j = 0; j < num_pages; j++) {
      tmp[j * 4096] += 1;
    }
  }
}
