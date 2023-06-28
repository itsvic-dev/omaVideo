#include <core/core.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/panic.h>
#include <linux/panic_notifier.h>
#include <platform/platform.h>

MODULE_DESCRIPTION("omaVideo - Linux kernel frontend");
MODULE_AUTHOR("omame");
MODULE_LICENSE("GPL");

void func_log(char *section, char *format, ...) {
  char buffer[256];
  va_list list;
  va_start(list, format);
  vsnprintf(buffer, 256, format, list);
  va_end(list);
  printk(KERN_INFO "omaVideo: [%s] %s\n", section, buffer);
}

struct omavideo_platform_funcs funcs = {
    .log = *func_log,
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
