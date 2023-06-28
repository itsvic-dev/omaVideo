#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

MODULE_DESCRIPTION("omaVideo - Linux kernel frontend");
MODULE_AUTHOR("omame");
MODULE_LICENSE("GPL");

static int omavideo_module_init(void) {
  pr_debug("hello\n");
  return 0;
}

static void omavideo_module_exit(void) { pr_debug("bye\n"); }

module_init(omavideo_module_init);
module_exit(omavideo_module_exit);
