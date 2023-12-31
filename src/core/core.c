#include <core/core.h>
#include <format/reader.h>
#include <format/renderer.h>

struct omavideo_platform_funcs *g_funcs;
struct omavideo_video_header *g_videoHeader;

int omavideo_init(struct omavideo_platform_funcs *funcs) {
  g_funcs = funcs;
  (g_funcs->log)("core", "hello, omavideo world!");

  (g_funcs->log)("core", "opening file");
  if (!(g_funcs->fopen)()) {
    (g_funcs->log)("core", "failed to open file, bailing out");
    return 1;
  }

  (g_funcs->log)("core", "reading file header");
  // read video header
  g_videoHeader = omavideo_format_read_header();
  (g_funcs->log)("core", "loaded %dx%d @ %d FPS video", g_videoHeader->width,
                 g_videoHeader->height, g_videoHeader->fps);

  // open display
  (g_funcs->log)("core", "opening display");
  if (!(g_funcs->display_open)(g_videoHeader->width, g_videoHeader->height)) {
    (g_funcs->log)("core", "failed to open display, bailing out");
    return 1;
  }

  omavideo_renderer_init();

  for (int frame_idx = 0; frame_idx < g_videoHeader->frame_count; frame_idx++) {
    long long frame_start = (g_funcs->get_ms_time)();

    struct omavideo_video_frame frame = omavideo_format_read_frame();
    omavideo_renderer_render_frame(&frame);
    (g_funcs->display_frame)(g_framebuffer);

    long long frame_end = (g_funcs->get_ms_time)();
    (g_funcs->msleep)(1000 / g_videoHeader->fps - (frame_end - frame_start));
  }

  // clean up
  (g_funcs->log)("core", "we're done, cleaning up");
  (g_funcs->display_close)();
  (g_funcs->free)(g_videoHeader);
  (g_funcs->fclose)();

  return 0;
}
