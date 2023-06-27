#include <X11/X.h>
#include <X11/Xlib.h>
#include <core/core.h>
#include <platform/platform.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

FILE *l_fp;

void func_log(char *section, char *format, ...) {
  va_list list;
  va_start(list, format);
  printf("[%s] ", section);
  vprintf(format, list);
  printf("\n");
  va_end(list);
}

bool func_fopen() {
  l_fp = fopen("video.bin", "r");
  return l_fp == 0 ? false : true;
}

bool func_fclose() {
  if (l_fp == 0)
    return false;
  fclose(l_fp);
  return true;
}

uint8_t *func_fread(size_t count) {
  uint8_t *ret = malloc(count);
  fread(ret, 1, count, l_fp);
  return ret;
}

Display *l_display;
Window l_window;

bool func_display_open(int width, int height) {
  l_display = XOpenDisplay(NULL);
  if (l_display == NULL) {
    printf("[linux] failed to open X display\n");
    return false;
  }
  int s = DefaultScreen(l_display);

  l_window = XCreateSimpleWindow(l_display, RootWindow(l_display, s), 0, 0,
                                 width, height, 1, BlackPixel(l_display, s),
                                 WhitePixel(l_display, s));

  XSelectInput(l_display, l_window, ExposureMask);
  Atom WM_NAME = XInternAtom(l_display, "WM_NAME", false);
  Atom STRING = XInternAtom(l_display, "STRING", false);
  XChangeProperty(l_display, l_window, WM_NAME, STRING, 8, PropModeReplace,
                  (unsigned char *)"omaVideo\0", 9);
  XMapWindow(l_display, l_window);

  XEvent event;
  XNextEvent(l_display, &event);

  return true;
}

bool func_display_close() {
  if (l_display == NULL) {
    return false;
  }

  XCloseDisplay(l_display);
  return true;
}

struct omavideo_platform_funcs linux_funcs = {
    .log = *func_log,

    .fopen = *func_fopen,
    .fclose = *func_fclose,
    .fread = *func_fread,

    .display_open = *func_display_open,
    .display_close = *func_display_close,
};

int main() {
  printf("staring omavideo...\n");
  return omavideo_init(&linux_funcs);
}
