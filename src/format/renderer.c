#include <core/core.h>
#include <format/renderer.h>
#include <stdlib.h>
#include <string.h>

uint8_t *g_framebuffer;

void omavideo_renderer_init() {
  (g_funcs->log)("format/renderer", "initializing");
  g_framebuffer = malloc(g_videoHeader->width * g_videoHeader->height);
}

void omavideo_renderer_render_frame(struct omavideo_video_frame *frame) {
  int p = 0;
  int idx = 0;
  uint8_t *cmds = frame->commands;
  if (cmds == NULL)
    return;

  while (p < frame->commands_count) {
    // (g_funcs->log)("format/renderer", "p=%d cmds[p]=%d", p, cmds[p]);
    switch (cmds[p]) {
    case CMD_MOVE: {
      uint16_t x = *((uint16_t *)&cmds[++p]);
      p++;
      uint16_t y = *((uint16_t *)&cmds[++p]);
      p++;
      idx = y * g_videoHeader->width + x;
      break;
    }
    case CMD_INC_BY: {
      idx += cmds[++p];
      break;
    }
    case CMD_INC: {
      // techically not used, but we support it anyway
      idx++;
      break;
    }

    case CMD_DRAW: {
      g_framebuffer[idx] = cmds[++p];
      idx++;
      break;
    }
    case CMD_INVERT: {
      g_framebuffer[idx] = 0xFF - g_framebuffer[idx];
      idx++;
      break;
    }
    case CMD_FILL: {
      memset(g_framebuffer, cmds[++p],
             g_videoHeader->width * g_videoHeader->height);
      break;
    }
    case CMD_FILL_DATA: {
      // technically not used, but we support it anyway
      memcpy(g_framebuffer, &cmds[++p],
             g_videoHeader->width * g_videoHeader->height);
      p += g_videoHeader->width * g_videoHeader->height;
      break;
    }
    default:
      (g_funcs->log)("format/renderer", "unknown command %d at p=%d", cmds[p],
                     p);
    }
    p++;
  }

  // free the frame data once we're done with it to not pollute memory
  free(frame->commands);
}
