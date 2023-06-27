#include <core/core.h>
#include <platform/platform.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

FILE *l_fp;

void func_log(char *section, char *format, ...) {
  va_list list;
  va_start(list, format);
  printf("[%s] ", section);
  vprintf(format, list);
  printf("\n");
  va_end(list);
}

bool func_fopen() {
  l_fp = fopen("video.bin", "r");
  return l_fp == 0 ? false : true;
}

bool func_fclose() {
  if (l_fp == 0)
    return false;
  fclose(l_fp);
  return true;
}

uint8_t *func_fread(size_t count) {
  uint8_t *ret = malloc(count);
  fread(ret, 1, count, l_fp);
  return ret;
}

struct omavideo_platform_funcs linux_funcs = {
    .log = *func_log,
    .fopen = *func_fopen,
    .fclose = *func_fclose,
    .fread = *func_fread,
};

int main() {
  printf("staring omavideo...\n");
  return omavideo_init(&linux_funcs);
}
