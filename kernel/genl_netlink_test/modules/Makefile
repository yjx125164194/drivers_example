obj-m := genl_netlink.o

KDIR ?= /lib/modules/$(shell uname -r)/build

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
debug:
	$(MAKE) -C $(KDIR) M=$(PWD) EXTRA_CFLAGS+=-D__YUSUR_LOG_ EXTRA_CFLAGS+=-DDEBUG modules
install:
	$(MAKE) -C $(KDIR) M=$(PWD) modules_install
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f Module.markers Module.symvers

