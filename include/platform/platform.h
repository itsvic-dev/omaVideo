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

  // framebuffer
  void (*open_fb)(void);
  void (*render_frame)(char *frame_data);
  void (*close_fb)(void);
};
