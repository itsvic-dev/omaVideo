#include <core/core.h>
#include <format/reader.h>
#include <stdlib.h>
#include <string.h>

struct omavideo_video_header *omavideo_format_read_header() {
  // read header
  struct omavideo_video_header *header =
      (struct omavideo_video_header *)(g_funcs->fread)(
          sizeof(struct omavideo_video_header));

  // make sure magic matches
  if (strncmp(header->magic, "OMAVIDEO", 8) != 0) {
    (g_funcs->log)("format/reader", "magic doesn't match, bail");
    free(header);
    return NULL;
  }

  return header;
}

struct omavideo_video_frame omavideo_format_read_frame() {
  uint8_t *header = (g_funcs->fread)(4);
  if (strncmp((char *)header, "OMFR", 4) != 0) {
    (g_funcs->log)("format/reader", "frame magic doesn't match: %x %x %x %x",
                   header[0], header[1], header[2], header[3]);
    struct omavideo_video_frame crash_frame = {10, NULL};
    return crash_frame;
  }
  // read commands count
  uint8_t *count_raw = (g_funcs->fread)(4);
  uint32_t count = *((uint32_t *)count_raw);
  free(count_raw);

  uint8_t *data;
  if (count == 0) {
    data = NULL;
  } else {
    data = (g_funcs->fread)(count);
  }

  struct omavideo_video_frame frame = {count, data};
  return frame;
}
