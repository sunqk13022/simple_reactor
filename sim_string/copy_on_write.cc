#include <string>
#include <stdio.h>

int main() {
  std::string str1 = "NewGGGGGG";
  std::string str2 = str1;
  printf("Sharing the memory:\n");
  printf("\tstr1 address:%x\n", str1.c_str());
  printf("\tstr2 address:%x\n", str2.c_str());
  str1[1] = 'q';
  str2[1] = 'q';
  printf("After Copy on write the memory:\n");
  printf("\tstr1 address:%x\n", str1.c_str());
  printf("\tstr2 address:%x\n", str2.c_str());
  return 0;
}
