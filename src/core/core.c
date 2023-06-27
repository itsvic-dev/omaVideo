#include <core/core.h>
#include <stdlib.h>
#include <string.h>

struct omavideo_platform_funcs *g_funcs;

int omavideo_init(struct omavideo_platform_funcs *funcs) {
  g_funcs = funcs;
  (g_funcs->log)("core", "hello, omavideo world!");

  (g_funcs->log)("core", "opening file");
  if (!(g_funcs->fopen)()) {
    (g_funcs->log)("core", "failed to open file, bailing out");
    return 1;
  }

  // test shit
  (g_funcs->log)("core", "verifying magic header");
  uint8_t *magic = (g_funcs->fread)(8);
  if (strncmp((char *)magic, "OMAVIDEO", 8) == 0) {
    (g_funcs->log)("core", "magic header matches wow!!");
  }
  free(magic);

  (g_funcs->fclose)();

  (g_funcs->log)("core", "TODO: do shit");
  return 0;
}
