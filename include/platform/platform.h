#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct omavideo_platform_funcs {
  void (*log)(char *section, char *format, ...);

  // file pointer
  bool (*fopen)(void);
  uint8_t *(*fread)(size_t count);
  bool (*fclose)(void);

  // display
  bool (*display_open)(int width, int height);
  void (*display_frame)(uint8_t *frame_data);
  bool (*display_close)(void);
};
