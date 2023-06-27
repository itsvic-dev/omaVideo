#pragma once
#include <platform/platform.h>

extern int omavideo_init(struct omavideo_platform_funcs *funcs);

extern struct omavideo_platform_funcs *g_funcs;
