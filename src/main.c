#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("ArtillerySimulator v1.0!\r\n");
  printf("ARGS (%d): ", argc);

  for (int i = 0; i < argc; ++i) {
    if (i > 0)
      printf(", ");
    printf("%s", argv[i]);
  }
  printf("\r\n");

  return 0;
}
