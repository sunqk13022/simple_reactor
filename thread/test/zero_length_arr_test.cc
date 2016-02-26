#include <stdlib.h>
#include <string.h>

struct line {
  int length;
  char contents[0];
};

int main() {
  int len = 10;
  struct line* thisline = (struct line*) malloc(sizeof(struct line) + len);
  thisline->length = len;
  memset(thisline->contents, 'a', len);
  return 0;
}
