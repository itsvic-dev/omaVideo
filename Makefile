KDIR = /lib/modules/`uname -r`/build
BUILD_DIR=$(PWD)/build

.PHONY: kbuild
kbuild:
	@$(MAKE) -C $(KDIR) M=$(BUILD_DIR) src=$(PWD)

.PHONY: clean
clean:
	@$(MAKE) -C $(KDIR) M=$(BUILD_DIR) src=$(PWD) clean
