#pragma once
#include <format/reader.h>

#define CMD_MOVE 0x01
#define CMD_INC_BY 0x02
#define CMD_INC 0x03

#define CMD_DRAW 0x10
#define CMD_INVERT 0x11
#define CMD_FILL 0x12
#define CMD_FILL_DATA 0x13

extern u8 *g_framebuffer;

extern void omavideo_renderer_init(void);
extern void omavideo_renderer_render_frame(struct omavideo_video_frame *frame);
