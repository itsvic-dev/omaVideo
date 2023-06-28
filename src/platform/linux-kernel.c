#include <asm-generic/fcntl.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <core/core.h>
#include <linux/buffer_head.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/fcntl.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/limits.h>
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/panic.h>
#include <linux/panic_notifier.h>
#include <platform/platform.h>

MODULE_DESCRIPTION("omaVideo - Linux kernel frontend");
MODULE_AUTHOR("omame");
MODULE_LICENSE("GPL");

#define FILE_PATH "/tmp/video.bin"
#define FB_WIDTH 1152 // i dont have a mechanism for detecting fb width lol!!

void func_log(char *section, char *format, ...) {
  char buffer[256];
  va_list list;
  va_start(list, format);
  vsnprintf(buffer, 256, format, list);
  va_end(list);
  printk(KERN_INFO "omaVideo: [%s] %s\n", section, buffer);
}

struct file *filp = NULL;

bool func_fopen(void) {
  filp = filp_open(FILE_PATH, O_RDONLY, 0);
  if (IS_ERR(filp)) {
    filp = NULL;
    return FALSE;
  }

  return TRUE;
}

bool func_fclose(void) {
  if (filp == NULL)
    return FALSE;
  filp_close(filp, NULL);
  return TRUE;
}

u8 *func_fread(u64 count) {
  u8 *ret = kzalloc(count, GFP_KERNEL);
  loff_t pos = filp->f_pos;
  kernel_read(filp, ret, count, &pos);
  filp->f_pos = pos;
  return ret;
}

void *func_malloc(u64 size) { return kzalloc(size, GFP_KERNEL); }

void func_free(void *ptr) { return kfree(ptr); }

int func_strncmp(const char *str1, const char *str2, u64 n) {
  return strncmp(str1, str2, n);
}

void *func_memset(void *dest, int value, u64 size) {
  return memset(dest, value, size);
}

struct file *fbfp;
int l_width;
int l_height;
u8 *display_buf;
bool func_display_open(int width, int height) {
  fbfp = filp_open("/dev/fb0", O_RDWR, 0);
  l_width = width;
  l_height = height;
  display_buf = kzalloc(FB_WIDTH * height * 4, GFP_KERNEL);

  if (IS_ERR(fbfp)) {
    pr_warn("omaVideo: failed to open /dev/fb0");
    return FALSE;
  }

  pr_warn(
      "omaVideo: TODO: framebuffer width detection, using hardcoded value %d",
      FB_WIDTH);
  // fbfp->f_op->unlocked_ioctl(fbfp, FBIOGET_VSCREENINFO,
  //                            (unsigned long)(&vinfo));
  // if (vinfo.bits_per_pixel != 32) {
  //   pr_info("omaVideo: framebuffer has %d BPP, expected 32",
  //           vinfo.bits_per_pixel);
  //   return FALSE;
  // }

  return TRUE;
}
void func_display_frame(u8 *frame_data) {
  for (int y = 0; y < l_height; y++) {
    for (int x = 0; x < l_width; x++) {
      int idx = y * l_width + x;
      display_buf[idx * 4] = frame_data[idx];
    }
  }
  // kernel_write(fbfp, display_buf, FB_WIDTH * l_height * 4, 0);
  fbfp->f_op->write(fbfp, display_buf, FB_WIDTH * l_height * 4, 0);
}
bool func_display_close(void) {
  filp_close(fbfp, NULL);
  return TRUE;
}

void func_msleep(unsigned long msecs) {
  // msleep(msecs);
}

long long func_get_ms_time(void) {
  // TODO
  return 0;
}

struct omavideo_platform_funcs funcs = {
    .log = *func_log,
    .msleep = *func_msleep,
    .get_ms_time = *func_get_ms_time,

    .malloc = *func_malloc,
    .free = *func_free,
    .memset = *func_memset,
    .strncmp = *func_strncmp,

    .fopen = *func_fopen,
    .fread = *func_fread,
    .fclose = *func_fclose,

    .display_open = *func_display_open,
    .display_frame = *func_display_frame,
    .display_close = *func_display_close,
};

static int omavideo_panic_handler(struct notifier_block *self, unsigned long i,
                                  void *buf) {
  pr_emerg("omaVideo: look mom i'm in a panic!!\n");
  omavideo_init(&funcs);
  return 0;
}

struct notifier_block omavideo_panic_block = {
    .notifier_call = *omavideo_panic_handler,
};

static int omavideo_module_init(void) {
  pr_info("omaVideo: hello\n");

  atomic_notifier_chain_register(&panic_notifier_list, &omavideo_panic_block);
  pr_info("omaVideo: added panic handler\n");
  return 0;
}

static void omavideo_module_exit(void) { pr_info("bye\n"); }

module_init(omavideo_module_init);
module_exit(omavideo_module_exit);
