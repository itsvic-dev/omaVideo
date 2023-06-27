#include <core/core.h>
#include <format/reader.h>
#include <stdlib.h>
#include <string.h>

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

  // read video header
  g_videoHeader = omavideo_format_read_header();
  (g_funcs->log)("core", "loaded %dx%d @ %d FPS video", g_videoHeader->width,
                 g_videoHeader->height, g_videoHeader->fps);

  // open display
  (g_funcs->log)("core", "opening display");
  (g_funcs->display_open)(g_videoHeader->width, g_videoHeader->height);

  // clean up
  (g_funcs->log)("core", "we're done, cleaning up");
  (g_funcs->display_close)();
  free(g_videoHeader);
  (g_funcs->fclose)();

  return 0;
}
