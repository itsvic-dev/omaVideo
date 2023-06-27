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
  // read commands count
  uint8_t *count_raw = (g_funcs->fread)(4);
  uint32_t count = *count_raw;
  free(count_raw);

  struct omavideo_video_frame frame = {count, (g_funcs->fread)(count)};
  return frame;
}
