#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct omavideo_platform_funcs {
  void (*log)(char *section, char *format, ...);
  void (*msleep)(unsigned long msecs);
  long long (*get_ms_time)();

  // memory management
  void *(*malloc)(size_t size);
  void (*free)(void *ptr);
  void *(*memset)(void *dest, int value, size_t size);
  int (*strncmp)(const char *str1, const char *str2, size_t n);

  // file pointer
  bool (*fopen)(void);
  uint8_t *(*fread)(size_t count);
  bool (*fclose)(void);

  // display
  bool (*display_open)(int width, int height);
  void (*display_frame)(uint8_t *frame_data);
  bool (*display_close)(void);
};
