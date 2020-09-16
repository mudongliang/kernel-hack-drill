# Linux kernel exploitation experiments

In this project, I will leverage QEMU VM as the environment for the Linux kernel exploitation experiments.

__Contents:__

  - __drill_mod.c__ - a small Linux kernel module with nice vulnerabilities.
  You can interact with it via a simple debugfs interface.
  - __drill_exploit_uaf.c__ - a basic use-after-free exploit.
  - __drill_exploit_nullderef.c__ - a basic null-ptr-deref exploit, which uses 
  wonderful [mmap_min_addr bypass][1] by Jann Horn.

Have fun!

## Preparation

1. Compile Linux Kernel

```
wget https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-5.8.9.tar.xz
tar -xvf linux-5.8.9.tar.xz
mv linux-5.8.9 linux
cd linux
make x86_64_defconfig
```

2. Prepare Debian Image for QEMU VM

```
cd image
./create_image.sh -d wheezy
sudo rm -rf chroot
```

3. Start QEMU VM at Terminal 1

```
./startvm
useradd -m -s /bin/bash drill
cp .ssh/authorized_keys /home/drill/.ssh/authorized_keys
chmod drill:drill .ssh/authorized_keys
```

3. Copy exploits into QEMU VM at Terminal 2

```
make
./scptovm prep_usr.sh
./scptovm drill_operations drill_trigger_crash drill_exploit_uaf drill_exploit_nullderef 
```

4. Remount `/sys/kernel/debug` at Terminal 1

Enter `root` and empty password to log in, then

```
mv /home/drill/prep_usr.sh .
./prep_usr.sh
```

The execution result is as below:
```
+ umount /sys/kernel/debug
+ mount -o uid=1000,gid=1000 -t debugfs none /sys/kernel/debug
+ insmod drill_mod.ko
[   41.445370] drill_mod: loading out-of-tree module taints kernel.
[   41.449360] drill: start hacking
```

5. Execute the exploits at Terminal 2

```
./connect_vm
drill@syzkaller:~$./drill_operations
drill@syzkaller:~$./drill_trigger_crash
drill@syzkaller:~$./drill_exploit_uaf
drill@syzkaller:~$./drill_exploit_nullderef 
```

## References

[1]: https://bugs.chromium.org/p/project-zero/issues/detail?id=1792&desc=2
