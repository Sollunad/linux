#include "fdtable.h"

const char *files[1000];

int FD_OFFSET = 10000;

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
	return i + FD_OFFSET;
}

ssize_t fdt_read(unsigned int fd, char __user *buf, size_t count) {
	const char* filename = _fdt_get_filename(fd);
	printk("read file %s\n", filename);
	return count;
}

bool fdt_is_responsible(unsigned int fd) {
	if (fd < FD_OFFSET) {
		return false;
	}
	return _fdt_get_filename(fd) != NULL;
}

