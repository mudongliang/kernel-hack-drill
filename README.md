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
cd linux-5.8.9
make x86_64_defconfig
```

```
wget https://github.com/torvalds/linux/archive/v5.0-rc1.tar.gz
tar -xvf v5.0-rc1.tar.gz
cd linux-v5.0-rc1
make x86_64_defconfig
```

Note that linux-5.0-rc1 is vulnerable to [Null Pointer Dereference exploit][1]. 

2. Prepare Debian Image for QEMU VM

```
cd image
./create_wheezy.sh
sudo rm -rf chroot
```
We create Debian Wheezy image by default as it can directly work well with defconfig.

If you want to create Debian Stretch image or higher with `[create-image.sh][2]`, the following configuration is required:

```
# Required for Debian Stretch
CONFIG_CONFIGFS_FS=y
CONFIG_SECURITYFS=y
```

3. Start QEMU VM and do some configuration at Terminal 1

```
ln -s linux-5.8.9 linux
./startvm
```

Create soft link with the target Linux kernel, enter `root` and empty password to log in, and then

```
useradd -m -s /bin/bash drill
mkdir -p /home/drill/.ssh/
cp .ssh/authorized_keys /home/drill/.ssh/authorized_keys
chown drill:drill /home/drill/.ssh /home/drill/.ssh/authorized_keys
```

3. Copy exploits and scripts into QEMU VM at Terminal 2

```
make
./scptovm prep_usr
./scptovm drill_operations drill_trigger_crash drill_exploit_uaf drill_exploit_nullderef 
```

4. Remount `/sys/kernel/debug` at Terminal 1

```
mv /home/drill/prep_usr .
./prep_usr
```

The execution result is as below:
```
+ umount /sys/kernel/debug
+ mount -o uid=1000,gid=1000 -t debugfs none /sys/kernel/debug
+ insmod drill_mod.ko
[   41.445370] drill_mod: loading out-of-tree module taints kernel.
[   41.449360] drill: start hacking
```

5. Execute exploits at Terminal 2

```
./connect_vm
drill@syzkaller:~$./drill_operations
drill@syzkaller:~$./drill_trigger_crash
drill@syzkaller:~$./drill_exploit_uaf
drill@syzkaller:~$./drill_exploit_nullderef 
```

## References

[1]: https://bugs.chromium.org/p/project-zero/issues/detail?id=1792&desc=2
[2]: https://github.com/google/syzkaller/blob/master/tools/create-image.sh
