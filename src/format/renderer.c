#include <core/core.h>
#include <format/renderer.h>

uint8_t *g_framebuffer;

void omavideo_renderer_init() {
  (g_funcs->log)("format/renderer", "initializing");
  g_framebuffer =
      (g_funcs->malloc)(g_videoHeader->width * g_videoHeader->height);
}

void omavideo_renderer_render_frame(struct omavideo_video_frame *frame) {
  int p = 0;
  int idx = 0;
  uint8_t *cmds = frame->commands;
  if (cmds == NULL)
    return;

  while (p < frame->commands_count) {
    // (g_funcs->log)("format/renderer", "p=%d cmds[p]=%x", p, cmds[p]);
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
    case CMD_REPEAT: {
      uint8_t size = cmds[++p];
      // if N isn't 0
      if (cmds[++p] != 0) {
        // (g_funcs->log)("format/renderer", "repeating cmd (size=%d) %d times",
        //                size, cmds[p]);
        cmds[p]--;
        // move p back
        p -= 2 + size + 1; // +1 because we're incrementing p at the end
      }
      break;
    }
    default:
      (g_funcs->log)("format/renderer", "unknown command %x at p=%d", cmds[p],
                     p);
    }
    p++;
  }

  // free the frame data once we're done with it to not pollute memory
  (g_funcs->free)(frame->commands);
}
