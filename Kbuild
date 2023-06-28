obj-m := omaVideo.o
omaVideo-y := src/platform/linux-kernel.o

EXTRA_CFLAGS := -g -I$(src)/include

# src/core
omaVideo-y += src/core/core.o
# src/format
omaVideo-y += src/format/reader.o src/format/renderer.o
