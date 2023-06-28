#pragma once
#include <core/types.h>

struct omavideo_platform_funcs {
  void (*log)(char *section, char *format, ...);
  void (*msleep)(unsigned long msecs);
  long long (*get_ms_time)();

  // memory management
  void *(*malloc)(u64 size);
  void (*free)(void *ptr);
  void *(*memset)(void *dest, int value, u64 size);
  int (*strncmp)(const char *str1, const char *str2, u64 n);

  // file pointer
  bool (*fopen)(void);
  u8 *(*fread)(u64 count);
  bool (*fclose)(void);

  // display
  bool (*display_open)(int width, int height);
  void (*display_frame)(u8 *frame_data);
  bool (*display_close)(void);
};
