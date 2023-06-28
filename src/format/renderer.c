#include <core/core.h>
#include <format/renderer.h>

u8 *g_framebuffer;

void omavideo_renderer_init() {
  (g_funcs->log)("format/renderer", "initializing");
  g_framebuffer =
      (g_funcs->malloc)(g_videoHeader->width * g_videoHeader->height);
}

void omavideo_renderer_render_frame(struct omavideo_video_frame *frame) {
  int p = 0;
  int idx = 0;
  u8 *cmds = frame->commands;
  if (cmds == NULL)
    return;

  while (p < frame->commands_count) {
    // (g_funcs->log)("format/renderer", "p=%d cmds[p]=%d", p, cmds[p]);
    switch (cmds[p]) {
    case CMD_MOVE: {
      u16 x = *((u16 *)&cmds[++p]);
      p++;
      u16 y = *((u16 *)&cmds[++p]);
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
      (g_funcs->memset)(g_framebuffer, cmds[++p],
                        g_videoHeader->width * g_videoHeader->height);
      break;
    }
    /* case CMD_FILL_DATA: {
      // not used and not supported for now
      memcpy(g_framebuffer, &cmds[++p],
             g_videoHeader->width * g_videoHeader->height);
      p += g_videoHeader->width * g_videoHeader->height;
      break;
    } */
    default:
      (g_funcs->log)("format/renderer", "unknown command %d at p=%d", cmds[p],
                     p);
    }
    p++;
  }

  // free the frame data once we're done with it to not pollute memory
  (g_funcs->free)(frame->commands);
}
