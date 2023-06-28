#pragma once
#include <core/types.h>

struct omavideo_video_header {
  char magic[8]; // "OMAVIDEO"
  uint16_t width;
  uint16_t height;
  uint8_t fps;
  uint16_t frame_count;
};

struct omavideo_video_header *omavideo_format_read_header();

struct omavideo_video_frame {
  uint32_t commands_count;
  uint8_t *commands;
};

struct omavideo_video_frame omavideo_format_read_frame();
