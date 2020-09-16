KPATH := ${PWD}/linux/

obj-m := drill_mod.o

all: drill_operations drill_trigger_crash drill_exploit_uaf drill_exploit_nullderef
	make -C $(KPATH) M=$(PWD) modules

clean:
	make -C $(KPATH) M=$(PWD) clean
	rm drill_operations drill_trigger_crash drill_exploit_uaf drill_exploit_nullderef
