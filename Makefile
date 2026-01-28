# 这是一个通用的内核模块 Makefile
# 只要把 obj-m := 后面的名字改成你的 .c 文件名(.o)即可

obj-m := mem_test.o

# 获取当前内核的构建目录
KDIR := /lib/modules/$(shell uname -r)/build

#以此目录作为当前路径
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
