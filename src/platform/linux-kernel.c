#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/panic.h>
#include <linux/panic_notifier.h>

MODULE_DESCRIPTION("omaVideo - Linux kernel frontend");
MODULE_AUTHOR("omame");
MODULE_LICENSE("GPL");

static int omavideo_panic_handler(struct notifier_block *self, unsigned long i,
                                  void *buf) {
  pr_emerg("look mom i'm in a panic!!\n");
  return 0;
}

struct notifier_block omavideo_panic_block = {
    .notifier_call = *omavideo_panic_handler,
};

static int omavideo_module_init(void) {
  pr_debug("hello\n");

  atomic_notifier_chain_register(&panic_notifier_list, &omavideo_panic_block);
  pr_debug("added panic handler\n");
  return 0;
}

static void omavideo_module_exit(void) { pr_debug("bye\n"); }

module_init(omavideo_module_init);
module_exit(omavideo_module_exit);
