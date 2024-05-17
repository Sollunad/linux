#include "fdtable.h"

const char *files[1000];

const int FD_OFFSET = 10000;

static const char * _fdt_get_filename(unsigned int fd) {
	return files[fd - FD_OFFSET];
}

int fdt_fstat(unsigned int fd, struct kstat *stat) {
	printk("stat fd %d", fd);
	return 0;
}

int fdt_open(const char *filename) {
	int i = 0;
	while (files[i] != NULL) {
		i++;
	}
	files[i] = filename;
	printk("open %s under fd %d\n", filename, i + FD_OFFSET);
	return i + FD_OFFSET;
}

ssize_t fdt_read(unsigned int fd, char __user *buf, size_t count) {
	printk("read fd %d with count %zu\n", fd, count);
	const char* filename = _fdt_get_filename(fd);
	printk("read file %s\n", filename);
	char dev_response[1024];
	int res_len = request_device("read", filename, NULL, dev_response);
	int status = copy_to_user(buf, dev_response, res_len);
	if (status) {
		printk("Error while copying to user buffer\n");
		return -status;
	}
	return count;
}

bool fdt_is_responsible(unsigned int fd) {
	if (fd < FD_OFFSET) {
		return false;
	}
	return _fdt_get_filename(fd) != NULL;
}

