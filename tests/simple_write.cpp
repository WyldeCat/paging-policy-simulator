#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

int main( )
{
  const size_t bytes_to_read = 4096 * 4;

  FILE *fp = fopen("/bin/bash", "rb");

  sleep(5);

  char *tmp = (char *)malloc(bytes_to_read);
  fprintf(stderr, "tmp : %p\n", tmp);

  sleep(5);

  size_t ret = fread(tmp, 1, bytes_to_read, fp);
  assert(ret == bytes_to_read);

  fprintf(stderr, "done\n");
}
