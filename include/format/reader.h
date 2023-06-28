#pragma once
#include <core/types.h>

struct omavideo_video_header {
  char magic[8]; // "OMAVIDEO"
  u16 width;
  u16 height;
  u8 fps;
  u16 frame_count;
};

struct omavideo_video_header *omavideo_format_read_header();

struct omavideo_video_frame {
  u32 commands_count;
  u8 *commands;
};

struct omavideo_video_frame omavideo_format_read_frame();
